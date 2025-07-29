// # Makefile zum Setup einer modularen Engine-Toolchain in MSYS2/MinGW64

// .PHONY: all install-deps clone build clean

// PREFIX := $(CURDIR)/external

// all: install-deps clone build

// install-deps:
// 	pacman -Sy --noconfirm \
// 		git cmake ninja \
// 		mingw-w64-x86_64-gcc \
// 		mingw-w64-x86_64-make \
// 		mingw-w64-x86_64-glm \
// 		mingw-w64-x86_64-openal \
// 		mingw-w64-x86_64-pkg-config \
// 		mingw-w64-x86_64-physfs

// clone:
// 	mkdir -p $(PREFIX)

// 	# EnTT (Header-only ECS)
// 	git clone https://github.com/skypjack/entt.git $(PREFIX)/entt

// 	# Dear ImGui (Header + .cpp)
// 	git clone https://github.com/ocornut/imgui.git $(PREFIX)/imgui

// 	# bgfx
// 	git clone --recursive https://github.com/bkaradzic/bgfx.git $(PREFIX)/bgfx
// 	git clone --recursive https://github.com/bkaradzic/bx.git $(PREFIX)/bx
// 	git clone --recursive https://github.com/bkaradzic/bimg.git $(PREFIX)/bimg

// 	# PhysX
// 	git clone --recursive https://github.com/NVIDIA/PhysX.git $(PREFIX)/PhysX

// build:
// 	# Build bgfx with GNU toolchain
// 	cd $(PREFIX)/bgfx && \
// 	make -j$(nproc) toolchain=mingw GENDIR=../.build

// 	# Build PhysX with CMake + Ninja
// 	cd $(PREFIX)/PhysX/physx && \
// 	cmake -B build \
// 		-S . \
// 		-G Ninja \
// 		-DPHYSX_ENABLE_SHARED=OFF \
// 		-DTARGET_BUILD_PLATFORM=windows \
// 		-DCMAKE_BUILD_TYPE=Release \
// 		-DPX_GENERATE_STATIC_LIBRARIES=ON \
// 		-DPX_OUTPUT_LIB_DIR=bin \
// 		-DPX_OUTPUT_DLL_DIR=bin \
// 		-DPX_OUTPUT_EXE_DIR=bin && \
// 	cmake --build build --target PhysX_static

// clean:
// 	rm -rf $(PREFIX)




// #include "defines.h"
// #include "symbolic/symbolicExpressionParser.h"
// #include "femModel/Model.h"

// #include <vector>
// #include <string>
// #include <algorithm>
// #include <cctype>

// // Idee : Neuron bis polynomgrad n

// typedef float Weight;

// struct Neuron{

//     Neuron() = default;

//     Neuron(const size_t& setUpForNumWeights){
//         setup(setUpForNumWeights);
//     }

//     void setup(const size_t& setUpForNumWeights){

//         numWeights = setUpForNumWeights;
//         weights.resize(numWeights, 1);
//     }

//     float calculateOutput(const std::vector<float> inputs){

//         RETURNING_ASSERT(inputs.size() == numWeights, "Anzahl der Inputwerte stimmt nicht mit Anzahl der Gewichte im Neuron überein",0);

//         float output = bias;
//         for(size_t counter = 0; counter < numWeights; counter++){
//             output += weights[counter]*inputs[counter];
//         }
//         return output;
//     }

//     void backpropagate(const std::vector<float>& inputs, const float& error, const float& learningRate) {
        
//         RETURNING_ASSERT(inputs.size() == numWeights, "Anzahl der Inputwerte stimmt nicht mit Anzahl der Gewichte im Neuron überein",);

//         // Gewichte anpassen
//         for(size_t i = 0; i < numWeights; ++i) {

//             weights[i] += learningRate * error * inputs[i];
//         }

//         // bias anpassen
//         bias += learningRate * error;
//     }

//     void logWeigths(){

