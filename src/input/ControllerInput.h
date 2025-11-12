#include <openvr.h>
#include "VRSystemManager.h"
#include "ControllerManager.h"

enum class ButtonState {
    NOT_PRESSED = 0,
    ON_PRESSED = 1,
    PRESSING = 2,
    ON_RELEASE = 3
};

class ControllerButton {
public:
    ControllerButton(vr::EVRButtonId id, VRSystemManager& vr_manager, ControllerManager& c_manager);
    ~ControllerButton();

    void update();

    ButtonState getState() const { return currentState; }

    bool isButtonPressedThisFrame() const;

    bool isButtonReleasedThisFrame() const;

    bool isHeld() const;
    
private:
    vr::EVRButtonId buttonID;

    VRSystemManager& vrManager;
    ControllerManager& controllerManager;

    ButtonState currentState; 
    bool wasDownLastFrame;  
};