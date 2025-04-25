#pragma once

#include "defines.h"

inline SYMBOL(xi);

std::array<float, 4> preprocessPDF(const Expression& pdensity, const float& tolerance, const float& segmentation);

void rejectionSampling(const Expression& pdensity, const float& tolerance, const float& segmentation);