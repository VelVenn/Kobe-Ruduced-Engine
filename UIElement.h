#pragma once

#include "Charactor.h"
#include "tstring.h"
#include "Widget.h"
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <graphics.h>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

class gameInterface;
class startMenu;

class UIElement
{
public:
	UIElement() = default;

	virtual ~UIElement()
	{
		for (auto& child : uChildren)
		{
			delete child;
		}
		for (auto& child : wChildren)
		{
			delete child;
		}
	}

public:
	UIElement* addChild(UIElement* child) // �����UI����
	{
		if (child != nullptr)
		{
			child->setParent(this);
			uChildren.push_back(child);
			return this;
		}
	}

	UIElement* addChild(Widget* child) // ����ӿؼ�
	{
		wChildren.push_back(child);
		return this;
	}

	UIElement* removeChild(UIElement* child) // �Ƴ���UI����
	{
		auto aim = find(uChildren.begin(), uChildren.end(), child);
		if (aim != uChildren.end())
		{
			uChildren.erase(aim);
			child->parent = nullptr;
		}
		return this;
	}

	UIElement* removeChild(Widget* child) // �Ƴ��ӿؼ�
	{
		auto aim = find(wChildren.begin(), wChildren.end(), child);
		if (aim != wChildren.end())
		{
			wChildren.erase(aim);
		}
		return this;
	}

	void setParent(UIElement* parent) // ���ø�UI����
	{
		this->parent = parent;
	}

public:
	virtual void render() // ��ȾUI����
	{
		for (auto& child : wChildren)
		{
			child->draw();
		}
		for (auto& child : uChildren)
		{
			child->render();
		}
	}

	virtual void updateStatus() // ����UI����״̬
	{
		for (auto& child : wChildren)
		{
			child->updateStatus();
		}
		for (auto& child : uChildren)
		{
			child->updateStatus();
		}
	}

	virtual void monitorEvent() {} // �����¼�

public:
	UIElement* getParent() const // ��ȡ��UI����
	{
		return parent;
	}

protected:
	vector<UIElement*> uChildren; // ��UI�����б�
	vector<Widget*> wChildren; // �ӿؼ��б�

protected:
	UIElement* parent = nullptr; // ��UI����
};

class gameInterface : public UIElement // ��Ϸ����
{
public:
	gameInterface
	(
		Event* event, bool* isRunning,
		Player* player, vector<Bullet>* bulletList,
		vector<Enemy*>* enemyList, vector<Enemy*>* enemyPool
	)
		// event: �¼�������, isRunning: �����Ƿ�����, 
		// player: ��Ҷ���, bulletList: �ӵ��б�, 
		// enemyList: �����б�, enemyPool: ���˳�
	{
		this->event = event;
		this->isRunning = isRunning;
		this->player = player;
		this->bulletList = bulletList;
		this->enemyList = enemyList;
		this->enemyPool = enemyPool;
	}

	gameInterface() = default;

	~gameInterface() override
	{
		if (bkg != nullptr)
		{
			delete bkg;
		}
	}

public:
	gameInterface* setPlayer(Player* player) // ������Ҷ���
	{
		this->player = player;
		return this;
	}

	gameInterface* setBulletList(vector<Bullet>* bulletList) // �����ӵ��б�
	{
		this->bulletList = bulletList;
		return this;
	}

	gameInterface* setEnemyList(vector<Enemy*>* enemyList) // ���õ����б�
	{
		this->enemyList = enemyList;
		return this;
	}

	gameInterface* setEnemyPool(vector<Enemy*>* enemyPool) // ���õ��˳�
	{
		this->enemyPool = enemyPool;
		return this;
	}

	gameInterface* setBkg(IMAGE* bkg) // ���ñ���ͼƬ
	{
		this->bkg = bkg;
		return this;
	}

	gameInterface* setEvent(Event* event) // �����¼�������
	{
		this->event = event;
		return this;
	}

	gameInterface* setDeltaTime(float* deltaT) // ������֮֡���ʱ��������λΪ��
	{
		this->deltaT = deltaT;
		return this;
	}

	gameInterface* setDeltaTick(int* deltaTick) // ������֮֡���ʱ��������λΪ����
	{
		this->deltaTick = deltaTick;
		return this;
	}

	gameInterface* setIsRunning(bool* isRunning) // ���ó�������״̬
	{
		this->isRunning = isRunning;
		return this;
	}

public:
	void render() override
	{
		putImage_regBack(0, 0, bkg);

		player->Draw();

		for (auto& enemy : *enemyList)
		{
			enemy->Draw();
		}

		for (auto& bullet : *bulletList)
		{
			bullet.Draw();
		}

		DrawPlayerScore(score);
	}

