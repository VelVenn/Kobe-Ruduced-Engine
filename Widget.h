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


class Widget
{
public:
	Widget() = default;

	Widget(ExMessage* msg)
	{
		this->msg = msg;
	}

	virtual ~Widget() = default;

	void getPressEvent()
	{
		if (msg == nullptr)
			return;

		if (msg->message == WM_KEYDOWN)
		{
			switch (msg->vkcode)
			{
			case VK_UP:
				keyState[keyPress::UP] = true;
				break;
			case VK_DOWN:
				keyState[keyPress::DOWN] = true;
				break;
			case VK_LEFT:
				keyState[keyPress::LEFT] = true;
				break;
			case VK_RIGHT:
				keyState[keyPress::RIGHT] = true;
				break;
			case VK_SPACE:
				keyState[keyPress::SPACE] = true;
				break;
			case VK_RETURN:
				keyState[keyPress::ENTER] = true;
				break;
			case VK_ESCAPE:
				keyState[keyPress::ESC] = true;
				break;
			case 'W':
				keyState[keyPress::W] = true;
				break;
			case 'A':
				keyState[keyPress::A] = true;
				break;
			case 'S':
				keyState[keyPress::S] = true;
				break;
			case 'D':
				keyState[keyPress::D] = true;
				break;
			case VK_SHIFT:
				keyState[keyPress::SHIFT] = true;
				break;
			case VK_CONTROL:
				keyState[keyPress::CTRL] = true;
				break;
			case VK_MENU:
				keyState[keyPress::ALT] = true;
				break;
			case '1':
				keyState[keyPress::ONE] = true;
				break;
			case '2':
				keyState[keyPress::TWO] = true;
				break;
			case '3':
				keyState[keyPress::THREE] = true;
				break;
			case '4':
				keyState[keyPress::FOUR] = true;
				break;
			case '5':
				keyState[keyPress::FIVE] = true;
				break;
			case '6':
				keyState[keyPress::SIX] = true;
				break;
			case '7':
				keyState[keyPress::SEVEN] = true;
				break;
			case '8':
				keyState[keyPress::EIGHT] = true;
				break;
			case '9':
				keyState[keyPress::NINE] = true;
				break;
			case '0':
				keyState[keyPress::ZERO] = true;
				break;
			case VK_LBUTTON:
				keyState[keyPress::MLK] = true;
				break;
			case VK_RBUTTON:
				keyState[keyPress::MRK] = true;
				break;
			case VK_MBUTTON:
				keyState[keyPress::MMK] = true;
				break;
			default:
				break;
			}
		}
		else if (msg->message == WM_KEYUP)
		{
			switch (msg->vkcode)
			{
			case VK_UP:
				keyState[keyPress::UP] = false;
				break;
			case VK_DOWN:
				keyState[keyPress::DOWN] = false;
				break;
			case VK_LEFT:
				keyState[keyPress::LEFT] = false;
				break;
			case VK_RIGHT:
				keyState[keyPress::RIGHT] = false;
				break;
			case VK_SPACE:
				keyState[keyPress::SPACE] = false;
				break;
			case VK_RETURN:
				keyState[keyPress::ENTER] = false;
				break;
			case VK_ESCAPE:
				keyState[keyPress::ESC] = false;
				break;
			case 'W':
				keyState[keyPress::W] = false;
				break;
			case 'A':
				keyState[keyPress::A] = false;
				break;
			case 'S':
				keyState[keyPress::S] = false;
				break;
			case 'D':
				keyState[keyPress::D] = false;
				break;
			case VK_SHIFT:
				keyState[keyPress::SHIFT] = false;
				break;
			case VK_CONTROL:
				keyState[keyPress::CTRL] = false;
				break;
			case VK_MENU:
				keyState[keyPress::ALT] = false;
				break;
			case '1':
				keyState[keyPress::ONE] = false;
				break;
			case '2':
				keyState[keyPress::TWO] = false;
				break;
			case '3':
				keyState[keyPress::THREE] = false;
				break;
			case '4':
				keyState[keyPress::FOUR] = false;
				break;
			case '5':
				keyState[keyPress::FIVE] = false;
				break;
			case '6':
				keyState[keyPress::SIX] = false;
				break;
			case '7':
				keyState[keyPress::SEVEN] = false;
				break;
			case '8':
				keyState[keyPress::EIGHT] = false;
				break;
			case '9':
				keyState[keyPress::NINE] = false;
				break;
			case '0':
				keyState[keyPress::ZERO] = false;
				break;
			case VK_LBUTTON:
				keyState[keyPress::MLK] = false;
				break;
			case VK_RBUTTON:
				keyState[keyPress::MRK] = false;
				break;
			case VK_MBUTTON:
				keyState[keyPress::MMK] = false;
				break;
			default:
				break;
			}
		}
	}

