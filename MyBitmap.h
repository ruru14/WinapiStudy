#pragma once

#include <vector>

// DirectX headers
#include <d2d1.h>

class MyBitmap {

public:
	void Tick(FLOAT deltaTime);
	ID2D1Bitmap* GetBitmap();
	void Initialize(UINT frameCount, std::vector<ID2D1Bitmap*> bitmapArr);
	D2D1_RECT_F GetBitmapPosition();
	void SetPosition(FLOAT x, FLOAT y);
	void Move(FLOAT x, FLOAT y);

public:
	std::vector<ID2D1Bitmap*> bitmap;
	std::vector<D2D1_SIZE_F> bitmapSize;
	D2D1_SIZE_F currentPosition;
	UINT frameCount = -1;
	UINT currentFrame;
	FLOAT elapseTime = 0.f;
};