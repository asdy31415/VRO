#include "Grabber.h"

float RotationCalculation::getLevelRotationCos(DirectX::XMVECTOR axis, DirectX::XMVECTOR screenTop) {

}

DirectX::XMMATRIX getRotationMatAlongAxis(DirectX::XMVECTOR axis, float rotationCos) {

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
    
    

}