#include "Capturer.h"

#include <iostream>

#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h> 

namespace WGD = winrt::Windows::Graphics::DirectX;
namespace WGC = winrt::Windows::Graphics::Capture;

extern "C" {
    HRESULT __stdcall CreateDirect3D11DeviceFromDXGIDevice(::IDXGIDevice* dxgiDevice,
        ::IInspectable** graphicsDevice);
}

// Sets DPI for consistent window scaling behavior.
BaseCapture::BaseCapture() {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (...) {
        // Apartment might already be initialized by the host application
    }
}

BaseCapture::~BaseCapture() {
    StopCaptureInternal();
    if (dxTexture) dxTexture->Release();
}


// Create a D3D11 dynamic texture for GPU access.
bool BaseCapture::initDX(ID3D11Device* device, ID3D11DeviceContext* context) {
    dxDevice = device;
    dxContext = context;

    if (!dxDevice || !item) return false;

    winrt::com_ptr<::IDXGIDevice> dxgiDevice;
    dxDevice->QueryInterface(dxgiDevice.put());

    winrt::com_ptr<::IInspectable> inspectableDevice;
    HRESULT hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectableDevice.put());
    if (FAILED(hr)) {
        std::cerr << "Failed to wrap D3D11 device for WGC." << std::endl;
        return false;
    }
    wgcDevice = inspectableDevice.as<WGD::Direct3D11::IDirect3DDevice>();

    // 2. Start the WGC session
    StartCaptureInternal();

    return true;
}

void BaseCapture::StartCaptureInternal() {
    if (!item || !wgcDevice) return;

    // We use CreateFreeThreaded to avoid needing a DispatcherQueue on the calling thread.
    // This is crucial for console apps or background threads.
    framePool = WGC::Direct3D11CaptureFramePool::CreateFreeThreaded(
        wgcDevice,
        WGD::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        2,
        item.Size());

    session = framePool.CreateCaptureSession(item);

    // Handle Frame Arrived
    framePool.FrameArrived([this](WGC::Direct3D11CaptureFramePool const& sender, IInspectable const&) {
        auto frame = sender.TryGetNextFrame();
        if (frame) {
            std::lock_guard<std::mutex> lock(frameMutex);
            latestFrame = frame;
            newFrameAvailable = true;
        }
    });

    session.StartCapture();
}


void BaseCapture::StopCaptureInternal() {
    if (session) {
        session.Close();
        session = nullptr;
    }
    if (framePool) {
        framePool.Close();
        framePool = nullptr;
    }
}

bool BaseCapture::captureFrame() {
    if (!dxDevice || !dxContext) return false;

    // Check if WGC gave us a new frame
    WGC::Direct3D11CaptureFrame frameToProcess{ nullptr };
    {
        std::lock_guard<std::mutex> lock(frameMutex);
        if (newFrameAvailable && latestFrame) {
            frameToProcess = latestFrame;
            newFrameAvailable = false;
        }
    }

    // If we have a new frame, update our main dxTexture
    if (frameToProcess) {
        // Get the texture interface from the WGC frame
        auto surface = frameToProcess.Surface();
        auto surfaceInterop = surface.as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
        
        winrt::com_ptr<ID3D11Texture2D> wgcTexture;
        surfaceInterop->GetInterface(IID_PPV_ARGS(wgcTexture.put()));

        if (wgcTexture) {
            D3D11_TEXTURE2D_DESC desc;
            wgcTexture->GetDesc(&desc);

            // Create or Recreate dxTexture if dimensions changed or it doesn't exist
            if (!dxTexture || width != desc.Width || height != desc.Height) {
                if (dxTexture) dxTexture->Release();
                
                width = desc.Width;
                height = desc.Height;

                D3D11_TEXTURE2D_DESC internalDesc = desc;
                internalDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                internalDesc.MiscFlags = 0;
                internalDesc.CPUAccessFlags = 0; // GPU to GPU copy, no CPU access needed
                internalDesc.Usage = D3D11_USAGE_DEFAULT;

                if (FAILED(dxDevice->CreateTexture2D(&internalDesc, nullptr, &dxTexture))) {
                    std::cerr << "Failed to create internal texture." << std::endl;
                    return false;
                }
                
                // If size changed, we might need to recreate the frame pool, 
                // but WGC handles standard resizes reasonably well. 
                // For robust resizing, we would call framePool.Recreate() here.
                framePool.Recreate(wgcDevice, WGD::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, item.Size());
            }

            // Copy GPU memory from WGC texture to our internal texture
            dxContext->CopyResource(dxTexture, wgcTexture.get());
            return true;
        }
    }

    // Return true if we have a valid texture at all (even if not updated this specific tick)
    return (dxTexture != nullptr);
}

WindowCapture::WindowCapture(HWND hwnd_) : hwnd(hwnd_) {

    auto interopFactory = winrt::get_activation_factory<
        WGC::GraphicsCaptureItem, 
        ::IGraphicsCaptureItemInterop // Global scope interface
    >();

    try {
        interopFactory->CreateForWindow(
            hwnd, 
            winrt::guid_of<WGC::GraphicsCaptureItem>(), 
            winrt::put_abi(item)
        );
        if (item) {
            auto size = item.Size();
            width = size.Width;
            height = size.Height;
        }
    } catch (winrt::hresult_error const& ex) {
        std::cerr << "WindowCapture failed: " << winrt::to_string(ex.message()) << std::endl;
    }
}

WindowCapture::~WindowCapture() {}

ScreenCapture::ScreenCapture(int screenIndex_) : screenIndex(screenIndex_) {
    // 1. Find the HMONITOR for the given index
    struct MonitorEnumData {
        int targetIndex;
        int currentIndex;
        HMONITOR result;
    } data = { screenIndex, 0, nullptr };

    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
        auto* pData = reinterpret_cast<MonitorEnumData*>(lParam);
        if (pData->currentIndex == pData->targetIndex) {
            pData->result = hMonitor;
            return FALSE; // Stop enumerating
        }
        pData->currentIndex++;
        return TRUE;
    }, reinterpret_cast<LPARAM>(&data));

    if (!data.result) {
        std::cerr << "ScreenCapture: Invalid screen index " << screenIndex << std::endl;
        return;
    }

    // 2. Create CaptureItem from HMONITOR
    auto interopFactory = winrt::get_activation_factory<
        WGC::GraphicsCaptureItem, 
        ::IGraphicsCaptureItemInterop
    >();
    
    try {
        interopFactory->CreateForMonitor(
            data.result, 
            winrt::guid_of<WGC::GraphicsCaptureItem>(), 
            winrt::put_abi(item)
        );
        if (item) {
            auto size = item.Size();
            width = size.Width;
            height = size.Height;
        }
    } catch (winrt::hresult_error const& ex) {
        std::cerr << "ScreenCapture failed: " << winrt::to_string(ex.message()) << std::endl;
    }
}
