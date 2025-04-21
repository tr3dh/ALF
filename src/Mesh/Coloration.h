#pragma once

#include "defines.h"

sf::Color getColorByValue(float val, float min, float max) {

    // Normalisiere den Wert auf [0, 1]
    float normalized = (val - min) / (max - min);
    normalized = std::clamp(normalized, 0.0f, 1.0f);

    // Regenbogenfarben HSB/HSV basiert
    float hue = (1.0f - normalized) * 0.666f; // Blau (0.666) → Rot (0.0)
    sf::Color color = sf::Color::Black;

    if (hue < 0.166f) {
        // Rot zu Gelb (R=255, G steigt)
        color.r = 255;
        color.g = static_cast<sf::Uint8>(hue * 6 * 255);
    } else if (hue < 0.333f) {
        // Gelb zu Grün (G=255, R fällt)
        color.g = 255;
        color.r = static_cast<sf::Uint8>((0.333f - hue) * 6 * 255);
    } else if (hue < 0.5f) {
        // Grün zu Türkis (G=255, B steigt)
        color.g = 255;
        color.b = static_cast<sf::Uint8>((hue - 0.333f) * 6 * 255);
    } else if (hue < 0.666f) {
        // Türkis zu Blau (B=255, G fällt)
        color.b = 255;
        color.g = static_cast<sf::Uint8>((0.666f - hue) * 6 * 255);
    } else {
        // Blau (Vollton)
        color.b = 255;
    }

    return color;
}