#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include <openvr.h>

#include "Capturer.h"

enum class OverlayReferenceSpace {
    World,
    Head,
    HandL,
    HandR
};

class Overlay {
public:
    Overlay(ID3D11Device* device, ID3D11DeviceContext* context);
    ~Overlay();

    void setTransform(const DirectX::XMMATRIX& matrix);
    void applyTransform(const DirectX::XMMATRIX& matrix);

    DirectX::XMMATRIX getTransformMatrix() const { return overlayMatrix; };
    ID3D11Texture2D* getTexture() const { return texture; }

    float OverlayWidth = 2.0f;

    float getAspectRatio();

    void setCapture(std::unique_ptr<BaseCapture> newCapture);

    void update();

    void setVROverlayHandle(vr::VROverlayHandle_t handle);

    vr::VROverlayHandle_t getVROverlayHandle() const { return overlayHandle; };

    void setReferenceSpace(OverlayReferenceSpace space);

    OverlayReferenceSpace getRefernceSpace() const { return referenceSpace; };

    void setRollingLock(bool enabled);

    bool getRollingLock() const { return rollingLock; };

    std::string getUUID() const { return uuid; }
    
private:
    ID3D11Device* dxDevice;
    ID3D11DeviceContext* dxContext;

    ID3D11Texture2D* texture = nullptr;

    DirectX::XMMATRIX overlayMatrix = DirectX::XMMatrixIdentity();

    OverlayReferenceSpace referenceSpace = OverlayReferenceSpace::World;

    bool rollingLock = false;

    std::unique_ptr<BaseCapture> capture = nullptr;

    vr::VROverlayHandle_t overlayHandle = vr::k_ulOverlayHandleInvalid;

    std::string uuid;
};
