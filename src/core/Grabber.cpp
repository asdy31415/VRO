#include "Grabber.h"
#include <algorithm>
#include <cmath>

float RotationCalculation::getLevelRotationCos(DirectX::XMVECTOR axis, DirectX::XMVECTOR screenTop) {
    DirectX::XMVECTOR levelVector = DirectX::XMVectorSet(-DirectX::XMVectorGetZ(axis), 0.0f, DirectX::XMVectorGetX(axis), 0.0f);
    if (DirectX::XMVectorGetZ(axis) < 0) {
        levelVector = DirectX::XMVectorScale(levelVector, -1);
    }
    levelVector = DirectX::XMVector3Normalize(levelVector);

    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(screenTop, levelVector));
}

DirectX::XMVECTOR RotationCalculation::getRotationQuatAlongAxis(DirectX::XMVECTOR axis, float rotationCos) {
    float clampedCos = std::clamp(rotationCos, -1.0f, 1.0f);

    float angle = acosf(clampedCos);

    return DirectX::XMQuaternionRotationAxis(axis, angle);
}

Grabber::Grabber(RayPointer& pointer, ControllerTransform& transform, ControllerButton& button)
    : rayPointer(pointer),
      controllerTransform(transform),
      controllerButton(button),
      currentOverlay(nullptr)
{}
Grabber::~Grabber() {
    currentOverlay = nullptr;
}

void Grabber::update(std::vector<Overlay*>& overlays) {
    if (controllerButton.isButtonPressedThisFrame()) {
        Overlay* target = rayPointer.PointingAt(overlays);
        if (target) {
            StartGrab(target);
        }
    }

    if (controllerButton.isHeld() && currentOverlay) {
        //TODO::fix dangling pointer
        DuringGrab();
    }

    if (controllerButton.isButtonReleasedThisFrame()) {
        EndGrab();
    }

}

void Grabber::StartGrab(Overlay* overlay) {
    currentOverlay = overlay;
    startControllerRot = controllerTransform.getRotation();

    DirectX::XMMATRIX overlayMat= currentOverlay->getTransformMatrix();

    DirectX::XMVECTOR overlayScale, overlayRotQuat, OverlayTrans;
    DirectX::XMMatrixDecompose(&overlayScale, &overlayRotQuat, &OverlayTrans, overlayMat);
    startOverlayRot = overlayRotQuat;

    DirectX::XMVECTOR controllerPos = controllerTransform.getTranslation();

    startOffset = DirectX::XMVectorSubtract(OverlayTrans, controllerPos);
}

void Grabber::DuringGrab() {
    DirectX::XMVECTOR currentControllerRot = controllerTransform.getRotation();
    DirectX::XMVECTOR currentControllerPos = controllerTransform.getTranslation();

    //Rotation
    DirectX::XMVECTOR invStartRot = DirectX::XMQuaternionInverse(startControllerRot);
    DirectX::XMVECTOR deltaRot = DirectX::XMQuaternionMultiply(currentControllerRot, invStartRot);

    DirectX::XMVECTOR newOverlayRot = DirectX::XMQuaternionMultiply(deltaRot, startOverlayRot);

    //Rotation correction
    if (currentOverlay->getRollingLock()) {
        DirectX::XMVECTOR zAxisLocal = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); 
        DirectX::XMVECTOR yAxisLocal = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        DirectX::XMVECTOR currentNormal = DirectX::XMVector3Rotate(zAxisLocal, newOverlayRot);
        DirectX::XMVECTOR currentTop    = DirectX::XMVector3Rotate(yAxisLocal, newOverlayRot);

        float rotCos = RotationCalculation::getLevelRotationCos(currentNormal, currentTop);

        DirectX::XMVECTOR correctionQuat = RotationCalculation::getRotationQuatAlongAxis(currentNormal, rotCos);

        newOverlayRot = DirectX::XMQuaternionMultiply(newOverlayRot, correctionQuat);
    }

    //Translation
    DirectX::XMVECTOR rotatedOffset = DirectX::XMVector3Rotate(startOffset, deltaRot);
    DirectX::XMVECTOR newOverlayPos = DirectX::XMVectorAdd(currentControllerPos, rotatedOffset);

    DirectX::XMMATRIX newTransform = DirectX::XMMatrixAffineTransformation(
        DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), // Scaling
        DirectX::XMVectorZero(),                      // Rotation Origin
        newOverlayRot,                       // Rotation Quaternion
        newOverlayPos                        // Translation
    );

    currentOverlay->setTransform(newTransform);
}

void Grabber::EndGrab() {
    currentOverlay = nullptr;
}