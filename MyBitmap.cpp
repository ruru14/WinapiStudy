#include "MyBitmap.h"

void MyBitmap::Tick(FLOAT deltaTime) {
	elapseTime += deltaTime;
}

ID2D1Bitmap* MyBitmap::GetBitmap() {
	if (bitmap.size() > 0) {
		return bitmap[(UINT)elapseTime%bitmap.size()];
	} else {
		return nullptr;
	}
}

void MyBitmap::Initialize(UINT frameCount, std::vector<ID2D1Bitmap*> bitmapArr) {
	this->frameCount = frameCount;
	bitmap = bitmapArr;
}
