#include <windows.h>
#include <vector>
#include <d3d11.h>
#include <mutex>
#include <atomic>

#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/base.h>

class BaseCapture {
public:
    virtual ~BaseCapture();

    virtual bool captureFrame() = 0;
    virtual bool initDX(ID3D11Device* device, ID3D11DeviceContext* context);
    ID3D11Texture2D* getTexture() const { return dxTexture; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    BaseCapture();
    
    void StartCaptureInternal();
    void StopCaptureInternal();

    ID3D11Device* dxDevice = nullptr;
    ID3D11DeviceContext* dxContext = nullptr;
    ID3D11Texture2D* dxTexture = nullptr;

    HDC hWindowDC = nullptr;
    HDC hMemoryDC = nullptr;
    HBITMAP hBitmap = nullptr;
    unsigned char* pixels = nullptr;
    int width = 0;
    int height = 0;

    winrt::Windows::Graphics::Capture::GraphicsCaptureItem item{ nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession session{ nullptr };
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice wgcDevice{ nullptr };

    // Thread safety for async frame arrival
    std::mutex frameMutex;
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame latestFrame{ nullptr };
    std::atomic<bool> newFrameAvailable{ false };
};

class WindowCapture : public BaseCapture {
public:
    explicit WindowCapture(HWND hwnd);
    ~WindowCapture() override;

    bool captureFrame() override;

private:
    HWND hwnd;
};

class ScreenCapture : public BaseCapture {
public:
    explicit ScreenCapture(int screenIndex);
    ~ScreenCapture() override;

    bool captureFrame() override;

private:
    int screenIndex;
};