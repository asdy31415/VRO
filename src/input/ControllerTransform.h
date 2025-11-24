#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "ControllerManager.h"
#include "VRSystemManager.h"


class ControllerTransform{
public:
    ControllerTransform(VRSystemManager& vr_manager, ControllerManager& controller_manager);
    ~ControllerTransform();

    void update();

    DirectX::XMMATRIX getTransform() { return transform; }

    DirectX::XMVECTOR ControllerTransform::getRotation() const;
    DirectX::XMVECTOR getTranslation() const;

private:
    VRSystemManager& vrManager;
    ControllerManager& controllerManager;

    DirectX::XMMATRIX transform;

    vr::TrackedDevicePose_t trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];
};

