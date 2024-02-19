#pragma once

// Default headers
#include <windows.h>
#include <tchar.h>

// DirectX headers
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

class MyApp {
public:
    MyApp();
    ~MyApp();
    HRESULT initialize(HINSTANCE hInstance);
    void runMessageLoop();

private:
    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    void DiscardDeviceResources();
    HRESULT OnRender();
    void OnResize(UINT width, UINT height);

private:
    HWND myHwnd = nullptr;
    ID2D1Factory* myDirect2dFactory = nullptr;
    ID2D1HwndRenderTarget* myRenderTarget = nullptr;
    ID2D1SolidColorBrush* myLightSlateGrayBrush = nullptr;
    ID2D1SolidColorBrush* myCornflowerBlueBrush = nullptr;
};