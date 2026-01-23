#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>

#include "OverlayManager.h"
#include "ControllerTransform.h"

class RayPointer {
public:
    RayPointer(ControllerTransform& c_Transform);
    ~RayPointer();

    // Returns true if the ray hits the valid quad of the overlay
    bool isPointed(Overlay& overlay);

    // Returns the hit position in World Space (useful for drawing a laser dot)
    DirectX::XMVECTOR pointingPosWorld(Overlay& overlay);

    // Returns the hit position in Overlay Local Space (useful for UI interaction/mouse clicks)
    DirectX::XMVECTOR pointingPosOverlay(Overlay& overlay);

    // Returns the closest overlay currently pointed at, or nullptr if none
    Overlay* PointingAt(std::vector<Overlay*>& overlays);

private:
    ControllerTransform& controllerTransform;

    bool GetIntersection(Overlay& overlay, float& dist, DirectX::XMVECTOR& localHit);
};