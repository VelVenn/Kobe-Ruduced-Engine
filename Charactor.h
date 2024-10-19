#pragma once

#include <vector>
#include <string>
#include <tchar.h>
#include <graphics.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include "Animation.h"
#include "Widget.h"
#include "tstring.h"

#define PLAYER_SPEED 450.0F
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720 

using namespace std;

typedef struct PlayerPosition
// ���ڱ�֤��ɫλ�õľ���
{
	float x;
	float y;
}fPOINT;

class Role
{
public:
	Role() = default;

	Role(tstring bPathL, tstring bPathR, int lFrNum, int rFrNum, int lFrGap, int rFrGap,
		int charW, int charH, float roleSp, float roleX = 640.0f, float roleY = 360.0f)
		// bPathL: ���ƶ�����֡·�� bPathR: ���ƶ�����֡·�� lFrNum: ���ƶ�֡�� rFrNum: ���ƶ�֡��
		// lFrGap: ���ƶ�֡��� rFrGap: ���ƶ�֡��� charW: ��ɫ��� charH: ��ɫ�߶� roleSp: ��ɫ�ٶ�
	{
		animeL = new Animation(bPathL, lFrNum, lFrGap);
		animeR = new Animation(bPathR, rFrNum, rFrGap);

		roleW = charW;
		roleH = charH;

		rolePos.x = roleX;
		rolePos.y = roleY;

		roleSpeed = roleSp;
	}

	virtual ~Role()
	{
		if (Shadow != nullptr)
			delete Shadow;

		if (animeL != nullptr)
			delete animeL;

		if (animeR != nullptr)
			delete animeR;

		if (attackL != nullptr)
			delete attackL;

		if (attackR != nullptr)
			delete attackR;

		if (moveL != nullptr)
			delete moveL;

		if (moveR != nullptr)
			delete moveR;
	}

	void setAnime(tstring bPathL, tstring bPathR, int lFrNum, int rFrNum, int lFrGap, int rFrGap)
		// ���ý�ɫ��ֹ����
	{
		animeL = new Animation(bPathL, lFrNum, lFrGap);
		animeR = new Animation(bPathR, rFrNum, rFrGap);
	}

	void setAttackAnime(tstring bPathL, tstring bPathR, int lFrNum, int rFrNum, int lFrGap, int rFrGap)
		// ���ý�ɫ��������
	{
		attackL = new Animation(bPathL, lFrNum, lFrGap);
		attackR = new Animation(bPathR, rFrNum, rFrGap);
	}

	void setMoveAnime(tstring bPathL, tstring bPathR, int lFrNum, int rFrNum, int lFrGap, int rFrGap)
		// ���ý�ɫ�ƶ�����
	{
		moveL = new Animation(bPathL, lFrNum, lFrGap);
		moveR = new Animation(bPathR, rFrNum, rFrGap);
	}

	void setEvent(Event* event)
		// �����¼�
	{
		this->event = event;
	}

	void setRolePos(float roleX, float roleY)
		// ���ý�ɫλ��
	{
		rolePos.x = roleX;
		rolePos.y = roleY;
	}

	void setRoleSpeed(float roleSp)
		// ���ý�ɫ�ٶ�
	{
		roleSpeed = roleSp;
	}

	void setRoleSize(int charW, int charH)
		// ���ý�ɫ��С
	{
		roleW = charW;
		roleH = charH;
	}

	void setShadow(tstring shadowPath, int shadowWidth, int shadowHeight, float heightRatio = 0.0f)
		// ���ý�ɫ��Ӱ
	{
		shadowW = shadowWidth;
		shadowH = shadowHeight;
		shRatio = heightRatio;

		Shadow = new IMAGE();
		loadimage(Shadow, shadowPath.c_str());
	}

