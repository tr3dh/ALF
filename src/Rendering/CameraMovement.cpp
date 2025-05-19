#include "CameraMovement.h"

void calcOrbitCamera(Camera& camera, const Vector2& movDelta, const Vector3& rotationCenter, bool fixRotationCenterOnScreen, const Vector2& rotationSensitivity){

    static Vector3 positionOffset, targetOffset;
    static Vector3 offsetCamera;
    static float radius, azimuth, elevation;
    static float yaw, pitch;
    static float minElevation = -PI/2 + 0.01f, maxElevation =  PI/2 - 0.01f;

    //
    positionOffset = Vector3Subtract(camera.position, rotationCenter);
    targetOffset = Vector3Subtract(camera.target, rotationCenter);

    radius = Vector3Length(Vector3Subtract(camera.position, rotationCenter));
    azimuth = atan2(positionOffset.x, positionOffset.z);
    elevation = asin(positionOffset.y / radius);

    yaw = -movDelta.x * rotationSensitivity.x;
    pitch = movDelta.y * rotationSensitivity.y;

    //
    azimuth += yaw;
    elevation += pitch;

    if (elevation < minElevation) elevation = minElevation;
    if (elevation > maxElevation) elevation = maxElevation;

    //
    offsetCamera = {
        radius * cosf(elevation) * sinf(azimuth),
        radius * sinf(elevation),
        radius * cosf(elevation) * cosf(azimuth)
    };
    camera.position = Vector3Add(rotationCenter, offsetCamera);

    if(!fixRotationCenterOnScreen || camera.target == rotationCenter){
        return;
    }

    static float targetRadius, targetAzimuth, targetElevation;

    targetRadius = Vector3Length(targetOffset);
    targetAzimuth = atan2(targetOffset.x, targetOffset.z);
    targetElevation = asin(targetOffset.y / targetRadius);

    targetAzimuth += yaw;
    targetElevation += pitch;

    if (targetElevation < minElevation) targetElevation = minElevation;
    if (targetElevation > maxElevation) targetElevation = maxElevation;

    Vector3 rotatedTargetOffset = {
        targetRadius * cosf(targetElevation) * sinf(targetAzimuth),
        0,              // targetRadius * sinf(targetElevation),
        targetRadius * cosf(targetElevation) * cosf(targetAzimuth)
    };

    camera.target = Vector3Add(rotationCenter, rotatedTargetOffset);
}

void calcNormalPlanarCamera(Camera& camera, const Vector3& movDelta, const Vector3& movSensitivity){

    static Vector3 normal, planarHorizontal, planarVertical;
    static Vector3 movCamera;

    normal = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    planarHorizontal = Vector3Normalize(Vector3CrossProduct(normal, camera.up));
    planarVertical = Vector3Normalize(Vector3CrossProduct(planarHorizontal, normal));

    movCamera = Vector3Scale(planarHorizontal, -movDelta.x * movSensitivity.x);
    movCamera = Vector3Add(movCamera, Vector3Scale(planarVertical, movDelta.y * movSensitivity.y));

    movCamera = Vector3Add(movCamera, Vector3Scale(normal, movDelta.z * movSensitivity.z));

    camera.position = Vector3Add(camera.position, movCamera);
    camera.target   = Vector3Add(camera.target, movCamera);
}

void calcFirstPersonCamera(Camera& camera, const Vector3& movDelta, const Vector3& rotDelta, const Vector3& movSensitivity, const Vector3& rotSensitivity, float dt){

    UpdateCameraPro(&camera,
        (Vector3){
            movDelta.x * movSensitivity.x * dt,
            movDelta.y * movSensitivity.y * dt,
            movDelta.z * movSensitivity.z * dt,
        },
        (Vector3){
            rotDelta.x * rotSensitivity.x,
            rotDelta.y * rotSensitivity.y,
            0.0f
        },
        rotDelta.z * rotSensitivity.z * dt
    );
}

        