#pragma once

#include "defines.h"
#include "Mesh/Material.h"

void seedFloatGenerator();

// Gibt Zufalls Float im Intervall [a, b] zurück
// rand gibt zahl zwischen 0 und RAND_MAX zurück
// wird über die Division mit dem Limit auf Wert zwischen null und 1 normalisiert und dann über (b - a) auf
// zahlenbereich gestreckt und mit minimum a geoffsettet
float randFloat(float a, float b);

std::array<float, 4> preprocessPDF(const Expression& pdensity, const float& tolerance, const float& segmentation);

void rejectionSampling(const Expression& pdensity, std::vector<float>& samples, unsigned int nSamples = 1000, const float& tolerance = 0.01, const float& segmentation = 0.01);

void processSamples(const std::vector<float>& samples);