#include "VRSystemManager.h"

#include <iostream>

VRSystemManager::VRSystemManager() {
    vr::EVRInitError eError = vr::VRInitError_None;

    // Initialize OpenVR as an overlay application
    vrSystem = vr::VR_Init(&eError, vr::VRApplication_Overlay);

    if (eError != vr::VRInitError_None) {
        vrSystem = nullptr;
        std::cerr << "OpenVR VR_Init failed: " << vr::VR_GetVRInitErrorAsSymbol(eError) << std::endl;
        return;
    }

    if (!vr::VROverlay()) {
        std::cerr << "OpenVR VROverlay() failed." << std::endl;
        vr::VR_Shutdown();
        vrSystem = nullptr;
        return;
    }
}

VRSystemManager::~VRSystemManager() {
    if (vrSystem) {
        vr::VR_Shutdown();
    }
    vrSystem = nullptr;
}