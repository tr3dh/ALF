#pragma once

#include "defines.h"

void calcOrbitCamera(Camera& camera, const Vector2& movDelta, const Vector3& rotationCenter, bool fixRotationCenterOnScreen, const Vector2& rotationSensitivity);

void calcNormalPlanarCamera(Camera& camera, const Vector3& movDelta, const Vector3& movSensitivity);

void calcFirstPersonCamera(Camera& camera, const Vector3& movDelta, const Vector3& rotDelta, const Vector3& movSensitivity, const Vector3& rotSensitivity, float dt);