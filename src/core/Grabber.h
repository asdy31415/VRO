#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "VRSystemManager.h"
#include "ControllerTransform.h"
#include "ControllerInput.h"
#include "OverlayManager.h"
#include "RayPointer.h"

class RotationCalculation {
public:
    static float getLevelRotationCos(DirectX::XMVECTOR axis, DirectX::XMVECTOR screenTop);

    static DirectX::XMMATRIX getRotationMatAlongAxis(DirectX::XMVECTOR axis, float rotationCos);
};


class Grabber {
public:
    Grabber(RayPointer& pointer, ControllerTransform& transform, ControllerButton& button);
    ~Grabber();

    void update(std::vector<Overlay*>& overlays);

    bool isGrabbing() const;
    Overlay* getGrabbedOverlay() const;

private:
    void StartGrab(Overlay* overlay);

    void DuringGrab();

    void EndGrab();

    RayPointer& rayPointer;
    ControllerTransform controllerTransform;
    ControllerButton& controllerButton;

    Overlay* currentOverlay = nullptr;
    
    DirectX::XMVECTOR startControllerRot;
    DirectX::XMVECTOR startOverlayRot;
    DirectX::XMVECTOR startOffset;
};
