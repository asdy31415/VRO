#include "Capturer.h"
#include <iostream>

// Sets DPI for consistent window scaling behavior.
BaseCapture::BaseCapture() {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

BaseCapture::~BaseCapture() {
    if (dxTexture) dxTexture->Release();
    delete[] pixels;
    if (hBitmap) DeleteObject(hBitmap);
    if (hMemoryDC) DeleteDC(hMemoryDC);
    if (hWindowDC) ReleaseDC(nullptr, hWindowDC);
}

// Allocate memory and GDI resources for bitmap capture.
bool BaseCapture::allocateBitmapResources(int w, int h) {
    width = w;
    height = h;

    // Create an in-memory DC compatible with the window DC.
    hMemoryDC = CreateCompatibleDC(hWindowDC);

    // Create a compatible bitmap to hold the captured pixels.
    hBitmap = CreateCompatibleBitmap(hWindowDC, width, height);
    SelectObject(hMemoryDC, hBitmap);

    // Allocate raw pixel buffer (aligned to 4 bytes per pixel).
    int rowPitch = ((width * 32 + 31) / 32) * 4;
    pixels = new unsigned char[rowPitch * height];
    return true;
}

// Create a D3D11 dynamic texture for GPU access.
bool BaseCapture::initDX(ID3D11Device* device, ID3D11DeviceContext* context) {
    dxDevice = device;
    dxContext = context;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = dxDevice->CreateTexture2D(&desc, nullptr, &dxTexture);
    if (FAILED(hr)) {
        std::cerr << "Failed to create DX texture" << std::endl;
        return false;
    }

    return true;
}

// Uploads CPU pixel buffer (from GetDIBits) to the D3D11 texture.
bool BaseCapture::updateTextureFromPixels() {
    if (!dxTexture || !dxContext || !pixels)
        return false;

    D3D11_MAPPED_SUBRESOURCE mapped;
    int rowPitch = ((width * 32 + 31) / 32) * 4;

    // Map the texture for CPU write access.
    if (SUCCEEDED(dxContext->Map(dxTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        for (int y = 0; y < height; y++) {
            memcpy(
                static_cast<BYTE*>(mapped.pData) + y * mapped.RowPitch,
                pixels + y * rowPitch,
                width * 4
            );
        }
        dxContext->Unmap(dxTexture, 0);
        return true;
    }
    return false;
}

/* ===============================
   WindowCapture Implementation
   =============================== */

// Sets up GDI resources for capturing a specific HWND.
WindowCapture::WindowCapture(HWND hwnd_) : hwnd(hwnd_) {
    RECT rect;
    if (GetWindowRect(hwnd, &rect)) {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    } else {
        std::cerr << "GetWindowRect failed!\n";
        width = height = 0;
    }
    // Get device context for the target window.
    hWindowDC = GetDC(hwnd);

    // Create memory DC + bitmap for off-screen capture.
    allocateBitmapResources(width, height);
}

WindowCapture::~WindowCapture() {
    if (hwnd && hWindowDC)
        ReleaseDC(hwnd, hWindowDC);
}

bool WindowCapture::captureFrame() {
    if (!hWindowDC || !hMemoryDC)
        return false;

    // Try PrintWindow first (works for most non-OpenGL windows).
    if (!PrintWindow(hwnd, hMemoryDC, PW_RENDERFULLCONTENT)) {
        // Fallback: direct BitBlt copy if PrintWindow fails.
        BitBlt(hMemoryDC, 0, 0, width, height, hWindowDC, 0, 0, SRCCOPY);
    }

    // Describe the bitmap layout for pixel extraction.
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(bi);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    // Retrieve pixel data into our buffer.
    if (!GetDIBits(hMemoryDC, hBitmap, 0, height, pixels, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS))
        return false;

    return updateTextureFromPixels();
}

/* ===============================
   ScreenCapture Implementation
   =============================== */
   
// Sets up resources for full-screen capture.
ScreenCapture::ScreenCapture(int screenIndex_) : screenIndex(screenIndex_) {

    DISPLAY_DEVICEW dd;
    dd.cb = sizeof(dd);
    bool found = false;
    wchar_t deviceName[32] = L"";

    // Enumerate all display devices until the requested index is found
    for (int i = 0; EnumDisplayDevicesW(nullptr, i, &dd, 0); i++) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            if (i == screenIndex) {
                wcscpy_s(deviceName, dd.DeviceName);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        std::cerr << "Invalid screen index: " << screenIndex << ", defaulting to primary display.\n";
        wcscpy_s(deviceName, L"\\\\.\\DISPLAY1");
    }

    // Create a device context for the target display
    hWindowDC = CreateDCW(L"DISPLAY", deviceName, nullptr, nullptr);
    if (!hWindowDC) {
        std::cerr << "Failed to create DC for display: " << deviceName << "\n";
        return;
    }

    // Query monitor geometry
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);

    HMONITOR hMon = MonitorFromPoint({ 0,0 }, MONITOR_DEFAULTTOPRIMARY);
    // Locate the specific monitor handle by enumerating monitors
    int monitorCount = 0;
    EnumDisplayMonitors(
        nullptr,
        nullptr,
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
            auto data = reinterpret_cast<std::pair<int, MONITORINFOEXW*>*>(lParam);
            if (data->first-- == 0) {
                data->second->cbSize = sizeof(MONITORINFOEXW);
                GetMonitorInfoW(hMonitor, data->second);
                return FALSE; // stop enumeration
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&std::pair<int, MONITORINFOEXW*>(screenIndex, &mi))
    );

    // Extract width and height of this monitor
    width = mi.rcMonitor.right - mi.rcMonitor.left;
    height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    // Create compatible memory DC and bitmap for capture
    allocateBitmapResources(width, height);
}


bool ScreenCapture::captureFrame() {
    if (!hWindowDC || !hMemoryDC)
        return false;

    BitBlt(hMemoryDC, 0, 0, width, height, hWindowDC, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(bi);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    if (!GetDIBits(hMemoryDC, hBitmap, 0, height, pixels, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS))
        return false;

    return updateTextureFromPixels();
}