//         LOG << "Neuron " << numWeights << " inputs, bias " << bias << ", weigths [";
        
//         for(const auto& weight : weights){
//             LOG << weight << ",";
//         }
//         LOG << "]" << endl;
//     }
    
//     //
//     size_t numWeights = 0;

//     float bias = 0;
//     std::vector<Weight> weights = {};
// };

// //
// struct NeuronalNet{

//     //
//     NeuronalNet() = default;

//     //
//     void setInput(const std::vector<float>& inputSource){
        
//         inputs = inputSource;
//         inputSize = inputs.size();
//     }

//     //
//     void addLayer(const size_t& numNeurons){

//         //
//         const size_t numLayers = layers.size() + 1;
//         layers.resize(numLayers);

//         // Wenn letzter Layer vorhanden ist Anzahl Gewichte auf Anzahl der Layerneuronen setzen, ansonsten auf inputFormat zugreifen
//         const size_t numWeights = numLayers > 1 ? layers[numLayers - 2].size() : inputSize;
//         layers.back().resize(numNeurons, Neuron{numWeights});

//         //
//         outputs.resize(numLayers);
//         outputs.back().resize(numNeurons);

//         //
//         LOG << "Netz erweitert auf " << numLayers << " Layer" << endl;
//     }

//     void calculateOutput(const std::vector<float>& inputSource){

//         //
//         setInput(inputSource);

//         //
//         calculateOutput();
//     }

//     void calculateOutput(){

//         //
//         for(size_t layer = 0; layer < layers.size(); layer++){

//             //
//             for(size_t neuron = 0; neuron < layers[layer].size(); neuron++){

//                 //
//                 const auto& input = layer > 0 ? outputs[layer-1] : inputs;

//                 //
//                 outputs[layer][neuron] = layers[layer][neuron].calculateOutput(input);
//             }
//         }
//     }

//     void backPropagate(const std::vector<float> targets, const float& learningRate, const float& scaleLayerWiseTo = 6.0f){

//         //
//         RETURNING_ASSERT(outputs.back().size() == targets.size(), "Output und Target Größe passen nicht zusammen",);

//         // nur in erstem frame
//         errors.resize(outputs.size());
//         for(size_t counter = 0; counter < outputs.size(); counter++){

//             errors[counter].resize(outputs[counter].size(),0);
//         }

//         for(size_t counter = 0; counter < targets.size(); counter++){

//             errors.back()[counter] = targets[counter] - outputs.back()[counter];
//         }

//         //
//         for(size_t layer = layers.size(); layer-- > 0;){

//             //
//             for(size_t neuron = layers[layer].size(); neuron-- > 0;){

//                 //
//                 for(size_t weigth = layers[layer][neuron].weights.size(); weigth-- > 0;){

//                     //
//                     if(layer < 1){
//                         continue;
//                     }

//                     //
//                     errors[layer -1][weigth] += layers[layer][neuron].weights[weigth] * errors[layer][neuron];
//                 }
//             }
//         }

//         //
//         for (size_t layer = layers.size(); layer-- > 0;) {

//             float maxAbsError = 0.0f;

//             //
//             for (const float& err : errors[layer]) {
//                 maxAbsError = std::max(maxAbsError, std::abs(err));
//             }

//             //
//             float scale = 1.0f;
//             if (maxAbsError > scaleLayerWiseTo) {
//                 scale = scaleLayerWiseTo / maxAbsError;
//             }

//             // 3. Backpropagation mit ggf. skalierten Fehlern
//             for (size_t neuron = layers[layer].size(); neuron-- > 0;) {

//                 const auto& input = (layer > 0) ? outputs[layer - 1] : inputs;

//                 float scaledError = errors[layer][neuron] * scale;
//                 layers[layer][neuron].backpropagate(input, scaledError, learningRate);
//             }
//         }
//     }

//     void logOutput(){

//         //
//         LOG << "Output : [";

