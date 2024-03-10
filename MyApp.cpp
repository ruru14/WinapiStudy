#include "MyApp.h"

MyApp::MyApp() :
    myHwnd(nullptr),
    myDirect2dFactory(nullptr),
    myRenderTarget(nullptr),
    myLightSlateGrayBrush(nullptr),
    myCornflowerBlueBrush(nullptr),
    myWICFactory(nullptr),
    myBitmap(nullptr) {
}

MyApp::~MyApp() {
    DiscardDeviceResources();
    SAFE_RELEASE(myDirect2dFactory);
}

HRESULT MyApp::CreateDeviceIndependentResources() {
    HRESULT hr = S_OK;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &myDirect2dFactory);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, 
            IID_PPV_ARGS(&myWICFactory));
    }
    mySequenceBitmap = new MyBitmap();

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

HRESULT MyApp::LoadBitmapFromFile(PCWSTR uri, ID2D1Bitmap** ppBitmap) {
    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    if (!myWICFactory) CreateDeviceIndependentResources();
    if (!myRenderTarget) CreateDeviceResources();

    HRESULT hr = myWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );

    if (SUCCEEDED(hr)) {
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr)) {
        hr = myWICFactory->CreateFormatConverter(&pConverter);
    }

    if (SUCCEEDED(hr)) {
        hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
        );
    }
    if (SUCCEEDED(hr)) {
        hr = myRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
        );
    }

    SAFE_RELEASE(pDecoder);
    SAFE_RELEASE(pSource);
    SAFE_RELEASE(pStream);
    SAFE_RELEASE(pConverter);
    SAFE_RELEASE(pScaler);

    return hr;
}

HRESULT MyApp::LoadBitmapFromFile2(PCWSTR uri, MyBitmap* myBitmap) {
    IWICBitmapDecoder* pDecoder = NULL;
    std::vector<ID2D1Bitmap*> bitmapArr;

    if (!myWICFactory) CreateDeviceIndependentResources();
    if (!myRenderTarget) CreateDeviceResources();

    HRESULT hr = myWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );

    UINT frameCount = -1;
    if (SUCCEEDED(hr)) {
        hr = pDecoder->GetFrameCount(&frameCount);
    }
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < frameCount; i++) {
            IWICFormatConverter* pConverter = nullptr;
            IWICBitmapFrameDecode* tmpSource = nullptr;
            ID2D1Bitmap* tmpBitmap = nullptr;
            if (SUCCEEDED(hr)) {
                hr = myWICFactory->CreateFormatConverter(&pConverter);
            }
            if (SUCCEEDED(hr)) {
                hr = pDecoder->GetFrame(i, &tmpSource);
            }
            if (SUCCEEDED(hr)) {
                hr = pConverter->Initialize(
                    tmpSource,
                    GUID_WICPixelFormat32bppPBGRA,
                    WICBitmapDitherTypeNone,
                    NULL,
                    0.f,
                    WICBitmapPaletteTypeMedianCut
                );
            }
            if (SUCCEEDED(hr)) {
                hr = myRenderTarget->CreateBitmapFromWicBitmap(
                    pConverter,
                    NULL,
                    &tmpBitmap
                );
            }
            if (SUCCEEDED(hr)) {
                bitmapArr.push_back(std::move(tmpBitmap));
            }
            SAFE_RELEASE(pConverter);
            SAFE_RELEASE(tmpSource);
        }
    }
    if (myBitmap) {
        myBitmap->Initialize(frameCount, bitmapArr);
    }

    SAFE_RELEASE(pDecoder);

    return hr;
}

void MyApp::DiscardDeviceResources() {
    SAFE_RELEASE(myBitmap);
    SAFE_RELEASE(myRenderTarget);
    SAFE_RELEASE(myLightSlateGrayBrush);
    SAFE_RELEASE(myCornflowerBlueBrush);
}

HRESULT MyApp::initialize(HINSTANCE hInstance) {
    QueryPerformanceFrequency(&myFrequency);
    QueryPerformanceCounter(&myPrevTime);

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

    std::cout << "Console::Initialize\n";
    TRACE(L"TRACE::Initialize\n");

    LoadBitmapFromFile(L"dx_logo.png", &myBitmap);
    LoadBitmapFromFile2(L"loading.gif", mySequenceBitmap);

    return hr;
}

void MyApp::runMessageLoop() {
    MSG msg;

    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            // Error handling
        } else {
            HandleKeyboardInput();
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
            case WM_KEYDOWN:
            case WM_KEYUP:
                switch (wParam) {
                case 'A':
                    if (lParam & 0x80000000) {
                        std::cout << "A Up\n";
                    } else {
                        std::cout << "A Down\n";
                    }
                    break;
                case VK_CONTROL:
                    if (lParam & 0x80000000) {
                        std::cout << "Ctrl Up\n";
                    } else {
                        std::cout << "Ctrl Down\n";
                    }
                    break;
                }
                result = 0; wasHandled = true;
                break;
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

void MyApp::HandleKeyboardInput() {
    bool isCtrl = GetAsyncKeyState(VK_CONTROL) & 0x8000;
    if (isCtrl && (GetAsyncKeyState('D') & 0x8000)) {
        std::cout << "Ctrl D\n";
    }

    if (GetAsyncKeyState('S') & 0x8000) {
        inputFlag['S'] = true;
        std::cout << "Input S\n";
    } else {
        if (inputFlag['S']) {
            inputFlag['S'] = false;
            std::cout << "Release S\n";
        }
    }
}

HRESULT MyApp::OnRender() {
    HRESULT hr = S_OK;
    hr = CreateDeviceResources();
    if (SUCCEEDED(hr)) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        FLOAT deltaTime = (FLOAT)(
            (DOUBLE)(currentTime.QuadPart - myPrevTime.QuadPart) / (DOUBLE)(myFrequency.QuadPart)
            );
        myPrevTime = currentTime;

        frameTime.push_back(deltaTime);
        while (frameTime.size() > frameAvgCount) frameTime.pop_front();

        FLOAT totalTime = 0.0f;
        for (auto& i : frameTime) {
            totalTime += i;
        }
        FLOAT frameRate = 1 / (totalTime / frameAvgCount);
        //std::cout << frameRate << "\n";

        mySequenceBitmap->Tick(deltaTime);

        myRenderTarget->BeginDraw();
        myRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        myRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        D2D1_SIZE_F rtSize = myRenderTarget->GetSize();

        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);

        if (myBitmap) {
            myRenderTarget->DrawBitmap(myBitmap,
                D2D1::RectF(
                    0.0f, 0.0f,
                    rtSize.width, rtSize.height
                ));
        }

        mySequenceBitmap->Move(deltaTime * 10, deltaTime * 10);
        if (mySequenceBitmap) {
            ID2D1Bitmap* tmp = mySequenceBitmap->GetBitmap();
            if (tmp) {
                myRenderTarget->DrawBitmap(tmp,
                    mySequenceBitmap->GetBitmapPosition()
                );
            }
        }

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
    if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        if (SUCCEEDED(app->initialize(hInstance))) {
            app->runMessageLoop();
        }
    }

    return 0;
}