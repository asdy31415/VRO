#include "OverlayManager.h"
#include <iostream>

Overlay::Overlay(ID3D11Device* device, ID3D11DeviceContext* context)
    : dxDevice(device), dxContext(context) {
        ID3D11Texture2D* texture = nullptr;
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
    overlayMatrix = DirectX::XMMatrixMultiply(matrix, overlayMatrix);
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

