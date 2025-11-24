#include <openvr.h>

#include"VRSystemManager.h"

class ControllerManager {
public:
    ControllerManager(VRSystemManager& manager);

    void update();

    vr::TrackedDeviceIndex_t getControllerIndex() const { return primaryControllerIndex; }
    vr::TrackedDeviceIndex_t getLeftHandIndex() const { return leftHandIndex; }
    vr::TrackedDeviceIndex_t getRightHandIndex() const { return rightHandIndex; }

private:
    VRSystemManager& vrManager;

    vr::TrackedDeviceIndex_t primaryControllerIndex;
    vr::TrackedDeviceIndex_t leftHandIndex;
    vr::TrackedDeviceIndex_t rightHandIndex;
};