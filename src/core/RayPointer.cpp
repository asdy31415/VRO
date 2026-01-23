#include "RayPointer.h"
#include "OverlayManager.h" 
#include "ControllerTransform.h" // Ensure this header is available

RayPointer::RayPointer(ControllerTransform& c_Transform) 
    : controllerTransform(c_Transform) 
{
}

RayPointer::~RayPointer() 
{
}


bool RayPointer::GetIntersection(Overlay& overlay, float& t, DirectX::XMVECTOR& localHitPos) {
    DirectX::XMVECTOR rayOriginWorld = controllerTransform.getTranslation();
    
    DirectX::XMVECTOR rayDirLocal = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); 
    
    DirectX::XMVECTOR ctrlRotQuat = controllerTransform.getRotation();
    DirectX::XMVECTOR rayDirWorld = DirectX::XMVector3Rotate(rayDirLocal, ctrlRotQuat);

    DirectX::XMMATRIX overlayMatrix = overlay.getTransformMatrix();
    DirectX::XMVECTOR determinant = XMMatrixDeterminant(overlayMatrix);
    DirectX::XMMATRIX invOverlayMatrix = XMMatrixInverse(&determinant, overlayMatrix);

    DirectX::XMVECTOR rayOriginLocal = XMVector3TransformCoord(rayOriginWorld, invOverlayMatrix);

    DirectX::XMVECTOR rayDirLocalOverlay = XMVector3TransformNormal(rayDirWorld, invOverlayMatrix);
    
    float dirZ = DirectX::XMVectorGetZ(rayDirLocalOverlay);
    float originZ = DirectX::XMVectorGetZ(rayOriginLocal);

    if (fabs(dirZ) < 1e-6f) {
        return false;
    }

    t = -originZ / dirZ;

    if (t < 0.0f) {
        return false;
    }

    localHitPos = DirectX::XMVectorAdd(rayOriginLocal, DirectX::XMVectorScale(rayDirLocalOverlay, t));

    return true;
}


bool RayPointer::isPointed(Overlay& overlay) {
    float t = 0.0f;
    DirectX::XMVECTOR localHit;
    
    if (!GetIntersection(overlay, t, localHit)) {
        return false;
    }

    float aspectRatio = overlay.getAspectRatio();
    float overlayHeight = overlay.OverlayWidth / aspectRatio;

    float localX = DirectX::XMVectorGetX(localHit);
    float localY = DirectX::XMVectorGetY(localHit);

    float halfWidth = overlay.OverlayWidth / 2.0f;
    float halfHeight = overlayHeight / 2.0f;

    if (localX >= -halfWidth && localX <= halfWidth &&
        localY >= -halfHeight && localY <= halfHeight) {
        return true;
    }

    return false;
}

DirectX::XMVECTOR RayPointer::pointingPosWorld(Overlay& overlay) {
    float t = 0.0f;
    DirectX::XMVECTOR localHit;

    GetIntersection(overlay, t, localHit);

    return XMVector3TransformCoord(localHit, overlay.getTransformMatrix());
}

DirectX::XMVECTOR RayPointer::pointingPosOverlay(Overlay& overlay) {
    float t = 0.0f;
    DirectX::XMVECTOR localHit;
    
    if(GetIntersection(overlay, t, localHit)) {
        return localHit;
    }
    
    return DirectX::XMVectorZero();
}

Overlay* RayPointer::PointingAt(std::vector<Overlay*>& overlays) {
    Overlay* closestOverlay = nullptr;
    float minDistance = FLT_MAX;

    for (Overlay* overlay : overlays) {
        if (!overlay) continue;

        float t = 0.0f;
        DirectX::XMVECTOR localHit;

        if (GetIntersection(*overlay, t, localHit)) {
            


            float aspectRatio = overlay->getAspectRatio();
            float halfH = (overlay->OverlayWidth / aspectRatio) / 2.0f;
            float halfW = overlay->OverlayWidth / 2.0f;

            float x = DirectX::XMVectorGetX(localHit);
            float y = DirectX::XMVectorGetY(localHit);

            if (x >= -halfW && x <= halfW && y >= -halfH && y <= halfH) {

                if (t < minDistance) {
                    minDistance = t;
                    closestOverlay = overlay;
                }
            }
        }
    }

    return closestOverlay;
}