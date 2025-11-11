#include <openvr.h>

class ControllerButton {
public:
    ControllerButton();
    ~ControllerButton();

    void updateState();

    int getButtonState() const { return buttonState; }

    bool isButtonPressed() const;
    
private:
    int buttonState;

    
};