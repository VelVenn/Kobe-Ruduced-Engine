#pragma once

#include <graphics.h>
#include <conio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <windows.h>
#include <unordered_map>
#include "tstring.h"
#include "Animation.h"

using namespace std;

class Event
{
public:
	Event(ExMessage* msg)
	{
		this->msg = msg;
	}

	Event() = default;

	virtual ~Event() = default;

public:
	void handleKeyBoardEvent()
	{
		if (msg == nullptr)
		{
			return;
		}

		switch (msg->message)
		{
		case WM_KEYDOWN:
			keyState[msg->vkcode] = true;
			break;
		case WM_KEYUP:
			keyState[msg->vkcode] = false;
			break;
		default:
			break;
		}
	}

	void handleMouseEvent()
	{
		if (msg == nullptr)
		{
			return;
		}

		switch (msg->message)
		{
		case WM_MOUSEMOVE:
			cursorPos.x = msg->x;
			cursorPos.y = msg->y;
			break;
		case WM_LBUTTONDOWN:
			keyState[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			keyState[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			keyState[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			keyState[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			keyState[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			keyState[VK_MBUTTON] = false;
			break;
		default:
			break;
		}
	}

public:
	POINT getCursorPos() const
	{
		return cursorPos;
	}

	tstring getPressedKey()
	{
		for (int i = 0; i < keyState.size(); i++)
		{
			if (keyState[i])
			{
				return getKeyName(i);
			}
		}
		return _T(""); // 使用 _T 宏来支持 tstring
	}

protected:
	ExMessage* msg = nullptr;

protected:
	POINT cursorPos = { 0,0 };

protected:
	vector<bool> keyState = vector<bool>(256, false); // true: pressed, false: unpressed

private:
	tstring getKeyName(int vkcode)
	{
		switch (vkcode)
		{
		case VK_LEFT: return _T("Left Arrow");
		case VK_RIGHT: return _T("Right Arrow");
		case VK_UP: return _T("Up Arrow");
		case VK_DOWN: return _T("Down Arrow");
		case VK_SPACE: return _T("Space");
		case VK_RETURN: return _T("Enter");
		case VK_LBUTTON: return _T("Left Mouse");
		case VK_RBUTTON: return _T("Right Mouse");
		case VK_MBUTTON: return _T("Middle Mouse");
		case VK_CONTROL: return _T("Ctrl");
		case VK_SHIFT: return _T("Shift");
		case VK_MENU: return _T("Alt");
		case VK_ESCAPE: return _T("Esc");
		default:
			if (vkcode >= 'A' && vkcode <= 'Z' || vkcode >= '0' && vkcode <= '9')
				return tstring(1, TCHAR(vkcode)); // 将键盘字母转为 TCHAR
			else
				return _T("Unknown");
		}
	}
};