	void getMouseMoveEvent()
	{
		if (msg == nullptr)
			return;

		if (msg->message == WM_MOUSEMOVE)
		{
			mousePos.x = msg->x;
			mousePos.y = msg->y;
		}
	}

protected:
	struct coord
	{
		float x = 0.0f;
		float y = 0.0f;
	}mousePos;

protected:
	ExMessage* msg = nullptr;

	enum class keyPress
	{
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		SPACE,
		ENTER,
		ESC,
		W,
		A,
		S,
		D,
		SHIFT,
		CTRL,
		ALT,
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		ZERO,
		MLK, // Mouse Left Key
		MRK, // Mouse Right Key
		MMK, // Mouse Middle Key
		NO_KEY
	};

	unordered_map<keyPress, bool> keyState; // ¼üÅÌ×´Ì¬¹þÏ£±í
};

class Button :Widget
{
public:
	Button() = default;
	Button(ExMessage* msg) :Widget(msg) {};
	Button
	(
		ExMessage* msg, IMAGE* unpressedImg, IMAGE* pressedImg, IMAGE* disabledImg
	)
		:Widget(msg), unpressedImg(unpressedImg), pressedImg(pressedImg), disabledImg(disabledImg) {};

	~Button() override
	{
		if (unpressedImg != nullptr)
		{
			delete unpressedImg;
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

	Button& setButtonPos(int x, int y)
	{
		buttonPos.x = x;
		buttonPos.y = y;
		return *this;
	}

	Button& setButtonSize(int w, int h)
	{
		buttonSize.w = w;
		buttonSize.h = h;
		return *this;
	}

	Button& setUnpressedImg(IMAGE* img)
	{
		unpressedImg = img;
		return *this;
	}

	Button& setPressedImg(IMAGE* img)
	{
		pressedImg = img;
		return *this;
	}

	Button& setDisabledImg(IMAGE* img)
	{
		disabledImg = img;
		return *this;
	}

	void drawButton()
	{
		if (unpressedImg == nullptr || pressedImg == nullptr || disabledImg == nullptr)
		{
			return;
		}

		if (!isActivated)
		{
			if (disabledImg != nullptr)
			{
				putImage_regBack(buttonPos.x, buttonPos.y, disabledImg);
			}
			return;
		}

		if (keyState[keyPress::MLK] && mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.w
			&& mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.h)
		{
			if (pressedImg != nullptr)
			{
				putImage_regBack(buttonPos.x, buttonPos.y, pressedImg);
			}
		}
		else
		{
			putImage_regBack(buttonPos.x, buttonPos.y, unpressedImg);
		}
	}

private:
	IMAGE* unpressedImg = nullptr; // Î´°´ÏÂÊ±Í¼Æ¬
	IMAGE* pressedImg = nullptr; // °´ÏÂÊ±Í¼Æ¬
	IMAGE* disabledImg = nullptr; // ½ûÓÃÊ±Í¼Æ¬

private:
	bool isActivated = true;

private:
	POINT buttonPos = { 0, 0 };

	struct size
	{
		int w = 0;
		int h = 0;
	}buttonSize;
};