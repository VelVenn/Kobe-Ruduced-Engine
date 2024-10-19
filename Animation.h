#pragma once

#include "tstring.h"
#include <conio.h>
#include <graphics.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <windows.h>

using namespace std;

void putImage_regBack(int x, int y, IMAGE* img)
// 此函数可以识别图片的背景信息
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

void loadAnimeImage(IMAGE* img, tstring basePath, int frameCount)
{
	tstring curPath;
	for (int i = 0; i < frameCount; i++)
	{
		curPath = basePath + to_tstring(i) + _T(".png");
		loadimage(img + i, curPath.c_str());
	}
}

class Animation
{
public:
	Animation(tstring basePath, int frameAmount, int setGap)
		// basePath: 基础路径 frameAmount: 帧数量 setGap: 帧间隔
	{
		frameGap = setGap;
		frameLs.resize(frameAmount);

		lastFrTick = GetTickCount();

		tstring curPath;
		for (size_t i = 0; i < frameAmount; i++)
		{
			curPath = basePath + to_tstring(i) + _T(".png"); // 生成当前帧路径

			IMAGE* frame = new IMAGE(); // 为当前帧分配内存
			loadimage(frame, curPath.c_str()); // 加载当前帧
			frameLs[i] = frame; // 将当前帧加入帧列表
		}
	}

	~Animation()
	{
		for (size_t i = 0; i < frameLs.size(); i++)
		{
			delete frameLs[i];
		}
	}

	DWORD getLastFrTick() const
	{
		return lastFrTick;
	}

	void playFrame(int x, int y)
		// x: x坐标 y: y坐标
	{
		DWORD curFrTick = GetTickCount(); // 获取当前时间
		DWORD curInterval = curFrTick - lastFrTick; // 当前帧间隔

		timer += curInterval; // 计时器增加
		if (timer >= frameGap) // 判断是否达到指定的帧间隔
		{
			curFrame = (curFrame + 1) % frameLs.size(); // 播放下一帧
			timer = 0;
		}

		putImage_regBack(x, y, frameLs[curFrame]); // 显示当前帧
		lastFrTick = curFrTick;
	}

	void playFrame(float interval, int x, int y)
		// interval: 帧间隔 x: x坐标 y: y坐标
	{

		timer += interval; // 计时器增加
		if (timer >= frameGap) // 判断是否达到指定的帧间隔
		{
			curFrame = (curFrame + 1) % frameLs.size(); // 播放下一帧
			timer = 0;
		}

		putImage_regBack(x, y, frameLs[curFrame]); // 显示当前帧
	}

	void stop()
	{
		timer = 0;
		curFrame = 0;
	}

private:
	vector<IMAGE*> frameLs; // 帧列表
	int frameGap = 0; // 帧间隔
	int curFrame = 0; // 当前帧
	float timer = 0.0f; // 计时器
	DWORD lastFrTick; // 上一帧时间
};
