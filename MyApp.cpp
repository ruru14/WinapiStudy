#include "MyApp.h"

MyApp::MyApp() : 
    myDirect2dFactory(nullptr),
    myLightSlateGrayBrush(nullptr),
    myCornflowerBlueBrush(nullptr),
    myWICFactory(nullptr),
    myBitmap(nullptr),
    myDirect2dDevice(nullptr),
    myDirect2dContext(nullptr),
    mySwapChain(nullptr) {
}

MyApp::~MyApp() {
    DiscardDeviceResources();
}

HRESULT MyApp::CreateDeviceIndependentResources() {
    HRESULT hr = S_OK;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_PPV_ARGS(&myDirect2dFactory));
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&myWICFactory));
    }
    mySequenceBitmap = std::make_shared<MyBitmap>();
    myCharacterBitmap = std::make_shared<MyBitmap>();

    return hr;
}

HRESULT MyApp::CreateDeviceResources() {
    HRESULT hr = S_OK;

    if (!myDirect2dContext) {
        hr = CreateDeviceContext();

        ComPtr<IDXGISurface> surface = nullptr;
        if (SUCCEEDED(hr)) {
            hr = mySwapChain->GetBuffer(
                0,
                IID_PPV_ARGS(&surface)
            );
        }
        ComPtr<ID2D1Bitmap1> bitmap = nullptr;
        if (SUCCEEDED(hr)) {
            FLOAT dpiX, dpiY;
            dpiX = (FLOAT)GetDpiForWindow(GetDesktopWindow());
            dpiY = dpiX;

            D2D1_BITMAP_PROPERTIES1 properties = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(
                    DXGI_FORMAT_B8G8R8A8_UNORM,
                    D2D1_ALPHA_MODE_IGNORE
                ),
                dpiX,
                dpiY
            );

            hr = myDirect2dContext->CreateBitmapFromDxgiSurface(
                surface.Get(),
                &properties,
                &bitmap
            );
        }
        if (SUCCEEDED(hr)) {
            myDirect2dContext->SetTarget(bitmap.Get());
        }

        if (SUCCEEDED(hr)) {
            hr = myDirect2dContext->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray), 
                &myLightSlateGrayBrush
            );
        }
        if (SUCCEEDED(hr)) {
            hr = myDirect2dContext->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue), 
                &myCornflowerBlueBrush
            );
        }
    }

    return hr;
}

HRESULT MyApp::CreateDeviceContext() {
    HRESULT hr = S_OK;

    D2D1_SIZE_U size = CalculateD2DWindowSize();

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
    };
    UINT countOfDriverTypes = ARRAYSIZE(driverTypes);

    DXGI_SWAP_CHAIN_DESC swapDescription;
    ZeroMemory(&swapDescription, sizeof(swapDescription));
    swapDescription.BufferDesc.Width = size.width;
    swapDescription.BufferDesc.Height = size.height;
    swapDescription.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapDescription.BufferDesc.RefreshRate.Numerator = 60;
    swapDescription.BufferDesc.RefreshRate.Denominator = 1;
    swapDescription.SampleDesc.Count = 1;
    swapDescription.SampleDesc.Quality = 0;
    swapDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDescription.BufferCount = 1;
    swapDescription.OutputWindow = myHwnd;
    swapDescription.Windowed = TRUE;

    ComPtr<ID3D11Device> d3dDevice;
    for (UINT driverTypeIndex = 0; driverTypeIndex < countOfDriverTypes; driverTypeIndex++) {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            driverTypes[driverTypeIndex],
            nullptr,
            createDeviceFlags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swapDescription,
            &mySwapChain,
            &d3dDevice,
            nullptr,
            nullptr
        );

        if (SUCCEEDED(hr)) {
            break;
        }
    }

    ComPtr<IDXGIDevice> dxgiDevice;
    if (SUCCEEDED(hr)) {
        hr = d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    }

    if (SUCCEEDED(hr)) {
        hr = myDirect2dFactory->CreateDevice(
            dxgiDevice.Get(),
            &myDirect2dDevice
        );
    }
    if (SUCCEEDED(hr)) {
        hr = myDirect2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &myDirect2dContext
        );
    }

    return hr;
}