	void getEvent()
		// ��ȡ�¼�
	{
		if (event != nullptr)
		{
			isPressKey[0] = event->isKeyPressed(VK_UP) || event->isKeyPressed('W');
			isPressKey[1] = event->isKeyPressed(VK_DOWN) || event->isKeyPressed('S');
			isPressKey[2] = event->isKeyPressed(VK_LEFT) || event->isKeyPressed('A');
			isPressKey[3] = event->isKeyPressed(VK_RIGHT) || event->isKeyPressed('D');
		}
	}

	virtual void Move(float deltaTime = 1.0f)
		// deltaTime: ��ѭ��ÿ֡ʱ����
	{
		float moveX = 0.0f;
		float moveY = 0.0f;

		if (isPressKey[0])
		{
			moveY -= roleSpeed;
		}
		if (isPressKey[1])
		{
			moveY += roleSpeed;
		}
		if (isPressKey[2])
		{
			moveX -= roleSpeed;
		}
		if (isPressKey[3])
		{
			moveX += roleSpeed;
		}

		float length = sqrt(moveX * moveX + moveY * moveY);

		if (length > 0)
		{
			moveX /= length;
			moveY /= length;

			deltaX = moveX * roleSpeed * deltaTime;
			deltaY = moveY * roleSpeed * deltaTime;

			rolePos.x += deltaX; // roleSpeed���ù�Сʱ�ᵼ�¾��ȶ�ʧ�����½�ɫ�ƶ�������
			rolePos.y += deltaY;

			LOG
			(
				"Delta: " << "X: " << deltaX << " "
				<< "Y: " << deltaY
			);

			LOG
			(
				"Role Pos: " << "X: " << rolePos.x << " "
				<< "Y: " << rolePos.y
			);
		}
		else
		{
			// ���û���κΰ������£�����ȷ���� deltaX �� deltaY
			deltaX = 0.0f;
			deltaY = 0.0f;
		}

		if (rolePos.x < 0)
		{
			rolePos.x = 0;
		}
		if (rolePos.x + roleW > SCREEN_WIDTH)
		{
			rolePos.x = SCREEN_WIDTH - roleW;
		}
		if (rolePos.y < 0)
		{
			rolePos.y = 0;
		}
		if (rolePos.y + roleH > SCREEN_HEIGHT)
		{
			rolePos.y = SCREEN_HEIGHT - roleH;
		}
	}

	void updateState()
		// ���½�ɫ״̬
	{
		if (deltaX == 0 && deltaY == 0)
		{
			currentState = RoleState::Idle;
		}
		else
		{
			currentState = RoleState::Moving;
		}
	}

	virtual void Draw()
		// shRatio: ��Ӱ�߶ȱ���
	{
		updateState(); // ���½�ɫ״̬

		if (Shadow != nullptr && shadowW != -1 && shadowH != -1)
		{
			int ShadowX = static_cast<int>(rolePos.x + (roleW / 2 - shadowW / 2));
			int ShadowY = static_cast<int>(rolePos.y + roleH - shadowH * shRatio);
			putImage_regBack(ShadowX, ShadowY, Shadow);
		}

		switch (currentState)
		{
		case RoleState::Idle:
			moveL->stop(); // ֹͣ�������ƶ�����
			moveR->stop(); // ֹͣ�������ƶ�����
			if (facingLeft)
			{
				animeL->playFrame(static_cast<int>(rolePos.x), static_cast<int>(rolePos.y));
			}
			else
			{
				animeR->playFrame(static_cast<int>(rolePos.x), static_cast<int>(rolePos.y));
			}
			break;
		case RoleState::Moving:
			animeL->stop(); // ֹͣ������ֹ����
			animeR->stop(); // ֹͣ�����Ҿ�ֹ����
			if (deltaX < 0)
			{
				facingLeft = true;
			}
			else if (deltaX > 0)
			{
				facingLeft = false;
			}

			if (facingLeft)
			{
				moveL->playFrame(static_cast<int>(rolePos.x), static_cast<int>(rolePos.y));
			}
			else
			{
				moveR->playFrame(static_cast<int>(rolePos.x), static_cast<int>(rolePos.y));
			}
			break;
		default:
			break;
		}
	}