//         //
//         for(const auto& output : outputs.back()){
//             LOG << output << ", ";
//         }
//         LOG << "]" << endl;
//     }

//     //
//     std::vector<std::vector<Neuron>> layers = {};

//     //
//     size_t inputSize = 0;
//     std::vector<float> inputs = {};
//     std::vector<std::vector<float>> outputs = {};

//     //
//     std::vector<std::vector<float>> errors = {};
// };

// int main(){

//     //
//     Neuron neuron(1);

//     //
//     auto polygon = [](float x) { return 2+3*x; };
//     const float learningRate = 0.005;

//     //
//     const size_t numInterations = 100;
//     for(size_t iteration = 0; iteration < numInterations; iteration++){

//         //
//         const float x = randFloat(2,10);

//         neuron.backpropagate({x}, polygon(x) - neuron.calculateOutput({x}), learningRate);

//         if(iteration >= numInterations - 1){

//             //
//             LOG << "Ziel : " << polygon(x) << endl;
//             LOG << "Ist : " << neuron.calculateOutput({x}) << endl;
//         }
//     }

//     NeuronalNet net;

//     net.setInput({1,2,3});
//     std::vector<float> target = {2,4,10,74};

//     // input layer
//     net.addLayer(2);

//     // denselayer
//     net.addLayer(5);
//     net.addLayer(5);
//     net.addLayer(5);
//     net.addLayer(5);
//     net.addLayer(5);

//     // Outputlayer
//     net.addLayer(target.size());

//     const float tolerance = 1e-3f; // z. B. 0.01

//     //
//     for(size_t iteration = 0; iteration < 10000; iteration++){

//         bool allWithinTolerance = true;

//         for (size_t i = 0; i < target.size(); ++i) {
//             if (std::abs(net.outputs.back()[i] - target[i]) >= tolerance) {
//                 allWithinTolerance = false;
//                 break;
//             }
//         }

//         if (allWithinTolerance) {
//             LOG << "break nach " << iteration << " Iterationen" << endl;
//             break;
//         }

//         net.calculateOutput();
//         net.backPropagate(target, 0.00000001, 40);
//     }

//     LOG << "Ziel ";
//     for(const auto& t : target){
//         LOG << t << ", ";
//     }
//     LOG << endl;

//     net.calculateOutput();
//     net.logOutput();

//     return 0;
// }

#include "defines.h"
#include "symbolic/symbolicExpressionParser.h"
#include "femModel/Model.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>

typedef float Weight;

// Aktivierungsfunktionen
struct ActivationFunction {
    // Leaky ReLU statt normaler ReLU (verhindert "dying ReLU")
    static float leakyRelu(float x) { return x > 0 ? x : 0.01f * x; }
    static float leakyReluDerivative(float x) { return x > 0 ? 1.0f : 0.01f; }
    
    static float relu(float x) { return std::max(0.0f, x); }
    static float reluDerivative(float x) { return x > 0 ? 1.0f : 0.0f; }
    
    static float sigmoid(float x) { 
        // Gradient Clipping für numerische Stabilität
        x = std::max(-50.0f, std::min(50.0f, x));
        return 1.0f / (1.0f + std::exp(-x)); 
    }
    static float sigmoidDerivative(float x) { 
        float s = sigmoid(x);
        return s * (1.0f - s);
    }
    
    static float tanh(float x) { return std::tanh(x); }
    static float tanhDerivative(float x) { 
        float t = tanh(x);
        return 1.0f - t * t;
    }
};

struct Neuron {
    Neuron() = default;
    
    Neuron(const size_t& setUpForNumWeights) {
        setup(setUpForNumWeights);
    }
    
