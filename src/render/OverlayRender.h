#include "OverlayManager.h"
#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

class OverlayRenderer {
public:
    OverlayRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
    ~OverlayRenderer();

    bool createVROverlay(Overlay& overlay);

    void destroyVROverlay(Overlay& ovelay);

    void render(Overlay& ovarlay);

    bool isInitialized() const { return vrSystem != nullptr; }
private:
    vr::IVRSystem* vrSystem = nullptr;
    vr::VROverlayHandle_t overlayHandle;

    ID3D11Device* dxDevice;
    ID3D11DeviceContext* dxContext;

    void setOverlay(Overlay &overlay);
};