	fPOINT getRolePos() const
	{
		return rolePos;
	}

	fPOINT getDeltaDistance() const
	{
		return { deltaX, deltaY };
	}
protected:
	Event* event = nullptr;

protected:
	float roleSpeed = 0.0f; // ��ɫ�ٶ�
	int roleW = 0; // ��ɫ���
	int roleH = 0; // ��ɫ�߶�
	int shadowW = -1; // ��Ӱ���
	int shadowH = -1; // ��Ӱ�߶�
	float shRatio = 0.0f; // ��Ӱ�߶ȱ���

protected:
	enum class RoleState
	{
		Idle = 0,  // ��ֹ
		Moving,    // �ƶ�
		Attacking  // ����
	};

	RoleState currentState = RoleState::Idle;

protected:
	fPOINT rolePos = { 0.0f, 0.0f }; // ��ɫλ��

	float deltaX = 0.0f; // ��ɫ�����ƶ��ľ���
	float deltaY = 0.0f; // ��ɫ�����ƶ��ľ���

	bool facingLeft = false; // ��ɫ�Ƿ�������
	bool isPressKey[4] = { false, false, false, false }; // �������Ҽ��Ƿ񱻰���, 0: ��, 1: ��, 2: ��, 3: ��

protected:
	IMAGE* Shadow = nullptr; // ��ӰͼƬ

	Animation* animeL = nullptr; // ��ֹ����
	Animation* animeR = nullptr; // �Ҿ�ֹ����

	Animation* attackL = nullptr; // �󹥻�����
	Animation* attackR = nullptr; // �ҹ�������

	Animation* moveL = nullptr; // ���ƶ�����
	Animation* moveR = nullptr; // ���ƶ�����
};

class Bullet
{
public:
	fPOINT bulletPos = { 300.0f, 300.0f }; // �ӵ�λ��

public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(233, 137, 41));
		setlinestyle(PS_SOLID | PS_JOIN_ROUND, 3);
		setfillcolor(RGB(233, 67, 41));
		setfillstyle(BS_SOLID);

		fillcircle(static_cast<int>(bulletPos.x), static_cast<int>(bulletPos.y), RADIUS);
	}
private:
	const int RADIUS = 10; // �ӵ��뾶
};

class Player : public Role
{
public:
	Player() = default;

	Player
	(
		tstring bPathL, tstring bPathR,
		int lFrNum, int rFrNum,
		int lFrGap, int rFrGap,
		int charW, int charH,
		float roleSp, float roleX = 640.0f, float roleY = 360.0f
	) :
		Role(bPathL, bPathR, lFrNum, rFrNum, lFrGap, rFrGap, charW, charH, roleSp, roleX, roleY)
	{
		setHittingBox(charW, charH);
	}

	~Player() override
	{
	}

	bool setHittingBox(int boxW, int boxH)
		// ���ý�ɫ��ײ���С, boxW: ��� boxH: �߶�
	{
		if (boxW > 0 && boxH > 0 && boxW <= roleW && boxH <= roleH)
		{
			hittingBox.x = boxW;
			hittingBox.y = boxH;
			return true;
		}
		return false;
	}

	const POINT& getHittingBox() const
		// ��ȡ��ɫ��ײ���С
	{
		return hittingBox;
	}

	const POINT& getPlayerSize() const
		// ��ȡ��ɫ��С
	{
		return { roleW, roleH };
	}

private:
	bool isAlive = true;

private:
	POINT hittingBox = { 0, 0 }; // ��ɫ��ײ���С, x: ��� y: �߶�
};

class Enemy : public Role
{
public:
	Enemy() = default;

