#include "Laser.h"
#include "ControllerTransform.h"
#include "OverlayManager.h" // Needed for Overlay definition

LaserBeam::LaserBeam(ControllerTransform& transform, Overlay& beamOverlay)
    : m_controller(transform), m_overlay(beamOverlay)
{
    // Initialize with a reasonable default width (e.g., 2cm)
    // You can tune this or change it at runtime
    m_overlay.OverlayWidth = 0.02f; 
}

LaserBeam::~LaserBeam()
{
}

void LaserBeam::setWidth(float widthMeters) {
    m_overlay.OverlayWidth = widthMeters;
}

void LaserBeam::update() {

    DirectX::XMMATRIX matRot = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2); // 90 degrees

    float stretchFactor = m_length / m_width;
    DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(1.0f, stretchFactor, 1.0f);

    DirectX::XMMATRIX matTrans = DirectX::XMMatrixTranslation(0.0f, 0.0f, -m_length / 2.0f);

    // Combine Local: Rotate THEN Move
    DirectX::XMMATRIX beamLocal = DirectX::XMMatrixMultiply(matScale, matRot);
    beamLocal = DirectX::XMMatrixMultiply(beamLocal, matTrans);

    DirectX::XMMATRIX finalMat = DirectX::XMMatrixMultiply(beamLocal, m_controller.getTransform());

    m_overlay.setTransform(finalMat);
}

ID3D11Texture2D* LaserBeam::CreateSolidTexture(ID3D11Device* device, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // 1x1 Pixel
    const int width = 1;
    const int height = 1;

    // RGBA Data (4 bytes)
    uint8_t pixel[4] = { r, g, b, a };

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixel;
    initData.SysMemPitch = width * 4; // Pitch doesn't matter much for 1x1, but correct is 4

    ID3D11Texture2D* texture = nullptr;
    device->CreateTexture2D(&desc, &initData, &texture);

    return texture;
}