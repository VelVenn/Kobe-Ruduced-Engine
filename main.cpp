#include "reliance.h"
#pragma comment(lib, "MSIMG32.LIB")

#define PLAYER_SPEED 450.0F
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720 
#define INTERVAL 1000.0F / 120.0F

using namespace std;

int main()
{
	initgraph(1280, 720);

	bool isRunning = true;
	ExMessage msg;

	IMAGE bkg; // 背景图片
	loadimage(&bkg, _T("Lib/Img/Kobe_bk.png"));

	Player kenshi(_T("Lib/Kenshi/Idle/L_"), _T("Lib/Kenshi/Idle/R_"), 4, 4, 66, 66, 64, 80, 500.0f);
	kenshi.setShadow(_T("Lib/Kenshi/shadow_player.png"), 32, 20);
	kenshi.setMoveAnime(_T("Lib/Kenshi/Run/L_"), _T("Lib/Kenshi/Run/R_"), 8, 8, 52, 52);
	kenshi.setHittingBox(60, 75);

	vector<Bullet> bulletList(3); // 子弹列表

	vector<Enemy*> enemyPool, enemyList; // 敌人池和敌人列表
	initEnemyPool
	(
		enemyPool, _T("Lib/Enemy/Boar/boar_left_"), _T("Lib/Enemy/Boar/boar_right_"),
		6, 6, 60, 60, 96, 96, 80.0f, 50
	);

	DWORD lastTime = GetTickCount();

	BeginBatchDraw();

	while (isRunning)
	{
		DWORD start_loop = GetTickCount();
		float deltaT = (start_loop - lastTime) / 1000.0f;
		int tickGap = start_loop - lastTime;
		lastTime = start_loop;

		tryGenEnemy(enemyList, enemyPool, 15, tickGap, 400);

		while (peekmessage(&msg))
		{
			kenshi.getEvent(msg);
		}

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

		cleardevice();

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

		FlushBatchDraw();

		DWORD end_loop = GetTickCount();
		if (end_loop - start_loop < 1000 / 120)
		{
			Sleep(1000 / 120 - (end_loop - start_loop)); // 控制程序运行帧率
		}

	}

	EndBatchDraw();

	return 0;
}