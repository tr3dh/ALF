#include "RejectionSampling.h"

void seedFloatGenerator(const float& seed){

    static bool seeded = false;

    if(!seeded){

        LOG << "** Seeded rd Float Gen with " << (seed == 0 ? "default" : "custom") << " Seed " << std::time(nullptr) << ENDL; 
        std::srand(static_cast<unsigned int>((seed == 0 ? std::time(nullptr) : seed)));
        seeded = true;
        return;
    }

    LOG << "** Continue using gen seed " << ENDL;
}

// Gibt Zufalls Float im Intervall [a, b] zurück
// rand gibt zahl zwischen 0 und RAND_MAX zurück
// wird über die Division mit dem Limit auf Wert zwischen null und 1 normalisiert und dann über (b - a) auf
// zahlenbereich gestreckt und mit minimum a geoffsettet
float randFloat(float a, float b) {
    return a + (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) * (b - a));
}

std::array<float, 4> preprocessPDF(const Expression& pdensity, const float& xi_min, const float& xi_max, const float& tolerance, const float& segmentation){

    //
    float leftBorder = 0, rightBorder = 0;
    float maxP_Xi = 0, max_Xi = 0;

    float currentXi = 0;
    float p_xi = 2 * tolerance;

    currentXi = 0;
    if(SymEngine::eval_double(*pdensity->subs({{xi, toExpression(currentXi)}})) == 0){
        return {0,0,0,0};
    };

    if(xi_min > 0){ currentXi = xi_min; }
    while(p_xi > tolerance && (xi_max != 0 ? currentXi < xi_max : 1) && xi_max >= 0){

        // Punkt auf pdensity ermitteln                                                  // Stelle xi
        p_xi = SymEngine::eval_double(*pdensity->subs({{xi, toExpression(currentXi)}}));   // Funktionswert

        // Check ob Maximum übertroffen wurde
        if(p_xi > maxP_Xi){
            maxP_Xi = p_xi;
            max_Xi = currentXi;
        }

        //
        currentXi += segmentation;
    }

    if(xi_max == 0){
        rightBorder = currentXi - segmentation;
    } else {
        // nach abbruch zuweisen
        rightBorder = (currentXi < xi_max ? currentXi - segmentation : xi_max);
    }

    currentXi = 0;
    p_xi = 2 * tolerance;

    if(xi_max < 0){ currentXi = xi_max; }
    while(p_xi > tolerance && (xi_min != 0 ? currentXi > xi_min : 1) && xi_min <= 0){

        // Punkt auf pdensity ermitteln -> p(currentXi) = p_xi                              // Stelle xi
        p_xi = SymEngine::eval_double(*pdensity->subs({{xi, toExpression(currentXi)}}));    // Funktionswert

        // Check ob Maximum übertroffen wurde
        if(p_xi > maxP_Xi){
            maxP_Xi = p_xi;
            max_Xi = currentXi;
        }

        //
        currentXi -= segmentation;
    }
    // nach abbruch zuweisen
    if(xi_min == 0){
        leftBorder = currentXi + segmentation;
    }
    else{
        leftBorder = (currentXi > xi_min ? currentXi + segmentation : xi_min);
    }

    return {max_Xi, maxP_Xi, leftBorder, rightBorder};
}

void rejectionSampling(const Expression& pdensity, std::vector<float>& samples, unsigned int nSamples, const float& xi_min, const float& xi_max, const float& tolerance, const float& segmentation){

    // Check benötigt sodass pdensity nur von Symbol xi abhängig ist

    // 
    LOG << "-- Untersuche pdf(xi) = " << pdensity << ENDL;

    //
    auto [max_Xi, maxP_Xi, leftBorder, rightBorder] = preprocessPDF(pdensity, xi_min, xi_max, tolerance, segmentation);

    //
    LOG << "   Max at p(" << max_Xi << ") = " << maxP_Xi << " " << ENDL;
    LOG << "   Borders : (" << leftBorder << "|"<< rightBorder << ")" << ENDL;
    LOG << ENDL;

    if(maxP_Xi <= 0 || leftBorder == rightBorder){

        RETURNING_ASSERT(TRIGGER_ASSERT, "Invalide pdf Funktion übergeben",);
        return;
    }

    // rejection sampling
    // . zufallszahl xi_random zwischen linker und rechter grenze generieren -> stelle xi
    // . zufallszahl q zwischen 0 und pmax generieren
    // . wenn p(xi_random) < q -> sample wird aufgenommen
    // . ansonsten ablgelehnt ('rejection')
    // . damit lässt sich eine zufallsverteilung samplen durch die gleichmäßige verteilung der stellen x_random
    //   und das Hinzufügen dieser mit wahrscheinlichkeit p(xi_random)

    // allokierung
    samples.clear();
    samples.reserve(nSamples);

    //
    unsigned int generatedSamples = 0;
    seedFloatGenerator();

    float xi_random, p_xi, q_random;

    while(generatedSamples < nSamples){

        xi_random = randFloat(leftBorder, rightBorder);                                     // zufällige Stelle
        p_xi = SymEngine::eval_double(*pdensity->subs({{xi, toExpression(xi_random)}}));    // Funktionswert bzw Wahrscheinlichkeitsdichte an Stelle

        q_random = randFloat(0,maxP_Xi);                                                    // Zufallswert zwischen max pd und 0 

        if(q_random < p_xi){

            samples.emplace_back(xi_random);
            generatedSamples++;
        }
    }
}

std::array<float,2> processSamples(const std::vector<float>& samples){

    float mean = 0.0f, deviation = 0.0f;
    size_t nSamples = samples.size();

    for(const auto& sample : samples){ mean += sample;}
    mean /= nSamples;

    for(const auto& sample : samples){ deviation += std::pow(sample - mean,2);}
    deviation = std::sqrt(deviation/(nSamples-1));

    LOG << "-- Processed Samples : [Mean|" << mean << "], [Deviation|" << deviation << "]" << ENDL; 
    LOG << ENDL;

    return {mean, deviation};
}