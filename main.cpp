#include "reliance.h"
#pragma comment(lib, "MSIMG32.LIB")

#define PLAYER_SPEED 450.0F
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720 
#define INTERVAL 1000.0F / 120.0F

using namespace std;

#define _BR_MAIN


#ifdef _BR_BUTTON

int main()
{
	initgraph(800, 800);
	setbkcolor(DARKGRAY);
	setbkmode(TRANSPARENT);

	ExMessage msg;

	Event event(&msg);

	Button exitButton({ 400,400 }, { 336,120 }, &event);
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
int main()
{
	// ---------------------- 初始化游戏对象 ----------------------
	initgraph(1280, 720); // 初始化窗口大小
	setbkmode(TRANSPARENT); // 设置绘制元素的背景填充透明

	bool isRunning = true;
	bool isGameStart = false;

	ExMessage msg;

	Event event(&msg); // 事件处理器

	IMAGE gameBk; // 游戏背景图片
	loadimage(&gameBk, _T("Lib/Img/Kobe_bk.png"));

	IMAGE startBk; // 开始界面背景图片
	loadimage(&startBk, _T("Lib/Img/pmbkg.png"));

	startButton startBtn({ 472, 400 }, { 336, 120 }, &event, &isGameStart); // 开始按钮
	startBtn
		.setIdleImg(_T("Lib/Button/Game menu/Start/idle.png"))
		.setHoverImg(_T("Lib/Button/Game menu/Start/hover.png"))
		.setPressedImg(_T("Lib/Button/Game menu/Start/pressed.png"));

	exitButton exitBtn({ 472, 550 }, { 336, 120 }, &event, &isRunning); // 退出按钮
	exitBtn
		.setIdleImg(_T("Lib/Button/Game menu/Exit/idle.png"))
		.setHoverImg(_T("Lib/Button/Game menu/Exit/hover.png"))
		.setPressedImg(_T("Lib/Button/Game menu/Exit/pressed.png"));

	Player kenshi(_T("Lib/Kenshi/Idle/L_"), _T("Lib/Kenshi/Idle/R_"), 4, 4, 66, 66, 64, 80, 500.0f);
	kenshi.setShadow(_T("Lib/Kenshi/shadow_player.png"), 32, 20, 1.2f);
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
	float deltaT = 0.0f;
	int tickGap = 0;

	BeginBatchDraw();
	//---------------------- 初始化游戏对象 ----------------------

	//---------------------- 初始化UI界面 ----------------------
	gameInterface* gameUI = new gameInterface(&event, &isRunning, &kenshi, &bulletList, &enemyList, &enemyPool);
	gameUI
		->setBkg(&gameBk)
		->setDeltaTime(&deltaT)
		->setDeltaTick(&tickGap);

	startMenu* startUI = new startMenu(&event, &isGameStart, &isRunning);
	startUI->setBkg(&startBk);
	startUI->setStartButton(&startBtn);
	startUI->setExitButton(&exitBtn);
	startUI->setGameUI(gameUI);

	//---------------------- 初始化UI界面 ----------------------

	while (isRunning)
	{
		// 计算两帧之间的时间间隔
		DWORD start_loop = GetTickCount();
		deltaT = (start_loop - lastTime) / 1000.0f;
		tickGap = start_loop - lastTime;
		lastTime = start_loop;

		// 处理事件
		while (peekmessage(&msg))
		{
			startUI->monitorEvent();
		}

		//---------------------- 游戏逻辑开始 ----------------------

		startUI->updateStatus(); // 更新UI状态

		//---------------------- 游戏逻辑结束 ----------------------

		cleardevice(); // 清屏

		startUI->render(); // 渲染UI

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