    // Verbesserte Gewichtsinitialisierung
    void setup(const size_t& setUpForNumWeights) {
        numWeights = setUpForNumWeights;
        weights.resize(numWeights);
        
        // Xavier/Glorot Initialisierung (besser für verschiedene Aktivierungsfunktionen)
        float stddev = std::sqrt(2.0f / (numWeights + 1)); // +1 für robustere Initialisierung
        for(auto& weight : weights) {
            weight = (randFloat(0, 1) * 2 - 1) * stddev; // Ohne zusätzliche Verkleinerung
        }
        
        // Bias zu Null initialisieren
        bias = 0.0f; // Neutrale Initialisierung
    }
    
    float calculateOutput(const std::vector<float>& inputs) {
        RETURNING_ASSERT(inputs.size() == numWeights, "Anzahl der Inputwerte stimmt nicht mit Anzahl der Gewichte im Neuron überein", 0);
        
        float output = bias;
        for(size_t counter = 0; counter < numWeights; counter++) {
            output += weights[counter] * inputs[counter];
        }
        
        // Aktivierungsfunktion anwenden
        return ActivationFunction::leakyRelu(output);
    }
    
    float calculateRawOutput(const std::vector<float>& inputs) {
        RETURNING_ASSERT(inputs.size() == numWeights, "Anzahl der Inputwerte stimmt nicht mit Anzahl der Gewichte im Neuron überein", 0);
        
        float output = bias;
        for(size_t counter = 0; counter < numWeights; counter++) {
            output += weights[counter] * inputs[counter];
        }
        return output;
    }
    
    void backpropagate(const std::vector<float>& inputs, const float& error, const float& learningRate, const float& rawOutput) {
        RETURNING_ASSERT(inputs.size() == numWeights, "Anzahl der Inputwerte stimmt nicht mit Anzahl der Gewichte im Neuron überein",);
        
        // Sehr aggressives Gradient Clipping für Stabilität
        float clippedError = std::max(-1.0f, std::min(1.0f, error));
        
        // Gradient der Aktivierungsfunktion
        float activationGradient = ActivationFunction::leakyReluDerivative(rawOutput);
        float totalError = clippedError * activationGradient;
        
        // Gewichte anpassen mit sehr starkem Clipping
        for(size_t i = 0; i < numWeights; ++i) {
            float weightUpdate = learningRate * totalError * inputs[i];
            weightUpdate = std::max(-0.1f, std::min(0.1f, weightUpdate)); // Sehr kleine Updates
            weights[i] += weightUpdate;
            
            // Gewichte stark begrenzen
            weights[i] = std::max(-10.0f, std::min(10.0f, weights[i]));
        }
        
        // Bias anpassen
        float biasUpdate = learningRate * totalError;
        biasUpdate = std::max(-0.1f, std::min(0.1f, biasUpdate));
        bias += biasUpdate;
        bias = std::max(-10.0f, std::min(10.0f, bias));
    }
    
    void logWeights() {
        LOG << "Neuron " << numWeights << " inputs, bias " << bias << ", weights [";
        for(const auto& weight : weights) {
            LOG << weight << ",";
        }
        LOG << "]" << endl;
    }
    
    size_t numWeights = 0;
    float bias = 0;
    std::vector<Weight> weights = {};
};

struct NeuronalNet {
    NeuronalNet() = default;
    
    void setInput(const std::vector<float>& inputSource) {
        inputs = inputSource;
        inputSize = inputs.size();
    }
    
    void addLayer(const size_t& numNeurons) {
        const size_t numLayers = layers.size() + 1;
        layers.resize(numLayers);
        
        // Anzahl der Gewichte bestimmen
        const size_t numWeights = numLayers > 1 ? layers[numLayers - 2].size() : inputSize;
        layers.back().resize(numNeurons, Neuron{numWeights});
        
        // Outputs und Raw Outputs erweitern
        outputs.resize(numLayers);
        outputs.back().resize(numNeurons);
        rawOutputs.resize(numLayers);
        rawOutputs.back().resize(numNeurons);
        
        LOG << "Netz erweitert auf " << numLayers << " Layer" << endl;
    }
    
