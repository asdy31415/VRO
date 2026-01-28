#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include "ControllerTransform.h"
#include "OverlayManager.h"


class Laser {
public:
    Laser(ID3D11Device* device, ID3D11DeviceContext* context);
    ~Laser();

    void update(ControllerTransform& controller);

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;

    std::unique_ptr<Overlay> beamOverlay;
    
    ID3D11Texture2D* CreateSolidTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};
