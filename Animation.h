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
// �˺�������ʶ��ͼƬ�ı�����Ϣ
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
		// basePath: ����·�� frameAmount: ֡���� setGap: ֡���
	{
		frameGap = setGap;
		frameLs.resize(frameAmount);

		lastFrTick = GetTickCount();

		tstring curPath;
		for (size_t i = 0; i < frameAmount; i++)
		{
			curPath = basePath + to_tstring(i) + _T(".png"); // ���ɵ�ǰ֡·��

			IMAGE* frame = new IMAGE(); // Ϊ��ǰ֡�����ڴ�
			loadimage(frame, curPath.c_str()); // ���ص�ǰ֡
			frameLs[i] = frame; // ����ǰ֡����֡�б�
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
		// x: x���� y: y����
	{
		DWORD curFrTick = GetTickCount(); // ��ȡ��ǰʱ��
		DWORD curInterval = curFrTick - lastFrTick; // ��ǰ֡���

		timer += curInterval; // ��ʱ������
		if (timer >= frameGap) // �ж��Ƿ�ﵽָ����֡���
		{
			curFrame = (curFrame + 1) % frameLs.size(); // ������һ֡
			timer = 0;
		}

		putImage_regBack(x, y, frameLs[curFrame]); // ��ʾ��ǰ֡
		lastFrTick = curFrTick;
	}

	void playFrame(float interval, int x, int y)
		// interval: ֡��� x: x���� y: y����
	{

		timer += interval; // ��ʱ������
		if (timer >= frameGap) // �ж��Ƿ�ﵽָ����֡���
		{
			curFrame = (curFrame + 1) % frameLs.size(); // ������һ֡
			timer = 0;
		}

		putImage_regBack(x, y, frameLs[curFrame]); // ��ʾ��ǰ֡
	}

	void stop()
	{
		timer = 0;
		curFrame = 0;
	}

private:
	vector<IMAGE*> frameLs; // ֡�б�
	int frameGap = 0; // ֡���
	int curFrame = 0; // ��ǰ֡
	float timer = 0.0f; // ��ʱ��
	DWORD lastFrTick; // ��һ֡ʱ��
};