    void calculateOutput(const std::vector<float>& inputSource) {
        setInput(inputSource);
        calculateOutput();
    }
    
    void calculateOutput() {
        for(size_t layer = 0; layer < layers.size(); layer++) {
            for(size_t neuron = 0; neuron < layers[layer].size(); neuron++) {
                const auto& input = layer > 0 ? outputs[layer-1] : inputs;
                
                // Raw Output (vor Aktivierung) speichern
                rawOutputs[layer][neuron] = layers[layer][neuron].calculateRawOutput(input);
                
                // Aktivierte Ausgabe
                if(layer == layers.size() - 1) {
                    // Letzter Layer: keine Aktivierung (für Regression)
                    outputs[layer][neuron] = rawOutputs[layer][neuron];
                } else {
                    outputs[layer][neuron] = layers[layer][neuron].calculateOutput(input);
                }
            }
        }
    }
    
    // Korrigierte Backpropagation-Methode
    void backPropagate(const std::vector<float>& targets, const float& learningRate) {
        RETURNING_ASSERT(outputs.back().size() == targets.size(), "Output und Target Größe passen nicht zusammen",);
        
        // Fehler-Vektoren initialisieren
        errors.resize(outputs.size());
        for(size_t counter = 0; counter < outputs.size(); counter++) {
            errors[counter].resize(outputs[counter].size(), 0);
        }
        
        // Ausgabefehler berechnen (weniger aggressives Clipping)
        for(size_t counter = 0; counter < targets.size(); counter++) {
            float error = targets[counter] - outputs.back()[counter];
            errors.back()[counter] = std::max(-10.0f, std::min(10.0f, error)); // Weniger aggressives Clipping
        }
        
        // Rückwärts durch die Layer
        for(size_t layer = layers.size(); layer-- > 0;) {
            
            // Fehler für vorherige Layer berechnen (außer beim ersten Layer)
            if(layer > 0) {
                // Fehler-Array für vorherige Schicht zurücksetzen
                std::fill(errors[layer-1].begin(), errors[layer-1].end(), 0.0f);
                
                for(size_t neuron = 0; neuron < layers[layer].size(); neuron++) {
                    // Aktivierungsgradient für AKTUELLEN Layer
                    float activationGradient;
                    if(layer == layers.size() - 1) {
                        // Output Layer: keine Aktivierung
                        activationGradient = 1.0f;
                    } else {
                        activationGradient = ActivationFunction::leakyReluDerivative(rawOutputs[layer][neuron]);
                    }
                    
                    float currentError = errors[layer][neuron] * activationGradient;
                    
                    for(size_t weight = 0; weight < layers[layer][neuron].weights.size(); weight++) {
                        // Fehler an vorherige Schicht weiterleiten
                        float propagatedError = layers[layer][neuron].weights[weight] * currentError;
                        errors[layer-1][weight] += propagatedError;
                    }
                }
            }
            
            // Gewichte und Bias aktualisieren
            for(size_t neuron = 0; neuron < layers[layer].size(); neuron++) {
                const auto& input = (layer > 0) ? outputs[layer-1] : inputs;
                
                // Aktivierungsgradient für aktuellen Layer
                float activationGradient;
                if(layer == layers.size() - 1) {
                    activationGradient = 1.0f; // Output Layer
                } else {
                    activationGradient = ActivationFunction::leakyReluDerivative(rawOutputs[layer][neuron]);
                }
                
                float totalError = errors[layer][neuron] * activationGradient;
                
                // Gewichte anpassen (weniger aggressives Clipping)
                for(size_t i = 0; i < layers[layer][neuron].weights.size(); ++i) {
                    float weightUpdate = learningRate * totalError * input[i];
                    weightUpdate = std::max(-1.0f, std::min(1.0f, weightUpdate)); // Weniger restriktiv
                    layers[layer][neuron].weights[i] += weightUpdate;
                    
                    // Gewichte weniger stark begrenzen
                    layers[layer][neuron].weights[i] = std::max(-50.0f, std::min(50.0f, layers[layer][neuron].weights[i]));
                }
                
                // Bias anpassen
                float biasUpdate = learningRate * totalError;
                biasUpdate = std::max(-1.0f, std::min(1.0f, biasUpdate));
                layers[layer][neuron].bias += biasUpdate;
                layers[layer][neuron].bias = std::max(-50.0f, std::min(50.0f, layers[layer][neuron].bias));
            }
        }
    }
    
