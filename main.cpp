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

		// ��ȡ���λ��
		POINT cursor = event.getCursorPos();

		// ��ȡ��ǰ���µİ���
		tstring pressedKey = event.getPressedKey();

		// ��ʾ���λ��
		std::basic_ostringstream<TCHAR> ss;
		ss << _T("Mouse Position: (") << cursor.x << _T(", ") << cursor.y << _T(")");
		outtextxy(50, 50, ss.str().c_str());

		// ��ʾ��ǰ���µİ���
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
// ����¼�
{
	event.handleKeyBoardEvent();
	event.handleMouseEvent();
}

int main()
{
	// ---------------------- ��ʼ����ʼ ----------------------
	initgraph(1280, 720); // ��ʼ�����ڴ�С
	setbkmode(TRANSPARENT); // ���û���Ԫ�صı������͸��

	bool isRunning = true;
	ExMessage msg;

	Event event(&msg); // �¼�������

	IMAGE bkg; // ����ͼƬ
	loadimage(&bkg, _T("Lib/Img/Kobe_bk.png"));

	Player kenshi(_T("Lib/Kenshi/Idle/L_"), _T("Lib/Kenshi/Idle/R_"), 4, 4, 66, 66, 64, 80, 500.0f);
	kenshi.setShadow(_T("Lib/Kenshi/shadow_player.png"), 32, 20);
	kenshi.setMoveAnime(_T("Lib/Kenshi/Run/L_"), _T("Lib/Kenshi/Run/R_"), 8, 8, 52, 52);
	kenshi.setHittingBox(60, 75);
	kenshi.setEvent(&event);

	vector<Bullet> bulletList(3); // �ӵ��б�

	vector<Enemy*> enemyPool, enemyList; // ���˳غ͵����б�
	initEnemyPool
	(
		enemyPool, _T("Lib/Enemy/Boar/boar_left_"), _T("Lib/Enemy/Boar/boar_right_"),
		6, 6, 60, 60, 96, 96, 80.0f, 50
	);

	DWORD lastTime = GetTickCount();

	BeginBatchDraw();
	//---------------------- ��ʼ������ ----------------------

	while (isRunning)
	{
		// ������֮֡���ʱ����
		DWORD start_loop = GetTickCount();
		float deltaT = (start_loop - lastTime) / 1000.0f;
		int tickGap = start_loop - lastTime;
		lastTime = start_loop;

		tryGenEnemy(enemyList, enemyPool, 15, tickGap, 400);

		// �����¼�
		while (peekmessage(&msg))
		{
			monitorEvent(event);
			kenshi.getEvent();
		}

		//---------------------- ��Ϸ�߼���ʼ ----------------------
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
				MessageBox(GetHWnd(), _T("�������������"), _T("Game Over"), MB_OK);
				isRunning = false;
				break;
			}
		}

		removeEnemyFromList(enemyList, bulletList);
		//---------------------- ��Ϸ�߼����� ----------------------

		cleardevice(); // ����

		putimage(0, 0, &bkg); // ���Ʊ���

		kenshi.Draw(1.2);

		for (auto& enemy : enemyList)
		{
			enemy->Draw();
		}

		for (auto& bullet : bulletList)
		{
			bullet.Draw();
		}

		FlushBatchDraw(); // ˢ�»���

		DWORD end_loop = GetTickCount();
		if (end_loop - start_loop < 1000 / 120)
		{
			Sleep(1000 / 120 - (end_loop - start_loop)); // ���Ƴ�������֡��
		}

	}

	EndBatchDraw();

	return 0;
}
#endif