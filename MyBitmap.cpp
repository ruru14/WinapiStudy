#include "MyBitmap.h"

void MyBitmap::Tick(FLOAT deltaTime) {
	elapseTime += deltaTime;
	if (bitmap.size() > 0) currentFrame = (UINT)elapseTime % bitmap.size();
}

ComPtr<ID2D1Bitmap> MyBitmap::GetBitmap() {
	if (bitmap.size() > 0) {
		return bitmap[currentFrame];
	} else {
		return nullptr;
	}
}

void MyBitmap::Initialize(UINT frameCount, std::vector<ComPtr<ID2D1Bitmap>> bitmapArr) {
	this->frameCount = frameCount;
	currentFrame = 0;
	bitmap = bitmapArr;
	for (auto& i : bitmap) {
		bitmapSize.push_back(i->GetSize());
		auto tmp = i->GetSize();
		bitmapRect.push_back(D2D1::RectF(0, 0, tmp.width, tmp.height));
	}
	auto tmp = bitmap[0]->GetPixelSize();
	currentPosition = D2D1::Point2F(tmp.width/2, tmp.height);
	//currentPosition = D2D1::Point2F(0.f, 0.f);
}

D2D1_POINT_2F MyBitmap::GetBitmapPosition() {
	return D2D1::Point2F(
		currentPosition.x,
		currentPosition.y
	);
}

D2D1_POINT_2F MyBitmap::GetBitmapDrawOffset() {
	auto tmp = bitmap[currentFrame]->GetPixelSize();
	return D2D1::Point2F(
		currentPosition.x - tmp.width / 2,
		currentPosition.y - tmp.height
	);
}

D2D1_RECT_F MyBitmap::GetBitmapRect() {
	if (bitmapRect.size() > currentFrame && bitmap.size() > currentFrame) {
		auto rect = bitmapRect[currentFrame];
		auto size = bitmap[currentFrame]->GetPixelSize();
		rect.left += currentPosition.x - size.width / 2;
		rect.right += currentPosition.x - size.width / 2;
		rect.top += currentPosition.y - size.height;
		rect.bottom += currentPosition.y - size.height;
		return rect;
	} else {
		return D2D1::RectF();
	}
}

void MyBitmap::SetPosition(FLOAT x, FLOAT y) {
	currentPosition.x = x;
	currentPosition.y = y;
}

void MyBitmap::Move(FLOAT x, FLOAT y) {
	currentPosition.x += x;
	currentPosition.y += y;
}

void MyBitmap::SetScale(FLOAT x, FLOAT y) {
	bitmapSacleX = x;
	bitmapSacleY = y;
}