    void logOutput() {
        LOG << "Output : [";
        for(const auto& output : outputs.back()) {
            LOG << output << ", ";
        }
        LOG << "]" << endl;
    }
    
    std::vector<std::vector<Neuron>> layers = {};
    
    size_t inputSize = 0;
    std::vector<float> inputs = {};
    std::vector<std::vector<float>> outputs = {};
    std::vector<std::vector<float>> rawOutputs = {};
    
    std::vector<std::vector<float>> errors = {};
};

int netmain() {

    Neuron neuron(1);
    
    auto polygon = [](float x) { return 2 + 3 * x; };
    const float learningRate = 0.7;
    
    const size_t numIterations = 1000;
    for(size_t iteration = 0; iteration < numIterations; iteration++) {
        const float x = randFloat(2, 10);
        float rawOut = neuron.calculateRawOutput({x});

        float error = polygon(x) - neuron.calculateOutput({x});

        if(iteration >= numIterations - 1 || error < 1e-4) {
            LOG << "Ziel : " << polygon(x) << endl;
            LOG << "Ist : " << neuron.calculateOutput({x}) << endl;
            break;
        } else {
            neuron.backpropagate({x}, error, learningRate, rawOut);
        }
    }
    
    // Netzwerk-Training
    NeuronalNet net;
    net.setInput({1, 2, 3});
    auto func = [](float x, float y, float z) { return 4 + x*x - z * z + y*y*z; };
    std::vector<float> target = {func(1,2,3)};

    net.addLayer(6);
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(6);   
    net.addLayer(target.size()); // Output Layer
    
    const float tolerance = 1e-3f;
    const float netLearningRate = 0.01f; // Höhere Lernrate
    
    float error = 5;
    for(size_t iteration = 0; iteration < 10000; iteration++) {

        std::vector<float> input = {randFloat(2,10), randFloat(2,10), randFloat(2,10)};
        net.setInput(input);
        target[0] = func(input[0], input[1], input[2]);

        net.calculateOutput();
        
        // Konvergenz prüfen
        bool allWithinTolerance = true;
        float totalError = 0.0f;
        for(size_t i = 0; i < target.size(); ++i) {
            error = std::abs(net.outputs.back()[i] - target[i]);
            totalError += error;
            if(error >= tolerance) {
                allWithinTolerance = false;
            }
        }
        
        if(allWithinTolerance) {
            LOG << "Konvergiert nach " << iteration << " Iterationen" << endl;
            break;
        }
        
        net.backPropagate(target, netLearningRate);
        
        // Fortschritt ausgeben
        if(iteration % 500 == 0) {
            LOG << "Iteration " << iteration << " (Fehler: " << totalError << "): ";
            net.logOutput();
        }
    }
    
    // Finale Ausgabe
    LOG << "Ziel: ";
    for(const auto& t : target) {
        LOG << t << ", ";
    }
    LOG << endl;
    
    net.calculateOutput();
    net.logOutput();
    
    return 0;
}

// installs
// git clone --recursive https://github.com/bkaradzic/bgfx.cmake
// cd bgfx.cmake
// make PROJECTS="shaderc example-28-compute" CONFIG=release
// cd /path/to/your/extern/bgfx.cmake
// mkdir build && cd build
// cd /path/to/your/extern/bgfx.cmake
// mkdir build && cd build

