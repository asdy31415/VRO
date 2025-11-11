#include <windows.h>
#include <vector>
#include <d3d11.h>

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
    bool allocateBitmapResources(int w, int h);
    bool updateTextureFromPixels();

    ID3D11Device* dxDevice = nullptr;
    ID3D11DeviceContext* dxContext = nullptr;
    ID3D11Texture2D* dxTexture = nullptr;

    HDC hWindowDC = nullptr;
    HDC hMemoryDC = nullptr;
    HBITMAP hBitmap = nullptr;
    unsigned char* pixels = nullptr;
    int width = 0;
    int height = 0;
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