	Enemy
	(
		tstring bPathL, tstring bPathR, // bPathL: ���ƶ�����֡·�� bPathR: ���ƶ�����֡·��
		int lFrNum, int rFrNum, // lFrNum: ���ƶ�֡�� rFrNum: ���ƶ�֡��
		int lFrGap, int rFrGap, // lFrGap: ���ƶ�֡��� rFrGap: ���ƶ�֡���
		int charW, int charH, // charW: ��ɫ��� charH: ��ɫ�߶�
		float roleSp, float roleX = 0.0f, float roleY = 0.0f // roleSp: ��ɫ�ٶ� roleX: ��ɫX���� roleY: ��ɫY����
	) :
		Role(bPathL, bPathR, lFrNum, rFrNum, lFrGap, rFrGap, charW, charH, roleSp, roleX, roleY)
	{
		resetPos();
		hittingPoint.x = rolePos.x + static_cast<float>(roleW / 2);
		hittingPoint.y = rolePos.y + static_cast<float>(roleH / 2);
	}

	~Enemy() override
	{
	}

	bool checkBulletHit(const Bullet& bullet) const
		// ���ӵ���ЧΪһ���㣬�жϵ����Ƿ񱻻���
	{
		bool isOverlapX = bullet.bulletPos.x >= rolePos.x && bullet.bulletPos.x <= rolePos.x + roleW;
		bool isOverlapY = bullet.bulletPos.y >= rolePos.y && bullet.bulletPos.y <= rolePos.y + roleH;

		return isOverlapX && isOverlapY;
	}

	bool checkPlayerHit(const Player& player) const
		// �����˵�ЧΪһ���㣬�ж�����Ƿ񱻻���
	{
		auto hitBox = player.getHittingBox(); // ��ȡ��ҵ���ײ���С, x: ��� y: �߶�

		// getPlayerSize() ��ȡ��ҵĴ�С, x: ��� y: �߶�
		// hitBoxPosX: ��ײ�����Ͻǵ�X���� hitBoxPosY: ��ײ�������Ͻǵ�Y����
		int hitBoxPosX =
			static_cast<int>(player.getRolePos().x + (player.getPlayerSize().x - hitBox.x) / 2);

		int hitBoxPosY =
			static_cast<int>(player.getRolePos().y + (player.getPlayerSize().y - hitBox.y) / 2);

		bool isOverlapX = hittingPoint.x > hitBoxPosX && hittingPoint.x < hitBoxPosX + hitBox.x;
		bool isOverlapY = hittingPoint.y > hitBoxPosY && hittingPoint.y < hitBoxPosY + hitBox.y;

		LOG("\noverlapX: " << isOverlapX << " overlapY: " << isOverlapY);
		LOG("hit point: " << "X: " << hittingPoint.x << " Y: " << hittingPoint.y << '\n');
		return isOverlapX && isOverlapY;
	}

	void setActiveStatus(const vector<Bullet>& clip)
		// ���õ��˵Ĵ��״̬
	{
		for (const auto& ammo : clip)
		{
			if (checkBulletHit(ammo))
			{
				isAlive = false;
				break;
			}
		}
	}

	void setActiveStatus(bool status)
	{
		isAlive = status;
	}

	void Move
	(
		const Player& player, float deltaTime,
		float lockDistance = 0.1f, float minMoveThreshold = 0.01f,
		float slowDownDist = 10.0f
	)   
		// player: ��ҽ�ɫ, deltaTime: ��ѭ��ÿ֡ʱ���� 
		// lockDistance: ��������, minMoveThreshold: ��С�ƶ���ֵ, slowDownDist: ���پ���
	{
		fPOINT playerPos = player.getRolePos();
		float moveX = playerPos.x - rolePos.x;
		float moveY = playerPos.y - rolePos.y;

		float length = sqrt(moveX * moveX + moveY * moveY);

		if (length >= lockDistance)
		{
			float slowDownFactor = length < slowDownDist ? length / slowDownDist : 1.0f; // ��������

			moveX /= length;
			moveY /= length;

			deltaX = moveX * roleSpeed * deltaTime * slowDownFactor;
			deltaY = moveY * roleSpeed * deltaTime * slowDownFactor;

			if (fabs(deltaX) > minMoveThreshold || fabs(deltaY) > minMoveThreshold)
			{
				rolePos.x += deltaX;
				rolePos.y += deltaY;
				setHittingPoint();
			}
		}
		else
		{
			rolePos = playerPos;
			setHittingPoint();

			deltaX = 0.0f;
			deltaY = 0.0f;
		}
	}

