#include "reliance.h"
#pragma comment(lib, "MSIMG32.LIB")

#define PLAYER_SPEED 450.0F
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720 
#define INTERVAL 1000.0F / 120.0F

using namespace std;

#define _BR_MAIN
#define _DEBUG_

#ifdef _BR_BUTTON

int main()
{
	initgraph(800, 800);
	setbkcolor(DARKGRAY);
	setbkmode(TRANSPARENT);

	ExMessage msg;

	Event event(&msg);

	Button exitButton({400,400},{336,120},&event);
	exitButton
		.setIdleImg(_T("Lib/Button/Game menu/Exit/idle.png"))
		.setHoverImg(_T("Lib/Button/Game menu/Exit/hover.png"))
		.setPressedImg(_T("Lib/Button/Game menu/Exit/pressed.png"));

	BeginBatchDraw();

	while (true)
	{
		while (peekmessage(&msg))
		{
			event.handleKeyBoardEvent();
			event.handleMouseEvent();
		}

		exitButton.updateStatus();

		cleardevice();

		// 获取鼠标位置
		POINT cursor = event.getCursorPos();

		// 获取当前按下的按键
		tstring pressedKey = event.getPressedKey();

		// 显示鼠标位置
		std::basic_ostringstream<TCHAR> ss;
		ss << _T("Mouse Position: (") << cursor.x << _T(", ") << cursor.y << _T(")");
		outtextxy(50, 50, ss.str().c_str());

		// 显示当前按下的按键
		tstring keyInfo = _T("Pressed Key: ") + pressedKey;
		outtextxy(50, 100, keyInfo.c_str());

		exitButton.draw();

		FlushBatchDraw();

		Sleep(1000 / 120);
	}

	EndBatchDraw();

	return 0;
}

#elif defined(_BR_MAIN)
void monitorEvent(Event& event)
// 监控事件
{
	event.handleKeyBoardEvent();
	event.handleMouseEvent();
}

int main()
{
	// ---------------------- 初始化开始 ----------------------
	initgraph(1280, 720); // 初始化窗口大小
	setbkmode(TRANSPARENT); // 设置绘制元素的背景填充透明

	bool isRunning = true;
	ExMessage msg;

	Event event(&msg); // 事件处理器

	IMAGE bkg; // 背景图片
	loadimage(&bkg, _T("Lib/Img/Kobe_bk.png"));

	Player kenshi(_T("Lib/Kenshi/Idle/L_"), _T("Lib/Kenshi/Idle/R_"), 4, 4, 66, 66, 64, 80, 500.0f);
	kenshi.setShadow(_T("Lib/Kenshi/shadow_player.png"), 32, 20);
	kenshi.setMoveAnime(_T("Lib/Kenshi/Run/L_"), _T("Lib/Kenshi/Run/R_"), 8, 8, 52, 52);
	kenshi.setHittingBox(60, 75);
	kenshi.setEvent(&event);

	vector<Bullet> bulletList(3); // 子弹列表

	vector<Enemy*> enemyPool, enemyList; // 敌人池和敌人列表
	initEnemyPool
	(
		enemyPool, _T("Lib/Enemy/Boar/boar_left_"), _T("Lib/Enemy/Boar/boar_right_"),
		6, 6, 60, 60, 96, 96, 80.0f, 50
	);

	DWORD lastTime = GetTickCount();

	BeginBatchDraw();
	//---------------------- 初始化结束 ----------------------

	while (isRunning)
	{
		// 计算两帧之间的时间间隔
		DWORD start_loop = GetTickCount();
		float deltaT = (start_loop - lastTime) / 1000.0f;
		int tickGap = start_loop - lastTime;
		lastTime = start_loop;

		tryGenEnemy(enemyList, enemyPool, 15, tickGap, 400);

		// 处理事件
		while (peekmessage(&msg))
		{
			monitorEvent(event);
			kenshi.getEvent();
		}

		//---------------------- 游戏逻辑开始 ----------------------
		kenshi.Move(deltaT);

		bulletOrbitPlayer(bulletList, kenshi, deltaT, 3.0, 85.0, 30.0, 3.0);

		for (auto& enemy : enemyList)
		{
			enemy->Move(kenshi, deltaT);
		}

		for (auto& enemy : enemyList)
		{
			if (enemy->checkPlayerHit(kenshi))
			{
				MessageBox(GetHWnd(), _T("让你飞起来！！"), _T("Game Over"), MB_OK);
				isRunning = false;
				break;
			}
		}

		removeEnemyFromList(enemyList, bulletList);
		//---------------------- 游戏逻辑结束 ----------------------

		cleardevice(); // 清屏

		putimage(0, 0, &bkg); // 绘制背景

		kenshi.Draw(1.2);

		for (auto& enemy : enemyList)
		{
			enemy->Draw();
		}

		for (auto& bullet : bulletList)
		{
			bullet.Draw();
		}

		FlushBatchDraw(); // 刷新画面

		DWORD end_loop = GetTickCount();
		if (end_loop - start_loop < 1000 / 120)
		{
			Sleep(1000 / 120 - (end_loop - start_loop)); // 控制程序运行帧率
		}

	}

	EndBatchDraw();

	return 0;
}
#endif