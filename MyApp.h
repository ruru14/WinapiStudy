#pragma once

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

// Default headers
#include <windows.h>
#include <tchar.h>
#include <deque>
#include <map>

#include <iostream>

// DirectX headers
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d2d1effects_2.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <d3d11.h>

// My classes
#include "MyBitmap.h"

using namespace Microsoft::WRL;

#if defined(_DEBUG) && defined(WIN32) && !defined(_AFX) && !defined(_AFXDLL)
#define TRACE TRACE_WIN32
void TRACE_WIN32(LPCTSTR lpszFormat, ...) {
    TCHAR lpszBuffer[0x160];
    va_list fmtList;
    va_start(fmtList, lpszFormat);
    _vstprintf_s(lpszBuffer, lpszFormat, fmtList);
    va_end(fmtList);
    OutputDebugString(lpszBuffer);
}
#else
#define TRACE
#endif

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
    HRESULT CreateDeviceContext();
    HRESULT LoadBitmapFromFile(
        PCWSTR uri,
        ID2D1Bitmap** ppBitmap);
    HRESULT LoadBitmapFromFile2(
        PCWSTR uri,
        MyBitmap* myBitmap);
    void DiscardDeviceResources();
    void Update();
    HRESULT OnRender();
    void OnResize(UINT width, UINT height);
    void HandleKeyboardInput();

    D2D1_SIZE_U CalculateD2DWindowSize();

private:
    HWND myHwnd = nullptr;
    ComPtr<ID2D1Factory1> myDirect2dFactory;
    //ComPtr<ID2D1HwndRenderTarget> myRenderTarget;
    ComPtr<ID2D1SolidColorBrush> myLightSlateGrayBrush;
    ComPtr<ID2D1SolidColorBrush> myCornflowerBlueBrush;

    ComPtr<IWICImagingFactory2> myWICFactory;
    ComPtr<ID2D1Bitmap> myBitmap;

    ComPtr<ID2D1Device> myDirect2dDevice;
    ComPtr<ID2D1DeviceContext> myDirect2dContext;
    ComPtr<IDXGISwapChain> mySwapChain;

    std::shared_ptr<MyBitmap> mySequenceBitmap;
    std::shared_ptr<MyBitmap> myCharacterBitmap;
    FLOAT MoveSpeed = 140.f;
    FLOAT gravity = 98.f;
    FLOAT downSpeed = 10.f;
    FLOAT MoveDirection[4] = {0.f, 0.f, 0.f, 0.f};
    bool isLeft = false;

    LARGE_INTEGER myPrevTime;
    LARGE_INTEGER myFrequency;

    std::deque<FLOAT> frameTime;
    const int frameAvgCount = 10;

    std::map<DWORD, bool> inputFlag;

};