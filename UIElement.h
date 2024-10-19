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
	UIElement* addChild(UIElement* child) // 添加子UI界面
	{
		if (child != nullptr)
		{
			child->setParent(this);
			uChildren.push_back(child);
			return this;
		}
	}

	UIElement* addChild(Widget* child) // 添加子控件
	{
		wChildren.push_back(child);
		return this;
	}

	UIElement* removeChild(UIElement* child) // 移除子UI界面
	{
		auto aim = find(uChildren.begin(), uChildren.end(), child);
		if (aim != uChildren.end())
		{
			uChildren.erase(aim);
			child->parent = nullptr;
		}
		return this;
	}

	UIElement* removeChild(Widget* child) // 移除子控件
	{
		auto aim = find(wChildren.begin(), wChildren.end(), child);
		if (aim != wChildren.end())
		{
			wChildren.erase(aim);
		}
		return this;
	}

	void setParent(UIElement* parent) // 设置父UI界面
	{
		this->parent = parent;
	}

public:
	virtual void render() // 渲染UI界面
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

	virtual void updateStatus() // 更新UI界面状态
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

	virtual void monitorEvent() {} // 监听事件

public:
	UIElement* getParent() const // 获取父UI界面
	{
		return parent;
	}

protected:
	vector<UIElement*> uChildren; // 子UI界面列表
	vector<Widget*> wChildren; // 子控件列表

protected:
	UIElement* parent = nullptr; // 父UI界面
};

class gameInterface : public UIElement // 游戏界面
{
public:
	gameInterface
	(
		Event* event, bool* isRunning,
		Player* player, vector<Bullet>* bulletList,
		vector<Enemy*>* enemyList, vector<Enemy*>* enemyPool
	)
		// event: 事件处理器, isRunning: 程序是否运行, 
		// player: 玩家对象, bulletList: 子弹列表, 
		// enemyList: 敌人列表, enemyPool: 敌人池
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
	gameInterface* setPlayer(Player* player) // 设置玩家对象
	{
		this->player = player;
		return this;
	}

	gameInterface* setBulletList(vector<Bullet>* bulletList) // 设置子弹列表
	{
		this->bulletList = bulletList;
		return this;
	}

	gameInterface* setEnemyList(vector<Enemy*>* enemyList) // 设置敌人列表
	{
		this->enemyList = enemyList;
		return this;
	}

	gameInterface* setEnemyPool(vector<Enemy*>* enemyPool) // 设置敌人池
	{
		this->enemyPool = enemyPool;
		return this;
	}

	gameInterface* setBkg(IMAGE* bkg) // 设置背景图片
	{
		this->bkg = bkg;
		return this;
	}

	gameInterface* setEvent(Event* event) // 设置事件处理器
	{
		this->event = event;
		return this;
	}

	gameInterface* setDeltaTime(float* deltaT) // 设置两帧之间的时间间隔，单位为秒
	{
		this->deltaT = deltaT;
		return this;
	}

	gameInterface* setDeltaTick(int* deltaTick) // 设置两帧之间的时间间隔，单位为毫秒
	{
		this->deltaTick = deltaTick;
		return this;
	}

	gameInterface* setIsRunning(bool* isRunning) // 设置程序运行状态
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

	void updateStatus() override // 更新游戏状态
	{
		player->Move(*deltaT);

		bulletOrbitPlayer(*bulletList, *player, *deltaT, 2.0, 85.0, 30.0, 3.0); // 子弹绕玩家旋转

		tryGenEnemy(*enemyList, *enemyPool, 15, *deltaTick, 400); // 生成敌人

		for (auto& enemy : *enemyList)
		{
			enemy->Move(*player, *deltaT);
		}

		for (auto& enemy : *enemyList) // 检查玩家是否被敌人击中
		{
			if (enemy->checkPlayerHit(*player))
			{
				TCHAR message[256];
				_stprintf(message, _T("你成功肘击了: %d 次。\n但你还是飞起来了"), score);
				MessageBox(GetHWnd(), message, _T("Game Over"), MB_OK);
				*isRunning = false;
				break;
			}
		}

		removeEnemyFromList(*enemyList, *bulletList, score); // 移除被击中的敌人并计分
	}

	void monitorEvent() override
	{
		event->handleKeyBoardEvent();
		event->handleMouseEvent();
		player->getEvent();
	}

private:
	float* deltaT = nullptr; // 两帧之间的时间间隔，单位为秒
	int* deltaTick = nullptr; // 两帧之间的时间间隔，单位为毫秒

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

class startMenu : public UIElement // 开始菜单
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
	startMenu* setStartButton(startButton* startBtn) // 设置开始按钮 
	{
		removeChild(this->startBtn);
		this->startBtn = startBtn;
		addChild(startBtn);
		return this;
	}

	startMenu* setExitButton(exitButton* exitBtn) // 设置退出按钮
	{
		removeChild(this->exitBtn);
		this->exitBtn = exitBtn;
		addChild(exitBtn);
		return this;
	}

	startMenu* setGameUI(gameInterface* gameUI) // 设置利用多态性，设置游戏界面
	{
		if (this->gameUI != nullptr) // 避免 nullptr 出现
		{
			this->gameUI->setParent(nullptr); // 解除原有 parent 关系
			removeChild(this->gameUI);        // 移除旧的 gameUI
		}
		this->gameUI = gameUI;               // 传入新的 gameUI
		addChild(gameUI);                    // 添加到子节点列表
		return this;
	}

	startMenu* setBkg(IMAGE* bkg)
	{
		this->bkg = bkg;
		return this;
	}

public:
	void render() override // 渲染开始菜单，如果游戏未开始则显示开始菜单，否则显示游戏界面
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
	bool* isGameStart = nullptr; // 游戏是否开始
	bool* isRunning = nullptr; // 程序是否运行

private:
	IMAGE* bkg = nullptr;

private:
	UIElement* gameUI = nullptr; // 游戏界面
};
