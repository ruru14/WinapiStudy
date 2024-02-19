#include "MyApp.h"

MyApp::MyApp() :
    myHwnd(nullptr),
    myDirect2dFactory(nullptr),
    myRenderTarget(nullptr),
    myLightSlateGrayBrush(nullptr),
    myCornflowerBlueBrush(nullptr) {
}

MyApp::~MyApp() {
    DiscardDeviceResources();
    SAFE_RELEASE(myDirect2dFactory);
}

HRESULT MyApp::CreateDeviceIndependentResources() {
    HRESULT hr = S_OK;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &myDirect2dFactory);

    return hr;
}

HRESULT MyApp::CreateDeviceResources() {
    HRESULT hr = S_OK;
    if (!myRenderTarget) {
        RECT rc; GetClientRect(myHwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        hr = myDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(myHwnd, size),
            &myRenderTarget
        );

        if (SUCCEEDED(hr)) {
            hr = myRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray), &myLightSlateGrayBrush
            );
        }
        if (SUCCEEDED(hr)) {
            hr = myRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &myCornflowerBlueBrush
            );
        }
    }

    return hr;
}

void MyApp::DiscardDeviceResources() {
    SAFE_RELEASE(myRenderTarget);
    SAFE_RELEASE(myLightSlateGrayBrush);
    SAFE_RELEASE(myCornflowerBlueBrush);
}

HRESULT MyApp::initialize(HINSTANCE hInstance) {
    HRESULT hr = CreateDeviceIndependentResources();
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
        CreateDeviceResources();
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
            case WM_SIZE:
            {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            myApp->OnResize(width, height);
            }
                result = 0; wasHandled = true;
                break;
            case WM_DISPLAYCHANGE:
                InvalidateRect(hWnd, NULL, FALSE);
                result = 0; wasHandled = true;
                break;
            case WM_PAINT:
                myApp->OnRender();
                //ValidateRect(hWnd, NULL);
                result = 0; wasHandled = true;
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
                break;
            }
        }
        if (!wasHandled) {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }

    }

    return result;
}

void MyApp::OnResize(UINT width, UINT height) {
    if (myRenderTarget) {
        myRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}

HRESULT MyApp::OnRender() {
    HRESULT hr = S_OK;
    hr = CreateDeviceResources();
    if (SUCCEEDED(hr)) {
        myRenderTarget->BeginDraw();
        myRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        myRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        D2D1_SIZE_F rtSize = myRenderTarget->GetSize();

        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);
        for (int x = 0; x < width; x += 10) {
            myRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                myLightSlateGrayBrush, 0.5f);
        }
        for (int y = 0; y < height; y += 10) {
            myRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                myLightSlateGrayBrush, 0.5f);
        }

        D2D1_RECT_F rectangle1 = D2D1::RectF(
            rtSize.width / 2 - 50.0f, rtSize.height / 2 - 50.0f,
            rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f
        );
        D2D1_RECT_F rectangle2 = D2D1::RectF(
            rtSize.width / 2 - 100.0f, rtSize.height / 2 - 100.0f,
            rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f
        );
        myRenderTarget->FillRectangle(&rectangle1, myLightSlateGrayBrush);
        myRenderTarget->DrawRectangle(&rectangle2, myCornflowerBlueBrush);

        hr = myRenderTarget->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
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