#include "__Coloration.h"

Color getColorByValue(float val, float min, float max) {

    // Normalisierung des Werts auf Intervall (0, 1)
    float normalized = (val - min) / (max - min);
    normalized = std::clamp(normalized, 0.0f, 1.0f);

    // Regenbogenfarben (HSB/HSV)
    float hue = (1.0f - normalized) * 0.666f; // Blau (0.666) → Rot (0.0)
    Color color = BLACK;

    if (hue < 0.166f) {
        // Rot zu Gelb (R=255, G steigt)
        color.r = 255;
        color.g = static_cast<uint8_t>(hue * 6 * 255);
    } else if (hue < 0.333f) {
        // Gelb zu Grün (G=255, R fällt)
        color.g = 255;
        color.r = static_cast<uint8_t>((0.333f - hue) * 6 * 255);
    } else if (hue < 0.5f) {
        // Grün zu Türkis (G=255, B steigt)
        color.g = 255;
        color.b = static_cast<uint8_t>((hue - 0.333f) * 6 * 255);
    } else if (hue < 0.666f) {
        // Türkis zu Blau (B=255, G fällt)
        color.b = 255;
        color.g = static_cast<uint8_t>((0.666f - hue) * 6 * 255);
    } else {
        // Blau
        color.b = 255;
    }

    return color;
}