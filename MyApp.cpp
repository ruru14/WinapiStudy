#include "MyApp.h"

HRESULT MyApp::initialize(HINSTANCE hInstance) {
    HRESULT hr;
    WNDCLASSEX winCls = { sizeof(WNDCLASSEX) };
    winCls.style = CS_HREDRAW | CS_VREDRAW;
    winCls.lpfnWndProc = MyApp::WndProc;
    winCls.cbClsExtra = 0;
    winCls.cbWndExtra = sizeof(LONG_PTR);
    winCls.hInstance = hInstance;
    winCls.hbrBackground = nullptr;
    winCls.lpszMenuName = nullptr;
    winCls.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
    winCls.lpszClassName = L"MyApp";
    RegisterClassEx(&winCls);
    myHwnd = CreateWindowW(
        L"MyApp", L"MyAppName",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480, nullptr, nullptr, hInstance, this
    );
    hr = myHwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        ShowWindow(myHwnd, SW_SHOWNORMAL);
        UpdateWindow(myHwnd);
    }

    return hr;
}

void MyApp::runMessageLoop() {
    MSG msg;

    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            // Error handling
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

LRESULT MyApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    if (message == WM_CREATE) {
        LPCREATESTRUCT cStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        MyApp* myApp = static_cast<MyApp*>(cStruct->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(myApp));

        result = 1;
    } else {
        MyApp* myApp = reinterpret_cast<MyApp*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        bool wasHandled = false;

        if (myApp) {
            switch (message) {
            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
            }
        }
        if (!wasHandled) {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }

    }

    return result;
}

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nShowCmd
) {
    MyApp* app = new MyApp();
    if (SUCCEEDED(app->initialize(hInstance))) {
        app->runMessageLoop();
    }

    return 0;
}