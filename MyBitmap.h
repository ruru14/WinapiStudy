#pragma once

#include <vector>

// DirectX headers
#include <d2d1.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class MyBitmap {

public:
	void Tick(FLOAT deltaTime);
	ComPtr<ID2D1Bitmap> GetBitmap();
	void Initialize(UINT frameCount, std::vector<ComPtr<ID2D1Bitmap>> bitmapArr);
	D2D1_POINT_2F GetBitmapPosition();
	D2D1_POINT_2F GetBitmapDrawOffset();
	D2D1_RECT_F GetBitmapRect();
	void SetPosition(FLOAT x, FLOAT y);
	void Move(FLOAT x, FLOAT y);
	void SetScale(FLOAT x, FLOAT y);

private:
	std::vector<ComPtr<ID2D1Bitmap>> bitmap;
	std::vector<D2D1_SIZE_F> bitmapSize;
	std::vector<D2D1_RECT_F> bitmapRect;
	D2D1_POINT_2F currentPosition;
	UINT frameCount = -1;
	UINT currentFrame;
	FLOAT elapseTime = 0.f;
	FLOAT bitmapSacleX = 1.f;
	FLOAT bitmapSacleY = 1.f;
};