HRESULT MyApp::LoadBitmapFromFile(PCWSTR uri, ID2D1Bitmap** ppBitmap) {
    ComPtr<IWICBitmapDecoder> pDecoder;
    ComPtr<IWICBitmapFrameDecode> pSource;
    ComPtr<IWICStream> pStream;
    ComPtr<IWICFormatConverter> pConverter;
    ComPtr<IWICBitmapScaler> pScaler;

    if (!myWICFactory) CreateDeviceIndependentResources();
    if (!myDirect2dContext) CreateDeviceContext();

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
            pSource.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
        );
    }
    if (SUCCEEDED(hr)) {
        hr = myDirect2dContext->CreateBitmapFromWicBitmap(
            pConverter.Get(),
            NULL,
            ppBitmap
        );
    }

    return hr;
}

HRESULT MyApp::LoadBitmapFromFile2(PCWSTR uri, MyBitmap* myBitmap) {
    ComPtr<IWICBitmapDecoder> pDecoder;
    std::vector<ComPtr<ID2D1Bitmap>> bitmapArr;

    if (!myWICFactory) CreateDeviceIndependentResources();
    if (!myDirect2dContext) CreateDeviceContext();

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
            ComPtr<IWICFormatConverter> pConverter;
            ComPtr<IWICBitmapFrameDecode> tmpSource;
            ComPtr<ID2D1Bitmap> tmpBitmap;
            if (SUCCEEDED(hr)) {
                hr = myWICFactory->CreateFormatConverter(&pConverter);
            }
            if (SUCCEEDED(hr)) {
                hr = pDecoder->GetFrame(i, &tmpSource);
            }
            if (SUCCEEDED(hr)) {
                hr = pConverter->Initialize(
                    tmpSource.Get(),
                    GUID_WICPixelFormat32bppPBGRA,
                    WICBitmapDitherTypeNone,
                    NULL,
                    0.f,
                    WICBitmapPaletteTypeMedianCut
                );
            }
            if (SUCCEEDED(hr)) {
                hr = myDirect2dContext->CreateBitmapFromWicBitmap(
                    pConverter.Get(),
                    NULL,
                    &tmpBitmap
                );
            }
            if (SUCCEEDED(hr)) {
                bitmapArr.push_back(std::move(tmpBitmap));
            }
        }
    }
    if (myBitmap) {
        myBitmap->Initialize(frameCount, bitmapArr);
    }


    return hr;
}

void MyApp::DiscardDeviceResources() {
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
    LoadBitmapFromFile2(L"loading.gif", mySequenceBitmap.get());
    LoadBitmapFromFile2(L"snail.png", myCharacterBitmap.get());
    myCharacterBitmap->SetScale(0.5f, 0.5f);

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
    if (myDirect2dContext) {
        HRESULT hr = S_OK;

        myDirect2dContext->SetTarget(nullptr);

        if (SUCCEEDED(hr)) {
            hr = mySwapChain->ResizeBuffers(
                0,
                width,
                height,
                DXGI_FORMAT_B8G8R8A8_UNORM,
                0
            );
        }

        ComPtr<IDXGISurface> surface = nullptr;
        if (SUCCEEDED(hr)) {
            hr = mySwapChain->GetBuffer(
                0,
                IID_PPV_ARGS(&surface)
            );
        }

        ComPtr<ID2D1Bitmap1> bitmap = nullptr;
        if (SUCCEEDED(hr)) {
            FLOAT dpiX, dpiY;
            dpiX = (FLOAT)GetDpiForWindow(GetDesktopWindow());
            dpiY = dpiX;
            D2D1_BITMAP_PROPERTIES1 properties = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(
                    DXGI_FORMAT_B8G8R8A8_UNORM,
                    D2D1_ALPHA_MODE_IGNORE
                ),
                dpiX,
                dpiY
            );
            hr = myDirect2dContext->CreateBitmapFromDxgiSurface(
                surface.Get(),
                &properties,
                &bitmap
            );
        }

        if (SUCCEEDED(hr)) {
            myDirect2dContext->SetTarget(bitmap.Get());
        }

        InvalidateRect(myHwnd, nullptr, FALSE);
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

    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        inputFlag[VK_LEFT] = true;
        MoveDirection[0] = -1;
        if (MoveDirection[2] != 0) {
            isLeft = false;
        } else {
            isLeft = true;
        }
        std::cout << "Input VK_LEFT\n";
    } else {
        if (inputFlag[VK_LEFT]) {
            inputFlag[VK_LEFT] = false;
            MoveDirection[0] = 0;
            std::cout << "Release VK_LEFT\n";
        }
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        inputFlag[VK_UP] = true;
        MoveDirection[1] = -1;
        std::cout << "Input VK_UP\n";
    } else {
        if (inputFlag[VK_UP]) {
            inputFlag[VK_UP] = false;
            MoveDirection[1] = 0;
            std::cout << "Release VK_UP\n";
        }
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        inputFlag[VK_RIGHT] = true;
        MoveDirection[2] = 1;
        if (MoveDirection[0] != 0) {
            isLeft = true;
        } else {
            isLeft = false;
        }
        std::cout << "Input VK_RIGHT\n";
    } else {
        if (inputFlag[VK_RIGHT]) {
            inputFlag[VK_RIGHT] = false;
            MoveDirection[2] = 0;
            std::cout << "Release VK_RIGHT\n";
        }
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        inputFlag[VK_DOWN] = true;
        MoveDirection[3] = 1;
        std::cout << "Input VK_DOWN\n";
    } else {
        if (inputFlag[VK_DOWN]) {
            inputFlag[VK_DOWN] = false;
            MoveDirection[3] = 0;
            std::cout << "Release VK_DOWN\n";
        }
    }
}

