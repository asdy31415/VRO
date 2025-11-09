#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include "Capturer.h"

class Overlay {
public:
    Overlay(ID3D11Device* device, ID3D11DeviceContext* context);
    ~Overlay();

    void setTransform(const DirectX::XMMATRIX& matrix);
    void applyTransform(const DirectX::XMMATRIX& matrix);

    DirectX::XMMATRIX getTransformMatrix() const { return overlayMatrix; };
    ID3D11Texture2D* getTexture() const { return texture; }

    float OverlayWidth = 2.0f;

    void setCapture(std::unique_ptr<BaseCapture> newCapture);

    void update();
    
private:
    ID3D11Device* dxDevice;
    ID3D11DeviceContext* dxContext;

    ID3D11Texture2D* texture = nullptr;

    DirectX::XMMATRIX overlayMatrix = DirectX::XMMatrixIdentity();

    std::unique_ptr<BaseCapture> capture = nullptr;
};
