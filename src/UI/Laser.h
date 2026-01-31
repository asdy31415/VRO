#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include "ControllerTransform.h"
#include "OverlayManager.h"


class LaserBeam {
public:
    // The overlay passed here should have the "Beam" texture loaded.
    LaserBeam(ControllerTransform& transform, Overlay& beamOverlay);
    ~LaserBeam();

    void update();

    // Set how wide the beam is (in meters). 
    // The Length is automatically calculated based on the texture's aspect ratio.
    void setWidth(float widthMeters);

    static ID3D11Texture2D* CreateSolidTexture(ID3D11Device* device, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

private:
    ControllerTransform& m_controller;
    Overlay& m_overlay;
    // Helper to calculate the correct matrix
    DirectX::XMMATRIX CalculateBeamTransform();

    float m_width = 0.01f;
    float m_length = 2.0f;
};