#include <openvr.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "VRSystemManager.h"
#include "ControllerTransform.h"
#include "ControllerInput.h"
#include "OverlayManager.h"
#include "RayPointer.h"

class RotationCalculation {
public:
    static float getLevelRotationCos(DirectX::XMVECTOR axis, DirectX::XMVECTOR screenTop);

    static DirectX::XMMATRIX getRotationMatrixAlongAxis(DirectX::XMVECTOR axis, float rotationCos);

};


class Grabber {
public:
    Grabber(Overlay& ovelay);
    ~Grabber();

};