	void resetPos()
	{
		enum class SpawnPos
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT
		};

		SpawnPos edge = (SpawnPos)(rand() % 4);
		switch (edge)
		{
		case SpawnPos::TOP:
			rolePos.x = rand() % SCREEN_WIDTH;
			rolePos.y = -roleH;
			break;
		case SpawnPos::BOTTOM:
			rolePos.x = rand() % SCREEN_WIDTH;
			rolePos.y = SCREEN_HEIGHT;
			break;
		case SpawnPos::LEFT:
			rolePos.x = -roleW;
			rolePos.y = rand() % SCREEN_HEIGHT;
			break;
		case SpawnPos::RIGHT:
			rolePos.x = SCREEN_WIDTH;
			rolePos.y = rand() % SCREEN_HEIGHT;
			break;
		default:
			break;
		}
	}

	void setHittingPoint()
		// ���õ��˵Ĺ�����
	{
		hittingPoint.x = rolePos.x + static_cast<float>(roleW / 2);
		hittingPoint.y = rolePos.y + static_cast<float>(roleH / 2);
	}

	void Draw(float shRatio = 0, float chTolerance = 0.2f)
		// shRatio: ��Ӱ�߶ȱ��� chTolerance: �����ݲ�
	{
		if (Shadow != nullptr && shadowW != -1 && shadowH != -1)
		{
			int ShadowX = static_cast<int>(rolePos.x + (roleW / 2 - shadowW / 2));
			int ShadowY = static_cast<int>(rolePos.y + roleH - shadowH * shRatio);
			putImage_regBack(ShadowX, ShadowY, Shadow);
		}

		if (fabs(deltaX) > chTolerance) // �����ݲ� 
		{
			facingLeft = deltaX < 0;
		}

		if (facingLeft)
		{
			animeL->playFrame(static_cast<int>(rolePos.x), static_cast<int>(rolePos.y));
		}
		else
		{
			animeR->playFrame(static_cast<int>(rolePos.x), static_cast<int>(rolePos.y));
		}
	}

	bool isActive() const
	{
		return isAlive;
	}

	void reset()
	{
		resetPos();
		setHittingPoint();
	}

private:
	fPOINT hittingPoint = { 0.0f, 0.0f };

private:
	bool facingLeft = false;
	bool isAlive = false;
};


void initEnemyPool
(
	vector<Enemy*>& enemyPool, tstring bPathL, tstring bPathR,
	int lFrNum, int rFrNum, int lFrGap, int rFrGap,
	int charW, int charH, float roleSp,
	int	poolSize
)
{
	for (int i = 0; i < poolSize; i++)
	{
		enemyPool.push_back(new Enemy(bPathL, bPathR, lFrNum, rFrNum, lFrGap, rFrGap, charW, charH, roleSp));
	}
}

Enemy* getEnemyFromPool(vector<Enemy*>& enemyPool)
{
	for (auto& enemy : enemyPool)
	{
		if (!enemy->isActive())  // ͨ�� isActive() �жϵ����Ƿ��ڿ���״̬
		{
			enemy->setActiveStatus(true);  // ���õ���Ϊ����״̬
			enemy->reset();  // ���õ��˵�λ��
			return enemy;
		}
	}
	return nullptr;  // ���������ĵ��˶���ʹ���У����Է��ؿջ�����չ����
}

