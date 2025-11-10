#include "OverlayRender.h"
#include <iostream>

OverlayRenderer::OverlayRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
    : dxDevice(device), dxContext(context),
      vrSystem(nullptr)
{
    vr::EVRInitError eError = vr::VRInitError_None;

    // Initialize OpenVR as an overlay application
    vrSystem = vr::VR_Init(&eError, vr::VRApplication_Overlay);

    if (eError != vr::VRInitError_None) {
        vrSystem = nullptr;
        std::cerr << "OpenVR VR_Init failed: " << vr::VR_GetVRInitErrorAsSymbol(eError) << std::endl;
        return;
    }

    if (!vr::VROverlay()) {
        std::cerr << "OpenVR VROverlay() failed." << std::endl;
        vr::VR_Shutdown();
        vrSystem = nullptr;
        return;
    }
}

/* Note: The main application is responsible for calling
   destroyVROverlay() on all overlays BEFORE destroying
   this OverlayRenderer object.*/
OverlayRenderer::~OverlayRenderer() {
    if (vrSystem) {
        vr::VR_Shutdown();
    }
    vrSystem = nullptr;
}

// Creates the handle and stores it in the Overlay object
bool OverlayRenderer::createVROverlay(Overlay& overlay) {
    if (vrSystem == nullptr) {
        std::cerr << "Cannot create overlay, renderer not initialized." << std::endl;
        return false;
    }

    vr::IVROverlay* vrOverlay = vr::VROverlay();
    vr::VROverlayHandle_t newHandle = vr::k_ulOverlayHandleInvalid;

    // Create the overlay
    std::string uuid = overlay.getUUID();
    std::string sKey = "vro.auto.overlay" + uuid;
    std::string sName = "VROverlay" + uuid;

    vr::VROverlayError overlayError = vrOverlay->CreateOverlay(
        sKey.c_str(),
        sName.c_str(),
        &newHandle
    );

    if (overlayError != vr::VROverlayError_None) {
        std::cerr << "OpenVR CreateOverlay failed: " 
                  << vrOverlay->GetOverlayErrorNameFromEnum(overlayError) << std::endl;
        return false;
    }

    vrOverlay->SetOverlayWidthInMeters(newHandle, overlay.OverlayWidth);
    vrOverlay->SetOverlayInputMethod(newHandle, vr::VROverlayInputMethod_None);
    vrOverlay->ShowOverlay(newHandle);

    overlay.setVROverlayHandle(newHandle);

    return true;
}

void OverlayRenderer::destroyVROverlay(Overlay& overlay) {
    if (!vr::VROverlay()) return;

    vr::VROverlayHandle_t handle = overlay.getVROverlayHandle();
    if (handle != vr::k_ulOverlayHandleInvalid) {
        vr::VROverlay()->DestroyOverlay(handle);
        overlay.setVROverlayHandle(vr::k_ulOverlayHandleInvalid); // Mark as invalid
    }
}

void OverlayRenderer::render(Overlay& overlay) {
    vr::VROverlayHandle_t handle = overlay.getVROverlayHandle();

    // Do nothing if overlay handle is invalid
    if (handle == vr::k_ulOverlayHandleInvalid) {
        return;
    }

    vr::IVROverlay* vrOverlay = vr::VROverlay();
    if (!vrOverlay) {
        return; // OpenVR system not available
    }

    // Get Texture from Overlay
    ID3D11Texture2D* d3dTexture = overlay.getTexture();
    
    if (!d3dTexture) {
        // Texture isn't ready yet
        return;
    }

    // Set Overlay Texture
    vr::Texture_t vrTexture;
    vrTexture.handle = (void*)d3dTexture;
    vrTexture.eType = vr::TextureType_DirectX;
    vrTexture.eColorSpace = vr::ColorSpace_Auto;

    vrOverlay->SetOverlayTexture(handle, &vrTexture);
    
    // Set Overlay Transform
    DirectX::XMMATRIX xmMatrix = overlay.getTransformMatrix();
    
    vr::HmdMatrix34_t vrMatrix;
    memcpy(&vrMatrix, &xmMatrix, sizeof(vrMatrix));
    
    vrOverlay->SetOverlayTransformAbsolute(
        handle, 
        vr::TrackingUniverseStanding, 
        &vrMatrix
    );
}