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

class Event // 事件类
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

	tstring getPressedKey() const
	{
		for (int i = 0; i < keyState.size(); i++)
		{
			if (keyState[i])
			{
				return getKeyName(i);
			}
		}
		return _T("");
	}

	bool isKeyPressed(int vkcode) const
	{
		return keyState[vkcode];
	}

protected:
	ExMessage* msg = nullptr;

protected:
	POINT cursorPos = { 0,0 };

protected:
	vector<bool> keyState = vector<bool>(256, false); // true: pressed, false: unpressed

private:
	tstring getKeyName(int vkcode) const
		// 获取按键名称
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
				return tstring(1, TCHAR(vkcode));
			else
				return _T("Unknown");
		}
	}
};

class Widget // 控件类
{
public:
	struct Size
	{
		int w = 0;
		int h = 0;
	};

public:
	Widget(POINT pos, Size size, Event* event) : widgetPos(pos), widgetSize(size), event(event)
	{
		rigion.left = pos.x;
		rigion.top = pos.y;
		rigion.right = pos.x + size.w;
		rigion.bottom = pos.y + size.h;
	}

	Widget(Size size, Event* event) : widgetSize(size), event(event) {}

	Widget() = default;

	virtual ~Widget() = default;
public:

	Widget& setRegion(POINT pos = { INT_MAX,INT_MAX }, Size size = { 0,0 })
	{
		if (pos.x != INT_MAX && pos.y != INT_MAX)
		{
			widgetPos = pos;
		}
		if (size.w != 0 && size.h != 0)
		{
			widgetSize = size;
		}

		rigion.left = widgetPos.x;
		rigion.top = widgetPos.y;
		rigion.right = widgetPos.x + widgetSize.w;
		rigion.bottom = widgetPos.y + widgetSize.h;

		return *this;
	}

	Widget& setPos(POINT pos)
	{
		widgetPos = pos;
		rigion.left = pos.x;
		rigion.top = pos.y;
		return *this;
	}

	Widget& setSize(Size size)
	{
		widgetSize = size;
		rigion.right = widgetPos.x + size.w;
		rigion.bottom = widgetPos.y + size.h;
		return *this;
	}

	Widget& setEvent(Event* event)
	{
		this->event = event;
		return *this;
	}

public:
	virtual void draw() = 0;

	virtual void updateStatus() = 0;

public:
	POINT getPos() const
	{
		return widgetPos;
	}

	Size getSize() const
	{
		return widgetSize;
	}

	RECT getRigion() const
	{
		return rigion;
	}

protected:
	POINT widgetPos = { 0,0 };
	Size widgetSize = { 0,0 };

	RECT rigion = { 0,0,0,0 };

protected:
	Event* event = nullptr;
};

class Button : public Widget
{
public:
	Button(POINT pos, Size size, Event* event) : Widget(pos, size, event) {}

	Button(Size size, Event* event) : Widget(size, event) {}

	Button() = default;

	~Button() override
	{
		if (idleImg != nullptr)
		{
			delete idleImg;
		}
		if (hoverImg != nullptr)
		{
			delete hoverImg;
		}
		if (pressedImg != nullptr)
		{
			delete pressedImg;
		}
		if (disabledImg != nullptr)
		{
			delete disabledImg;
		}
	}

public:
	Button& setIdleImg(tstring path)
	{
		idleImg = new IMAGE;
		loadimage(idleImg, path.c_str(), widgetSize.w, widgetSize.h);
		return *this;
	}

	Button& setHoverImg(tstring path)
	{
		hoverImg = new IMAGE;
		loadimage(hoverImg, path.c_str(), widgetSize.w, widgetSize.h);
		return *this;
	}

	Button& setPressedImg(tstring path)
	{
		pressedImg = new IMAGE;
		loadimage(pressedImg, path.c_str(), widgetSize.w, widgetSize.h);
		return *this;
	}

	Button& setDisabledImg(tstring path)
	{
		disabledImg = new IMAGE;
		loadimage(disabledImg, path.c_str(), widgetSize.w, widgetSize.h);
		return *this;
	}

public:
	void draw() override
	{
		switch (state)
		{
		case ButtonState::Idle:
			putImage_regBack(widgetPos.x, widgetPos.y, idleImg);
			break;
		case ButtonState::Hover:
			putImage_regBack(widgetPos.x, widgetPos.y, hoverImg);
			break;
		case ButtonState::Pressed:
			putImage_regBack(widgetPos.x, widgetPos.y, pressedImg);
			break;
		case ButtonState::Disabled:
			putImage_regBack(widgetPos.x, widgetPos.y, disabledImg);
			break;
		}
	}

	void updateStatus() override
		// 更新按钮状态
	{
		if (state == ButtonState::Disabled)
		{
			return;
		}

		if (isCursorInRigion()) // 鼠标在按钮区域内
		{
			if (event->isKeyPressed(VK_LBUTTON))
			{
				state = ButtonState::Pressed;
			}
			else if (state == ButtonState::Pressed)
			{
				OnClick();
				state = ButtonState::Hover;
			}
			else
			{
				state = ButtonState::Hover;
			}
		}
		else
		{
			state = ButtonState::Idle;
		}
	}

protected:
	virtual void OnClick()
	{
		cout << "Button has been clicked!" << endl;
	}

protected:
	enum class ButtonState
	{
		Idle = 0,
		Hover,
		Pressed,
		Disabled
	};

	ButtonState state = ButtonState::Idle;

protected:
	IMAGE* idleImg = nullptr;
	IMAGE* hoverImg = nullptr;
	IMAGE* pressedImg = nullptr;
	IMAGE* disabledImg = nullptr;

private:
	bool isCursorInRigion()
	{
		POINT cursor = event->getCursorPos();
		return 
			cursor.x >= rigion.left && cursor.x <= rigion.right && 
			cursor.y >= rigion.top && cursor.y <= rigion.bottom;
	}
};