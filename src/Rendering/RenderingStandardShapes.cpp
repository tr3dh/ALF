#include "RenderingStandardShapes.h"

//
void DrawArrow3D(Vector3 start, Vector3 end, float shaftRadius, float headRadius, float headLength, Color color) {

    Vector3 dir = Vector3Subtract(end, start);
    float length = Vector3Length(dir);
    Vector3 normDir = Vector3Normalize(dir);

    Vector3 shaftEnd = Vector3Add(start, Vector3Scale(normDir, length - headLength));

    // Schaft : gleichmäßiger Zylinder
    DrawCylinderEx(start, shaftEnd, shaftRadius, shaftRadius, 8, color);

    // Spitze : Kegel (Zylinder mit topRadius 0)
    DrawCylinderEx(shaftEnd, end, headRadius, 0.0f, 8, color);
}