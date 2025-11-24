#include "ControllerInput.h"

#include <iostream>

ControllerButton::ControllerButton(vr::EVRButtonId id, VRSystemManager& vr_manager, ControllerManager& controller_manager)
    : buttonID(id),
      vrManager(vr_manager),
      controllerManager(controller_manager),
      currentState(ButtonState::NOT_PRESSED),
      wasDownLastFrame(false) {}

ControllerButton::~ControllerButton() {}

void ControllerButton::update() {
    vr::IVRSystem* vrSystem = vrManager.getVRSystem();
    if (!vrManager.isInitialized()) {
        // If VR isn't running, reset to a safe state
        currentState = ButtonState::NOT_PRESSED;
        wasDownLastFrame = false;
        return;
    }

    vr::TrackedDeviceIndex_t controllerIndex = controllerManager.getControllerIndex();

    // Get the controller's current state
    vr::VRControllerState_t controllerState;
    if (!vrSystem->GetControllerState(controllerIndex, &controllerState, sizeof(controllerState))) {
        // Failed to get state, reset
        currentState = ButtonState::NOT_PRESSED;
        wasDownLastFrame = false;
        return;
    }

    bool isDownThisFrame = (controllerState.ulButtonPressed & vr::ButtonMaskFromId(buttonID)) > 0;

    if (isDownThisFrame && !wasDownLastFrame) {
        currentState = ButtonState::ON_PRESSED;
    } else if (isDownThisFrame && wasDownLastFrame) {
        currentState = ButtonState::PRESSING;
    } else if (!isDownThisFrame && wasDownLastFrame) {
        currentState = ButtonState::ON_RELEASE;
    } else {
        currentState = ButtonState::NOT_PRESSED;
    }

    wasDownLastFrame = isDownThisFrame;
}

bool ControllerButton::isButtonPressedThisFrame() const {
    return currentState == ButtonState::ON_PRESSED;
}

bool ControllerButton::isButtonReleasedThisFrame() const {
    return currentState == ButtonState::ON_RELEASE;
}

bool ControllerButton::isHeld() const {
    return currentState == ButtonState::ON_PRESSED || currentState == ButtonState::PRESSING;
}