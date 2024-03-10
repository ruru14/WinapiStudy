#include "MyBitmap.h"

void MyBitmap::Tick(FLOAT deltaTime) {
	elapseTime += deltaTime;
}

ID2D1Bitmap* MyBitmap::GetBitmap() {
	if (bitmap.size() > 0) {
		currentFrame = (UINT)elapseTime % bitmap.size();
		return bitmap[currentFrame];
	} else {
		return nullptr;
	}
}

void MyBitmap::Initialize(UINT frameCount, std::vector<ID2D1Bitmap*> bitmapArr) {
	this->frameCount = frameCount;
	currentFrame = 0;
	bitmap = bitmapArr;
	for (auto& i : bitmap) {
		bitmapSize.push_back(i->GetSize());
	}
	currentPosition = D2D1::SizeF(0.f, 0.f);
}

D2D1_RECT_F MyBitmap::GetBitmapPosition() {
	return D2D1::RectF(
		0 + currentPosition.width, 0 + currentPosition.height,
		bitmapSize[currentFrame].width + currentPosition.width, bitmapSize[currentFrame].height + currentPosition.height
	);
}

void MyBitmap::SetPosition(FLOAT x, FLOAT y) {
	currentPosition.width = x;
	currentPosition.height = y;
}

void MyBitmap::Move(FLOAT x, FLOAT y) {
	currentPosition.width += x;
	currentPosition.height += y;
}
