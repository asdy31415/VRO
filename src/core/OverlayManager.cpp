#include "OverlayManager.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <sstream>

Overlay::Overlay(ID3D11Device* device, ID3D11DeviceContext* context)
    : dxDevice(device),
      dxContext(context),
      texture(nullptr)
{
    // Generate UUID
    std::random_device rd;
    std::stringstream ss;

    ss << std::hex << std::setfill('0');

    for (size_t i = 0; i < 16; i++) {
        unsigned char byte = static_cast<unsigned char>(rd() & 0xFF);
        ss << std::setw(2) << static_cast<int>(byte);
    }
    uuid = ss.str();
}

Overlay::~Overlay() {
    if (texture) {
        texture->Release();
        texture = nullptr;
    }
}

void Overlay::setTransform(const DirectX::XMMATRIX& matrix) {
    overlayMatrix = matrix;
    return;
}


void Overlay::applyTransform(const DirectX::XMMATRIX& matrix) {
    overlayMatrix = DirectX::XMMatrixMultiply(overlayMatrix, matrix);
}

void Overlay::setCapture(std::unique_ptr<BaseCapture> newCapture) {
    if (!newCapture) return;

    // Initialize the capture object with this overlay's device/context
    if (!newCapture->initDX(dxDevice, dxContext)) {
        std::cerr << "Failed to initialize capture DX texture.\n";
        return;
    }

    // Take ownership of the capture object
    capture = std::move(newCapture);
}

void Overlay::update() {
    if (capture && capture->captureFrame()) {
        texture = capture->getTexture();
        // Use tex for rendering overlay
    }
}

void Overlay::setVROverlayHandle(vr::VROverlayHandle_t handle) {
    overlayHandle = handle;
}
