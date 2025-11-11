#include <openvr.h>

class VRSystemManager {
public:
    VRSystemManager();
    ~VRSystemManager();

    bool isInitialized() const { return vrSystem != nullptr; }

    vr::IVRSystem* getVRSystem() const { return vrSystem; }

private:
    vr::IVRSystem* vrSystem = nullptr;

    VRSystemManager(const VRSystemManager&) = delete;
    VRSystemManager& operator=(const VRSystemManager&) = delete;

};