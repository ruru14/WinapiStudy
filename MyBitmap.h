#pragma once

#include <vector>

// DirectX headers
#include <d2d1.h>

class MyBitmap {

public:
	void Tick(FLOAT deltaTime);
	ID2D1Bitmap* GetBitmap();
	void Initialize(UINT frameCount, std::vector<ID2D1Bitmap*> bitmapArr);

public:
	std::vector<ID2D1Bitmap*> bitmap;
	UINT frameCount = -1;
	FLOAT elapseTime = 0.f;
};