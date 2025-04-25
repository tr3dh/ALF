#include "RejectionSampling.h"

std::array<float, 4> preprocessPDF(const Expression& pdensity, const float& tolerance, const float& segmentation){

    //
    float leftBorder = 0, rightBorder = 0;
    float maxP_Xi = 0, max_Xi = 0;

    float currentXi = 0;
    float p_xi = 2 * tolerance;

    currentXi = 0;
    while(p_xi > tolerance){

        // Punkt auf pdensity ermitteln                                                  // Stelle xi
        p_xi = SymEngine::eval_double(*pdensity->subs({{xi, toExpression(currentXi)}}));   // Funktionswert

        // Check ob Maximum übertroffen wurde
        if(p_xi > maxP_Xi){
            maxP_Xi = p_xi;
            max_Xi = currentXi;
        }

        // check nach Abbruchbedingung
        if(p_xi <= tolerance){ rightBorder = currentXi; }

        //
        currentXi += segmentation;
    }

    currentXi = 0;
    p_xi = 2 * tolerance;
    while(p_xi > tolerance){

        // Punkt auf pdensity ermitteln -> p(currentXi) = p_xi                                                   // Stelle xi
        p_xi = SymEngine::eval_double(*pdensity->subs({{xi, toExpression(currentXi)}}));   // Funktionswert

        // Check ob Maximum übertroffen wurde
        if(p_xi > maxP_Xi){
            maxP_Xi = p_xi;
            max_Xi = currentXi;
        }

        // check nach Abbruchbedingung
        if(p_xi <= tolerance){ leftBorder = currentXi; }

        //
        currentXi -= segmentation;
    }

    return {max_Xi, maxP_Xi, leftBorder, rightBorder};
}

void rejectionSampling(const Expression& pdensity, const float& tolerance, const float& segmentation){

    // Check benötigt sodass pdensity nur von Symbol xi abhängig ist

    // 
    LOG << "-- Untersuche pdf(xi) = " << pdensity << endl;

    //
    auto [max_Xi, maxP_Xi, leftBorder, rightBorder] = preprocessPDF(pdensity, tolerance, segmentation);

    //
    LOG << "   Max at p(" << max_Xi << ") = " << maxP_Xi << " " << endl;
    LOG << "   Borders : (" << leftBorder << "|"<< rightBorder << ")" << endl;
}