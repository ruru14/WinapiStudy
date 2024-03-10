# [Direct2D] 비트맵 이미지 그리기
- https://milleatelier.tistory.com/151
- 샘플 이미지 (DX Logo)추가
- COM 초기화 (CoInitializeEx)
- LoadBitmapFromFile함수 초기화 및 구현
- 이미지 관련 다음 필드 추가
  - IWICImagingFactory* myWICFactory
  - ID2D1Bitmap* myBitmap
- 생성자 및 초기화 함수 편집
- OnRender함수 이미지 출력 추가
