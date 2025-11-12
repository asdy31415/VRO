#include "ControllerTransform.h"

static DirectX::XMMATRIX ConvertHmdMatrixToXMMATRIX(const vr::HmdMatrix34_t& mat) {
    return DirectX::XMMATRIX(
        mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
        mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
        mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
        0.0f,        0.0f,        0.0f,        1.0f        
    );
}

ControllerTransform::ControllerTransform(VRSystemManager& vr_manager, ControllerManager& controller_Manager)
    : vrManager(vr_manager),
      controllerManager(controller_Manager),
      transform(DirectX::XMMatrixIdentity()) {

    for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
        trackedDevicePoses[i] = {};
    }
}

ControllerTransform::~ControllerTransform() {}

void ControllerTransform::update() {
    vr::IVRSystem* vrSystem = vrManager.getVRSystem();
    if (vrManager.isInitialized()) {
        transform = DirectX::XMMatrixIdentity();
        return;
    }

    vrSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, trackedDevicePoses, vr::k_unMaxTrackedDeviceCount);
    vr::TrackedDeviceIndex_t controllerIndex = controllerManager.getControllerIndex();

    if (controllerIndex != vr::k_unTrackedDeviceIndexInvalid &&
        trackedDevicePoses[controllerIndex].bPoseIsValid) {

        transform = ConvertHmdMatrixToXMMATRIX(trackedDevicePoses[controllerIndex].mDeviceToAbsoluteTracking);
    }
    else {
        transform = DirectX::XMMatrixIdentity();
    }
}

DirectX::XMVECTOR ControllerTransform::getRotation() const {
    return DirectX::XMQuaternionRotationMatrix(transform);
}

DirectX::XMVECTOR ControllerTransform::getTranslation() const {
    return transform.r[3];
}