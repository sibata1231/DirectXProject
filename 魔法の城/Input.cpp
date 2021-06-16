#include "input.h"

const int Input::m_nMouseBtn[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

HRESULT Input::Init() {
	HRESULT hr = S_OK;
	hr = InitKeyboard();	// キーボードの初期化
	if (SUCCEEDED(hr)) {	// ゲームパッドの初期化
		hr = InitJoystick();
	}
	if (SUCCEEDED(hr)) {	// マウスの初期化
		hr = InitMouse();
	}
	return hr;
}

void Input::Uninit() {
	UninitMouse();	  // マウスの終了処理
	UninitJoystick(); // ゲームパッドの終了処理
	UninitKeyboard(); // キーボードの終了処理
}

void Input::Update() {
	UpdateKeyboard(); // キーボードの更新
	UpdateJoystick(); // ゲームパッドの更新
    UpdateMouse();    // マウスの更新
}

HRESULT Input::InitKeyboard() {
	HRESULT hr = S_OK;
	return hr;
}

void Input::UninitKeyboard() {
}

HRESULT Input::UpdateKeyboard(void) {
	HRESULT hr = S_OK;
	WORD wKeyState;
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++) {	// キーボードの状態を取得
		wKeyState = GetAsyncKeyState(nCntKey);
		m_wKeyStateTrigger[nCntKey] = (m_wKeyState[nCntKey] ^ wKeyState) & wKeyState;
		m_wKeyStateRelease[nCntKey] = (m_wKeyState[nCntKey] ^ wKeyState) & ~wKeyState;
		m_wKeyStateRepeat[nCntKey] = m_wKeyStateTrigger[nCntKey];
		if (wKeyState & 0x8000) {
			m_nKeyStateRepeatCnt[nCntKey]++;
			if (m_nKeyStateRepeatCnt[nCntKey] >= 20) {
				m_wKeyStateRepeat[nCntKey] = wKeyState;
			}
		} else {
			m_nKeyStateRepeatCnt[nCntKey] = 0;
			m_wKeyStateRepeat[nCntKey] = 0;
		}
		m_wKeyState[nCntKey] = wKeyState;
	}
	return hr;
}

HRESULT Input::InitJoystick() {
    THE_INPUT.GetJoyCount();
	return S_OK;
}

void Input::UninitJoystick() {

}

HRESULT Input::UpdateJoystick() {
	HRESULT hr = S_OK;

	for (DWORD i = 0; i < m_dwJoyCount; ++i) {
		JOYINFOEX& ji = m_joyState[i];
		DWORD dwButtons = ji.dwButtons;
		ZeroMemory(&ji, sizeof(ji));
		ji.dwSize = sizeof(ji);
		ji.dwFlags = JOY_RETURNALL;
		if (joyGetPosEx(i, &ji) != JOYERR_NOERROR) {
			hr = E_FAIL;
			break;
		}
		m_dwJoyButtonTrigger[i] = (m_joyState[i].dwButtons ^ dwButtons) & dwButtons;
		m_dwJoyButtonRelease[i] = (m_joyState[i].dwButtons ^ dwButtons) & ~dwButtons;
	}
	return hr;
}

HRESULT Input::InitMouse() {
	return S_OK;
}

void Input::UninitMouse() {
}

HRESULT Input::UpdateMouse() {
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(FindWindow(Window::ClassName,nullptr), &pt);
	m_ptMouse = pt;
	return S_OK;
}

bool Input::GetKeyPress(int nKey) {
	return (m_wKeyState[nKey] & 0x8000) ? true : false;
}

bool Input::GetKeyTrigger(int nKey) {
	return (m_wKeyStateTrigger[nKey] & 0x8000) ? true : false;
}

bool Input::GetKeyRepeat(int nKey) {
	return (m_wKeyStateRepeat[nKey] & 0x8000) ? true : false;
}

bool Input::GetKeyRelease(int nKey) {
	return (m_wKeyStateRelease[nKey] & 0x8000) ? true : false;
}

DWORD Input::GetJoyCount() {
	JOYINFO	ji;
	DWORD dwCount = joyGetNumDevs();
	if (dwCount > NUM_JOY_MAX)
		dwCount = NUM_JOY_MAX;
	for (m_dwJoyCount = 0; m_dwJoyCount < dwCount; ++m_dwJoyCount) {
		if (joyGetPos(m_dwJoyCount, &ji) != JOYERR_NOERROR)
			break;
	}
	return m_dwJoyCount;
}

JOYINFOEX *Input::GetJoyState(DWORD dwJoy) {
	if (dwJoy >= NUM_JOY_MAX) return nullptr;
	return &m_joyState[dwJoy];
}

LONG Input::GetJoyX(DWORD dwJoy) {
	if (dwJoy >= NUM_JOY_MAX) return 0L;
	return (LONG)m_joyState[dwJoy].dwXpos - 0x08000;
}

LONG Input::GetJoyY(DWORD dwJoy) {
	if (dwJoy >= NUM_JOY_MAX) return 0L;
	return (LONG)m_joyState[dwJoy].dwYpos - 0x08000;
}

LONG Input::GetJoyZ(DWORD dwJoy) {
	if (dwJoy >= NUM_JOY_MAX) return 0L;
	return (LONG)m_joyState[dwJoy].dwZpos - 0x08000;
}

bool Input::GetJoyButton(DWORD dwJoy, DWORD dwBtn) {
	if (dwJoy >= NUM_JOY_MAX) return false;
	return (m_joyState[dwJoy].dwButtons & (1 << dwBtn)) ? true : false;
}

bool Input::GetJoyTrigger(DWORD dwJoy, DWORD dwBtn) {
	if (dwJoy >= NUM_JOY_MAX) return false;
	return (m_dwJoyButtonTrigger[dwJoy] & (1 << dwBtn)) ? true : false;
}

bool Input::GetJoyRelease(DWORD dwJoy, DWORD dwBtn) {
	if (dwJoy >= NUM_JOY_MAX) return false;
	return (m_dwJoyButtonRelease[dwJoy] & (1 << dwBtn)) ? true : false;
}

POINT *Input::GetMousePosition() {
	return &m_ptMouse;
}

bool Input::GetMouseButton(DWORD dwBtn) {
	if (dwBtn >= _countof(m_nMouseBtn)) return false;
	return GetKeyPress(m_nMouseBtn[dwBtn]);
}

bool Input::GetMouseTrigger(DWORD dwBtn) {
	if (dwBtn >= _countof(m_nMouseBtn)) return false;
	return GetKeyTrigger(m_nMouseBtn[dwBtn]);
}

bool Input::GetMouseRelease(DWORD dwBtn) {
	if (dwBtn >= _countof(m_nMouseBtn)) return false;
	return GetKeyRelease(m_nMouseBtn[dwBtn]);
}
