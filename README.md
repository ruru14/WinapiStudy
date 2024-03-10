# [WinAPI] 키보드 입력 (WM_KEYDOWN, WM_KEYUP, GetAsyncKeyState)
- https://milleatelier.tistory.com/150
- 윈도우 프로시저에서 WM_KEYDOWN, WM_KEYUP메시지에 대한 로직 구현
- MyApp클래스에 비동기 입력을 담당할 HandleKeyboardInput함수 선언
- MyApp클래스에 비동기 입력 해제를 확인할 수 있는 map변수 inputFlag 선언
- GetAsyncKeyState를 사용한 비동기 입력 구현 함수 HandleKeyboardInput함수 구현
- 메시지 루프에서 HandleKeyboardInput함수 호출
