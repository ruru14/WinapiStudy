# [Direct2D] 움직이는 비트맵 이미지 그리기
- 샘플 이미지 loading.gif 추가
- 시퀀스 이미지를 저장할 객체 MyBitmap 추가
  - 객체를 초기화하는 Initialize함수 추가
  - 객체의 프레임을 진행시킬 Tick함수 추가
  - 현재 프레임의 비트맵을 반환하는 GetBitmap함수 추가
  - 시퀀스 이미지를 저장하는 필드 std::vector<ID2D1Bitmap*> bitmap추가
  - 시퀀스 이미지의 프레임 수를 저장하는 필드 UINT frameCount추가
  - 프로그램의 경과 시간을 저장하는 필드 FLOAT elapseTime추가
- URI를 통해 시퀀스 이미지를 가져오고, MyBitmap객체를 초기화하는 LoadBitmapFromFile2함수 추가
- Initialize함수에 MyBitmap객체를 loading.gif로 초기화하는 코드 추가
- OnRender함수에 MyBitmap객체의 Tick을 진행시키고, 프레임 이미지를 렌더링하는 코드 추가
