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
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

// My classes
#include "MyBitmap.h"

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
    HRESULT LoadBitmapFromFile(
        PCWSTR uri,
        ID2D1Bitmap** ppBitmap);
    HRESULT LoadBitmapFromFile2(
        PCWSTR uri,
        MyBitmap* myBitmap);
    void DiscardDeviceResources();
    HRESULT OnRender();
    void OnResize(UINT width, UINT height);
    void HandleKeyboardInput();

private:
    HWND myHwnd = nullptr;
    ID2D1Factory* myDirect2dFactory = nullptr;
    ID2D1HwndRenderTarget* myRenderTarget = nullptr;
    ID2D1SolidColorBrush* myLightSlateGrayBrush = nullptr;
    ID2D1SolidColorBrush* myCornflowerBlueBrush = nullptr;

    IWICImagingFactory* myWICFactory = nullptr;
    ID2D1Bitmap* myBitmap = nullptr;

    MyBitmap* mySequenceBitmap = nullptr;

    LARGE_INTEGER myPrevTime;
    LARGE_INTEGER myFrequency;

    std::deque<FLOAT> frameTime;
    const int frameAvgCount = 10;

    std::map<DWORD, bool> inputFlag;

};