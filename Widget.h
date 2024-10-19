#pragma once

#include "Animation.h"
#include "tstring.h"
#include <conio.h>
#include <graphics.h>
#include <string>
#include <tchar.h>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <chrono>

using namespace std;

#define DEBUG

class Event // �¼���
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
		// ��������¼�
	{
		if (msg == nullptr)
		{
			return;
		}

		switch (msg->message)
		{
		case WM_KEYDOWN:
			keyState[msg->vkcode] = true; // ��������
			break;
		case WM_KEYUP:
			keyState[msg->vkcode] = false; // �����ͷ�
			lastKeyUpTimes[msg->vkcode] = chrono::steady_clock::now(); // ��¼�����ͷ�ʱ��
			break;
		default:
			break;
		}
	}

	void handleMouseEvent()
		// ��������¼� 
	{
		if (msg == nullptr)
		{
			return;
		}

		switch (msg->message)
		{
		case WM_MOUSEMOVE: // ����ƶ�
			cursorPos.x = msg->x;
			cursorPos.y = msg->y;
			break;
		case WM_LBUTTONDOWN: // ����������
			keyState[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP: // �������ͷ�
			keyState[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONUP: // ����Ҽ��ͷ�
			keyState[VK_RBUTTON] = false;
			lastKeyUpTimes[VK_RBUTTON] = chrono::steady_clock::now();
			break;
		case WM_MBUTTONDOWN: // ����м�����
			keyState[VK_MBUTTON] = true;
			lastKeyUpTimes[VK_MBUTTON] = chrono::steady_clock::now();
			break;
		case WM_MBUTTONUP: // ����м��ͷ�
			keyState[VK_MBUTTON] = false;
			lastKeyUpTimes[VK_MBUTTON] = chrono::steady_clock::now();
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

	chrono::steady_clock::time_point getLastKeyUpTime(int vkcode) const
	{
		return lastKeyUpTimes[vkcode];
	}

protected:
	ExMessage* msg = nullptr; // �ⲿ�豸��Ϣ

protected:
	POINT cursorPos = { 0,0 }; // ���λ��

protected:
	vector<bool> keyState = vector<bool>(256, false); // true: pressed, false: unpressed

	vector<chrono::steady_clock::time_point> lastKeyUpTimes 
		= vector<chrono::steady_clock::time_point>(256, chrono::steady_clock::now()); // ��¼�����ͷ�ʱ��

private:
	tstring getKeyName(int vkcode) const
		// ��ȡ��������
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

class Widget // �ؼ���
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
		Region.left = pos.x;
		Region.top = pos.y;
		Region.right = pos.x + size.w;
		Region.bottom = pos.y + size.h;
	}

	Widget(Size size, Event* event) : widgetSize(size), event(event) {}

	Widget() = default;

	virtual ~Widget() = default;
public:

	Widget& setRegion(POINT pos = { INT_MAX,INT_MAX }, Size size = { 0,0 })
		// ���ÿؼ�����
	{
		if (pos.x != INT_MAX && pos.y != INT_MAX)
		{
			widgetPos = pos;
		}
		if (size.w != 0 && size.h != 0)
		{
			widgetSize = size;
		}

		Region.left = widgetPos.x;
		Region.top = widgetPos.y;
		Region.right = widgetPos.x + widgetSize.w;
		Region.bottom = widgetPos.y + widgetSize.h;

		return *this;
	}

	Widget& setPos(POINT pos)
	{
		widgetPos = pos;
		Region.left = pos.x;
		Region.top = pos.y;
		return *this;
	}

	Widget& setSize(Size size)
	{
		widgetSize = size;
		Region.right = widgetPos.x + size.w;
		Region.bottom = widgetPos.y + size.h;
		return *this;
	}

	Widget& setEvent(Event* event)
		// �����¼�������
	{
		this->event = event;
		return *this;
	}

public:
	virtual void draw() = 0; // ���ƿؼ���������ʵ��

	virtual void updateStatus() {}; // ���¿ؼ�״̬��������ѡ����ʵ��

public:
	POINT getPos() const
	{
		return widgetPos;
	}

	Size getSize() const
	{
		return widgetSize;
	}

	RECT getRegion() const
	{
		return Region;
	}

protected:
	POINT widgetPos = { 0,0 };
	Size widgetSize = { 0,0 };

	RECT Region = { 0,0,0,0 }; // �ؼ�����

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
		LOG("Button's resources have been released");
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
		// ���°�ť״̬
	{
		if (state == ButtonState::Disabled) // �����ť�����ã��򲻸���״̬
		{
			return;
		}

		if (isCursorInRegion()) // ����ڰ�ť������
		{
			if (event->isKeyPressed(VK_LBUTTON))
			{
				state = ButtonState::Pressed; // ������������£���ť״̬Ϊ����
			}
			else if (state == ButtonState::Pressed)
			{
				OnClick(); // ����������ͷţ��򴥷�����¼�
				state = ButtonState::Hover;
			}
			else
			{
				state = ButtonState::Hover; // �������ڰ�ť��������δ�����������ť״̬Ϊ��ͣ
			}
		}
		else
		{
			state = ButtonState::Idle; // �����겻�ڰ�ť�����ڣ���ť״̬ΪĬ��״̬
		}
	}

protected:
	virtual void OnClick() // ����¼�
	{
		cout << "Button has been clicked!" << endl;
	}

protected:
	enum class ButtonState
	{
		Idle = 0, // Ĭ��״̬
		Hover, // �����ͣ
		Pressed, // ��갴��
		Disabled // ����
	};

	ButtonState state = ButtonState::Idle;

protected:
	IMAGE* idleImg = nullptr;
	IMAGE* hoverImg = nullptr;
	IMAGE* pressedImg = nullptr;
	IMAGE* disabledImg = nullptr;

private:
	bool isCursorInRegion()
	{
		POINT cursor = event->getCursorPos();
		return
			cursor.x >= Region.left && cursor.x <= Region.right &&
			cursor.y >= Region.top && cursor.y <= Region.bottom;
	}
};

class startButton : public Button
{
public:
	startButton(POINT pos, Size size, Event* event, bool* isGameStart) :
		Button(pos, size, event), isGameStart(isGameStart) {}

	startButton(Size size, Event* event, bool* isGameStart) : 
		Button(size, event), isGameStart(isGameStart) {}

	startButton() = default;

	~startButton() override = default;

protected:
	void OnClick() override
	{
		*(isGameStart) = true;
	}

protected:
	bool* isGameStart = nullptr;
};

class exitButton : public Button
{
public:
	exitButton(POINT pos, Size size, Event* event, bool* isRunning) :
		Button(pos, size, event), isRunning(isRunning) {}

	exitButton(Size size, Event* event, bool* isRunning) :
		Button(size, event), isRunning(isRunning) {}

	exitButton() = default;

	~exitButton() override = default;

protected:
	void OnClick() override
	{
		*(isRunning) = false;
	}

protected:
	bool* isRunning = nullptr;
};


/*
class Textbox : public Widget
{
public:
	Textbox(POINT pos, Size size, Event* event) : Widget(pos, size, event) {}

	Textbox(Size size, Event* event) : Widget(size, event) {}

	Textbox() = default;

	~Textbox() override
	{
		if (background != nullptr)
		{
			delete background;
		}
	}

public:
	Textbox& setBackground(tstring path)
	{
		background = new IMAGE;
		loadimage(background, path.c_str());
		return *this;
	}

	Textbox& setText(tstring text)
	{
		this->text = text;
		return *this;
	}

public:
	void draw() override
	{
		RECT textRegion = getRegion();

		if (background != nullptr)
		{
			putImage_regBack(widgetPos.x, widgetPos.y, background);
		}
		drawtext(text.c_str(),&textRegion, DT_CENTER);
	}

protected:
	IMAGE* background = nullptr;

protected:
	tstring text = _T("");
};
*/