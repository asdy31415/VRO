#include <openvr.h>
#include "VRSystemManager.h"

enum class ButtonState {
    NOT_PRESSED = 0,
    ON_PRESSED = 1,
    PRESSING = 2,
    ON_RELEASE = 3
};

class ControllerButton {
public:
    ControllerButton(vr::EVRButtonId id, VRSystemManager& manager);
    ~ControllerButton();

    void update();

    ButtonState getState() const { return currentState; }

    bool isButtonPressedThisFrame() const;

    bool isButtonReleasedThisFrame() const;

    bool isHeld() const;
    
private:
    vr::TrackedDeviceIndex_t controllerIndex;
    vr::EVRButtonId buttonID;

    VRSystemManager& vrManager;

    ButtonState currentState; 
    bool wasDownLastFrame;  
};