void tryGenEnemy
(
	vector<Enemy*>& enemyList, vector<Enemy*>& enemyPool,
	int maxEnemyAmount, int deltaTick, int genGap
)
// enemyList: �����б� maxEnemyAmount: ���������� deltaTick: ��ѭ��ÿ֡ʱ���� genGap: ���ɵ��˼��
{
	static int timer = 0;
	timer += deltaTick;
	if (enemyList.size() < maxEnemyAmount)
	{
		if (timer >= genGap)
		{
			Enemy* enemy = getEnemyFromPool(enemyPool);
			if (enemy != nullptr)
			{
				enemyList.push_back(enemy);
			}
			timer = 0;
		}
	}
}

bool removeEnemyFromList(vector<Enemy*>& enemyList, const vector<Bullet>& clip)
// �ӵ����б����Ƴ������ĵ���
// enemyList: �����б�, clip: �ӵ��б�
{
	auto enemy = enemyList.begin();
	while (enemy != enemyList.end())
	{
		(*enemy)->setActiveStatus(clip); // ���õ��˵Ĵ��״̬
		if (!(*enemy)->isActive())
		{
			enemy = enemyList.erase(enemy); // ���б����Ƴ�����
		}
		else
		{
			enemy++; // ָ����һ������
		}
	}
	return true;
}

bool removeEnemyFromList(vector<Enemy*>& enemyList, const vector<Bullet>& clip, int& score)
// �ӵ����б����Ƴ������ĵ���, ���Ʒ�
// enemyList: �����б�, clip: �ӵ��б�, score: ����
{
	auto enemy = enemyList.begin();
	while (enemy != enemyList.end())
	{
		(*enemy)->setActiveStatus(clip); // ���õ��˵Ĵ��״̬
		if (!(*enemy)->isActive())
		{
			score++;
			enemy = enemyList.erase(enemy); // ���б����Ƴ�����
		}
		else
		{
			enemy++; // ָ����һ������
		}
	}
	return true;
}

void DrawPlayerScore(int score)
{
	static TCHAR text[64]; // ����һ����̬�ַ�����
	_stprintf_s(text, _T("��ǰ��ҵ÷�:%d"), score);// ��ʽ���ַ��������÷ֲ��뵽 text ��

	setbkmode(TRANSPARENT); // ���ñ���ģʽΪ͸��
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}

void bulletOrbitPlayer
(
	vector<Bullet>& bulletList, const Player& player, float deltaTime,
	float rotateVol = 5.0f, float rotateRadius = 80.0f,
	float amplitude = 20.0f, float frequency = 4.0f
)
// �ӵ�Χ�������ת
// bulletList: �ӵ��б�, player: ��ҽ�ɫ, deltaTime: ��ѭ��ÿ֡ʱ����
// rotateVol: ��ת�ٶ�(rad/s), rotateRadius: ��ת�뾶, amplitude: ���, frequency: Ƶ��
{
	static float bulletAngle = 0; // ��һ���ӵ��ĽǶ�
	float angleGap = 2 * M_PI / bulletList.size(); // �ӵ�����Ƕ�

	for (size_t i = 0; i < bulletList.size(); i++)
	{
		float curAngle = bulletAngle + i * angleGap; // ��ǰ�ӵ��Ƕ�

		// �����ӵ��ļ������������ӵ���λ��
		// bulletCentralX = playerCentralX + r(theta) * cos(theta)
		// bulletCentralY = playerCentralY + r(theta) * sin(theta)
		float curveFactor = rotateRadius + amplitude * cos(frequency * curAngle); // ��������

		bulletList[i].bulletPos.x
			= player.getRolePos().x + player.getPlayerSize().x / 2 + curveFactor * cos(curAngle);

		bulletList[i].bulletPos.y
			= player.getRolePos().y + player.getPlayerSize().y / 2 + curveFactor * sin(curAngle);
	}

	bulletAngle += rotateVol * deltaTime; // ͨ��ʱ������һ����ת�ٶ����ӵ��Ƕ�����
	bulletAngle = fmod(bulletAngle, 2 * M_PI); // ʹ�� fmod ������Ƕȵ�ѭ��, ��ֹ�ӵ��Ƕȴ��� 2 * PI
}