	void updateStatus() override // ������Ϸ״̬
	{
		player->Move(*deltaT);

		bulletOrbitPlayer(*bulletList, *player, *deltaT, 2.0, 85.0, 30.0, 3.0); // �ӵ��������ת

		tryGenEnemy(*enemyList, *enemyPool, 15, *deltaTick, 400); // ���ɵ���

		for (auto& enemy : *enemyList)
		{
			enemy->Move(*player, *deltaT);
		}

		for (auto& enemy : *enemyList) // �������Ƿ񱻵��˻���
		{
			if (enemy->checkPlayerHit(*player))
			{
				TCHAR message[256];
				_stprintf(message, _T("��ɹ������: %d �Ρ�\n���㻹�Ƿ�������"), score);
				MessageBox(GetHWnd(), message, _T("Game Over"), MB_OK);
				*isRunning = false;
				break;
			}
		}

		removeEnemyFromList(*enemyList, *bulletList, score); // �Ƴ������еĵ��˲��Ʒ�
	}

	void monitorEvent() override
	{
		event->handleKeyBoardEvent();
		event->handleMouseEvent();
		player->getEvent();
	}

private:
	float* deltaT = nullptr; // ��֮֡���ʱ��������λΪ��
	int* deltaTick = nullptr; // ��֮֡���ʱ��������λΪ����

private:
	Event* event = nullptr;

private:
	Player* player = nullptr;
	vector<Bullet>* bulletList = nullptr;

private:
	vector<Enemy*>* enemyList = nullptr;
	vector<Enemy*>* enemyPool = nullptr;

private:
	IMAGE* bkg = nullptr;

private:
	bool* isRunning = nullptr;

private:
	int score = 0;
};

class startMenu : public UIElement // ��ʼ�˵�
{
public:
	startMenu(Event* event, bool* isGameStart, bool* isRunning)
	{
		this->event = event;
		this->isGameStart = isGameStart;
		this->isRunning = isRunning;
	}

	startMenu() = default;

	~startMenu() override
	{
		if (bkg != nullptr)
		{
			delete bkg;
		}
	}

public:
	startMenu* setStartButton(startButton* startBtn) // ���ÿ�ʼ��ť 
	{
		removeChild(this->startBtn);
		this->startBtn = startBtn;
		addChild(startBtn);
		return this;
	}

	startMenu* setExitButton(exitButton* exitBtn) // �����˳���ť
	{
		removeChild(this->exitBtn);
		this->exitBtn = exitBtn;
		addChild(exitBtn);
		return this;
	}

	startMenu* setGameUI(gameInterface* gameUI) // �������ö�̬�ԣ�������Ϸ����
	{
		if (this->gameUI != nullptr) // ���� nullptr ����
		{
			this->gameUI->setParent(nullptr); // ���ԭ�� parent ��ϵ
			removeChild(this->gameUI);        // �Ƴ��ɵ� gameUI
		}
		this->gameUI = gameUI;               // �����µ� gameUI
		addChild(gameUI);                    // ��ӵ��ӽڵ��б�
		return this;
	}

	startMenu* setBkg(IMAGE* bkg)
	{
		this->bkg = bkg;
		return this;
	}

public:
	void render() override // ��Ⱦ��ʼ�˵��������Ϸδ��ʼ����ʾ��ʼ�˵���������ʾ��Ϸ����
	{
		if (*isRunning && !(*isGameStart))
		{
			putImage_regBack(0, 0, bkg);
			startBtn->draw();
			exitBtn->draw();
		}
		else if (*isRunning && *isGameStart)
		{
			gameUI->render();
		}
	}

	void updateStatus() override
	{
		if (*isRunning && !(*isGameStart))
		{
			startBtn->updateStatus();
			exitBtn->updateStatus();
		}
		else if (*isRunning && *isGameStart)
		{
			gameUI->updateStatus();
		}
	}

	void monitorEvent() override
	{
		if (*isRunning && !(*isGameStart))
		{
			event->handleKeyBoardEvent();
			event->handleMouseEvent();
		}
		else if (*isRunning && *isGameStart)
		{
			gameUI->monitorEvent();
		}
	}

private:
	Event* event = nullptr;

private:
	startButton* startBtn = nullptr;
	exitButton* exitBtn = nullptr;

private:
	bool* isGameStart = nullptr; // ��Ϸ�Ƿ�ʼ
	bool* isRunning = nullptr; // �����Ƿ�����

private:
	IMAGE* bkg = nullptr;

private:
	UIElement* gameUI = nullptr; // ��Ϸ����
};
