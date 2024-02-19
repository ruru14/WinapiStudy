#pragma once

#include <Windows.h>

class MyApp {
public:
    HRESULT initialize(HINSTANCE hInstance);
    void runMessageLoop();

private:
    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND myHwnd = nullptr;
};