D2D1_SIZE_U MyApp::CalculateD2DWindowSize() {
    RECT rc;
    GetClientRect(myHwnd, &rc);

    D2D1_SIZE_U d2dWindowSize = { 0 };
    d2dWindowSize.width = rc.right;
    d2dWindowSize.height = rc.bottom;

    return d2dWindowSize;
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
        myCharacterBitmap->Tick(deltaTime);

        myDirect2dContext->BeginDraw();
        myDirect2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
        myDirect2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
        
        D2D1_SIZE_F rtSize = myDirect2dContext->GetSize();

        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);

        if (myBitmap) {
            myDirect2dContext->DrawBitmap(myBitmap.Get(),
                D2D1::RectF(
                    0.0f, 0.0f,
                    rtSize.width, rtSize.height
                ));
        }

        mySequenceBitmap->Move(deltaTime * 10, deltaTime * 10);
        if (mySequenceBitmap) {
            ComPtr<ID2D1Bitmap> tmp = mySequenceBitmap->GetBitmap();
            if (tmp) {
                myDirect2dContext->DrawImage(tmp.Get(), 
                    mySequenceBitmap->GetBitmapPosition());
            }
        }

        myCharacterBitmap->Move(
            deltaTime * MoveSpeed * (MoveDirection[0] + MoveDirection[2]),
            deltaTime * MoveSpeed * (MoveDirection[1] + MoveDirection[3]));
        if (myCharacterBitmap) {
            ComPtr<ID2D1Bitmap> tmp = myCharacterBitmap->GetBitmap();
            if (tmp) {
                ComPtr<ID2D1Effect> affineTransformEffect;
                ComPtr<ID2D1Effect> colorMatrixEffect;
                ComPtr<ID2D1Effect> colorMatrixEffect2;
                ComPtr<ID2D1Effect> edgeDetectionEffect;
                ComPtr<ID2D1Effect> edgeDetectionEffect2;
                ComPtr<ID2D1Effect> edgeDetectionEffect3;
                myDirect2dContext->CreateEffect(
                    CLSID_D2D12DAffineTransform,
                    &affineTransformEffect
                );
                myDirect2dContext->CreateEffect(
                    CLSID_D2D1ColorMatrix,
                    &colorMatrixEffect
                );
                myDirect2dContext->CreateEffect(
                    CLSID_D2D1ColorMatrix,
                    &colorMatrixEffect2
                );
                myDirect2dContext->CreateEffect(
                    CLSID_D2D1EdgeDetection,
                    &edgeDetectionEffect
                );
                myDirect2dContext->CreateEffect(
                    CLSID_D2D1EdgeDetection,
                    &edgeDetectionEffect2
                );
                myDirect2dContext->CreateEffect(
                    CLSID_D2D1EdgeDetection,
                    &edgeDetectionEffect3
                );
                affineTransformEffect->SetInput(0, tmp.Get());
                colorMatrixEffect->SetInputEffect(0, affineTransformEffect.Get());
                colorMatrixEffect2->SetInputEffect(0, colorMatrixEffect.Get());

                edgeDetectionEffect->SetInputEffect(0, affineTransformEffect.Get());
                edgeDetectionEffect2->SetInputEffect(0, colorMatrixEffect.Get());
                edgeDetectionEffect3->SetInputEffect(0, colorMatrixEffect2.Get());

                auto size = tmp->GetPixelSize();
                D2D1_POINT_2F ps = myCharacterBitmap->GetBitmapPosition();
                //D2D1_POINT_2F center = D2D1::Point2F(ps.right - ((ps.right - ps.left) / 2), ps.bottom - ((ps.bottom - ps.top) / 2));

                // Affine transform
                affineTransformEffect->SetValue(                 
                    D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, 
                    D2D1::Matrix3x2F::Scale(D2D1::SizeF((isLeft ? -1 : 1)*0.2f, 0.2f), D2D1::Point2F(size.width/2, size.height/2))
                );

                // Color matrix 1
                D2D1_MATRIX_5X4_F matrix = D2D1::Matrix5x4F(
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                    0, 0, 0, 100, 
                    0, 1, 0, 0);
                colorMatrixEffect->SetValue(
                    D2D1_COLORMATRIX_PROP_COLOR_MATRIX,
                    matrix
                );
                colorMatrixEffect->SetValue(
                    D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT,
                    TRUE
                );

                // Color matrix 2
                D2D1_MATRIX_5X4_F matrix2 = D2D1::Matrix5x4F(
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 0.5,
                    0, 0, 0, 0);
                colorMatrixEffect2->SetValue(
                    D2D1_COLORMATRIX_PROP_COLOR_MATRIX,
                    matrix2
                );

                //myDirect2dContext->SetTransform(D2D1::Matrix3x2F::Scale((isLeft ? -1 : 1), 1.f, center));
                /*myDirect2dContext->DrawBitmap(tmp.Get(),
                    ps
                );*/
                myDirect2dContext->DrawImage(affineTransformEffect.Get(), D2D1::Point2F(ps.x - 175.f, ps.y - 200.f));
                myDirect2dContext->DrawImage(colorMatrixEffect.Get(), D2D1::Point2F(ps.x - 175.f, ps.y - 70.f));
                myDirect2dContext->DrawImage(colorMatrixEffect2.Get(), D2D1::Point2F(ps.x - 175.f, ps.y + 60.f));

                myDirect2dContext->DrawImage(edgeDetectionEffect.Get(), D2D1::Point2F(ps.x - 25.f, ps.y - 200.f));
                myDirect2dContext->DrawImage(edgeDetectionEffect2.Get(), D2D1::Point2F(ps.x - 25.f, ps.y - 70.f));
                myDirect2dContext->DrawImage(edgeDetectionEffect3.Get(), D2D1::Point2F(ps.x - 25.f, ps.y + 60.f));

                //myDirect2dContext->SetTransform(D2D1::Matrix3x2F::Scale(1.f, 1.f));
            }
        }

        for (int x = 0; x < width; x += 10) {
            myDirect2dContext->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                myLightSlateGrayBrush.Get(), 0.5f);
        }
        for (int y = 0; y < height; y += 10) {
            myDirect2dContext->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                myLightSlateGrayBrush.Get(), 0.5f);
        }

        D2D1_RECT_F rectangle1 = D2D1::RectF(
            rtSize.width / 2 - 50.0f, rtSize.height / 2 - 50.0f,
            rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f
        );
        D2D1_RECT_F rectangle2 = D2D1::RectF(
            rtSize.width / 2 - 100.0f, rtSize.height / 2 - 100.0f,
            rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f
        );
        //myDirect2dContext->FillRectangle(&rectangle1, myLightSlateGrayBrush.Get());
        //myDirect2dContext->DrawRectangle(&rectangle2, myCornflowerBlueBrush.Get());

        hr = myDirect2dContext->EndDraw();
    }

    if (SUCCEEDED(hr)) {
        hr = mySwapChain->Present(0, 0);
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
        CoUninitialize();
    }

    return 0;
}