// Aufruf
// ./.build/win64_mingw_gcc/bin/shaderc.exe \
//   -f ../../shaders/matrix_multiply.compute.slang \
//   -o ../../shaders/matrix_multiply.bin \
//   --type compute --platform windows --profile spirv \
//   --bin2c matrix_multiply

// ./shaderc.exe --version

#include "symbolic/symbolicExpressionParser.h"

void isolateExpr(std::string& expr){

    if (expr.at(0) == '(') {
        
        int count = 0;
        size_t i = 0;
        for (; i < expr.size(); ++i) {
            if (expr[i] == '(') count++;
            else if (expr[i] == ')') count--;
            // Wenn count bei 0 ist Klammer die schließende Klammer zur ersten öffnenden
            if (count == 0) break;
        }

        if (i == expr.size() - 1) {
            expr = expr.substr(1, expr.size() - 2);
        }
    }
}

bool contains(std::string expr, const std::string symbol){

    ASSERT(!expr.contains(' '), "an Lexer übergebene Expression enthält Leerzeichen");

    if(expr == symbol){
        return true;
    }

    if(!string::contains(expr, symbol)){
        return false;
    }

    isolateExpr(expr);

    const std::string dominantOperator = getDominantOperator(expr);
    std::vector<std::string> args = lexExpression(expr, dominantOperator);

    if(dominantOperator == ""){
        return false;
    }

    for(const auto& arg : args){
        if(contains(arg, symbol)){
            return true;
        }
    }

    return false;
}

std::string diff(std::string expr, const std::string symbol){

    std::string result = "";
    ASSERT(!expr.contains(' '), "an Lexer übergebene Expression enthält Leerzeichen");

    if(!contains(expr, symbol)){
        return "0";
    }

    isolateExpr(expr);

    if(expr == symbol){
        return "1";
    }

    const std::string dominantOperator = getDominantOperator(expr);
    std::vector<std::string> args = lexExpression(expr, dominantOperator);

    if(dominantOperator == "+"){

        // init
        result = "(" + diff(args[0], symbol) + ")";

        //
        for(size_t argIdx = 1; argIdx < args.size(); argIdx++){
            result += "+(" + diff(args[argIdx], symbol) + ")";
        }

        return result;
    }
    else if(dominantOperator == "*"){

        //
        for(size_t argIdxDeriv = 0; argIdxDeriv < args.size(); argIdxDeriv++){

            //
            result += argIdxDeriv == 0 ? "" : "+";
            result += "(" + diff(args[argIdxDeriv], symbol);

            //
            for(size_t argIdx = 0; argIdx < args.size(); argIdx++){
                
                //
                if(argIdx == argIdxDeriv){
                    continue;
                }

                result += "*(" + args[argIdx] + ")";
            }

            result += ")";
        }
    }
    else if(dominantOperator == "^"){

        std::string& exponent = args[1];
        std::string& base = args[0];
        
        isolateExpr(exponent);
        isolateExpr(base);

        bool exponentContainsSymbol = contains(exponent, symbol);
        bool baseContainsSymbol = contains(base, symbol);

        // konstanter Exponent
        if(baseContainsSymbol && !exponentContainsSymbol){
            return "(" + exponent + ")*(" + base + ")^(" + exponent + "+-1)*(" + diff(base,symbol) + ")"; 
        }
        else if (!baseContainsSymbol && exponentContainsSymbol) {
            return "(" + base + ")^(" + exponent + ")*ln(" + base + ")*(" + diff(exponent, symbol) + ")";
        }
        else {
            std::string baseDiff = diff(base, symbol);
            std::string exponentDiff = diff(exponent, symbol);

            return "(" + base + ")^(" + exponent + ")*(" + exponentDiff + "*ln(" + base + ") + "
                + exponent + "*(" + baseDiff + ")/(" + base + "))";
        }
    }
    // Ableitungen für symbole einfügen

    return result;
}

int main(){

    LOG << diff("e^(2*x)","x") << endl;
    return 0;
}