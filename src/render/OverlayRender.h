#include "OverlayManager.h"
#include "VRSystemManager.h"
#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

class OverlayRenderer {
public:
    OverlayRenderer(ID3D11Device* device, ID3D11DeviceContext* context, VRSystemManager& manager);
    ~OverlayRenderer();

    bool createVROverlay(Overlay& overlay);

    void destroyVROverlay(Overlay& ovelay);

    void render(Overlay& ovarlay);

private:
    VRSystemManager& vrManager;

    ID3D11Device* dxDevice;
    ID3D11DeviceContext* dxContext;
};