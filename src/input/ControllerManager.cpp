#include "ControllerManager.h"

ControllerManager::ControllerManager(VRSystemManager& manager)
    : vrManager(manager),
      primaryControllerIndex(vr::k_unTrackedDeviceIndexInvalid),
      leftHandIndex(vr::k_unTrackedDeviceIndexInvalid),
      rightHandIndex(vr::k_unTrackedDeviceIndexInvalid) {
}

void ControllerManager::update() {
    vr::IVRSystem* vrSystem = vrManager.getVRSystem();
    if (!vrManager.isInitialized()) {
        primaryControllerIndex = vr::k_unTrackedDeviceIndexInvalid;
        leftHandIndex = vr::k_unTrackedDeviceIndexInvalid;
        rightHandIndex = vr::k_unTrackedDeviceIndexInvalid;
        return;
    }

    // Find both controllers
    rightHandIndex = vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
    leftHandIndex = vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);

    // Set the "primary" controller index
    if (rightHandIndex != vr::k_unTrackedDeviceIndexInvalid) {
        primaryControllerIndex = rightHandIndex;
    } else if (leftHandIndex != vr::k_unTrackedDeviceIndexInvalid) {
        primaryControllerIndex = leftHandIndex;
    } else {
        // No controllers found
        primaryControllerIndex = vr::k_unTrackedDeviceIndexInvalid;
    }
}