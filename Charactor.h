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
// 用于保证角色位置的精度
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
		// bPathL: 左移动动画帧路径 bPathR: 右移动动画帧路径 lFrNum: 左移动帧数 rFrNum: 右移动帧数
		// lFrGap: 左移动帧间隔 rFrGap: 右移动帧间隔 charW: 角色宽度 charH: 角色高度 roleSp: 角色速度
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
		// 设置角色静止动画
	{
		animeL = new Animation(bPathL, lFrNum, lFrGap);
		animeR = new Animation(bPathR, rFrNum, rFrGap);
	}

	void setAttackAnime(tstring bPathL, tstring bPathR, int lFrNum, int rFrNum, int lFrGap, int rFrGap)
		// 设置角色攻击动画
	{
		attackL = new Animation(bPathL, lFrNum, lFrGap);
		attackR = new Animation(bPathR, rFrNum, rFrGap);
	}

	void setMoveAnime(tstring bPathL, tstring bPathR, int lFrNum, int rFrNum, int lFrGap, int rFrGap)
		// 设置角色移动动画
	{
		moveL = new Animation(bPathL, lFrNum, lFrGap);
		moveR = new Animation(bPathR, rFrNum, rFrGap);
	}

	void setEvent(Event* event)
		// 设置事件
	{
		this->event = event;
	}

	void setRolePos(float roleX, float roleY)
		// 设置角色位置
	{
		rolePos.x = roleX;
		rolePos.y = roleY;
	}

	void setRoleSpeed(float roleSp)
		// 设置角色速度
	{
		roleSpeed = roleSp;
	}

	void setRoleSize(int charW, int charH)
		// 设置角色大小
	{
		roleW = charW;
		roleH = charH;
	}

	void setShadow(tstring shadowPath, int shadowWidth, int shadowHeight, float heightRatio = 0.0f)
		// 设置角色阴影
	{
		shadowW = shadowWidth;
		shadowH = shadowHeight;
		shRatio = heightRatio;

		Shadow = new IMAGE();
		loadimage(Shadow, shadowPath.c_str());
	}

	void getEvent()
		// 获取事件
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
		// deltaTime: 主循环每帧时间间隔
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

			rolePos.x += deltaX; // roleSpeed设置过小时会导致精度丢失，导致角色移动不流畅
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
			// 如果没有任何按键按下，则明确重置 deltaX 和 deltaY
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
		// 更新角色状态
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
		// shRatio: 阴影高度比例
	{
		updateState(); // 更新角色状态

		if (Shadow != nullptr && shadowW != -1 && shadowH != -1)
		{
			int ShadowX = static_cast<int>(rolePos.x + (roleW / 2 - shadowW / 2));
			int ShadowY = static_cast<int>(rolePos.y + roleH - shadowH * shRatio);
			putImage_regBack(ShadowX, ShadowY, Shadow);
		}

		switch (currentState)
		{
		case RoleState::Idle:
			moveL->stop(); // 停止播放左移动动画
			moveR->stop(); // 停止播放右移动动画
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
			animeL->stop(); // 停止播放左静止动画
			animeR->stop(); // 停止播放右静止动画
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
	float roleSpeed = 0.0f; // 角色速度
	int roleW = 0; // 角色宽度
	int roleH = 0; // 角色高度
	int shadowW = -1; // 阴影宽度
	int shadowH = -1; // 阴影高度
	float shRatio = 0.0f; // 阴影高度比例

protected:
	enum class RoleState
	{
		Idle = 0,  // 静止
		Moving,    // 移动
		Attacking  // 攻击
	};

	RoleState currentState = RoleState::Idle;

protected:
	fPOINT rolePos = { 0.0f, 0.0f }; // 角色位置

	float deltaX = 0.0f; // 角色横向移动的距离
	float deltaY = 0.0f; // 角色纵向移动的距离

	bool facingLeft = false; // 角色是否面向左
	bool isPressKey[4] = { false, false, false, false }; // 上下左右键是否被按下, 0: 上, 1: 下, 2: 左, 3: 右

protected:
	IMAGE* Shadow = nullptr; // 阴影图片

	Animation* animeL = nullptr; // 左静止动画
	Animation* animeR = nullptr; // 右静止动画

	Animation* attackL = nullptr; // 左攻击动画
	Animation* attackR = nullptr; // 右攻击动画

	Animation* moveL = nullptr; // 左移动动画
	Animation* moveR = nullptr; // 右移动动画
};

class Bullet
{
public:
	fPOINT bulletPos = { 300.0f, 300.0f }; // 子弹位置

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
	const int RADIUS = 10; // 子弹半径
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
		// 设置角色碰撞箱大小, boxW: 宽度 boxH: 高度
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
		// 获取角色碰撞箱大小
	{
		return hittingBox;
	}

	const POINT& getPlayerSize() const
		// 获取角色大小
	{
		return { roleW, roleH };
	}

private:
	bool isAlive = true;

private:
	POINT hittingBox = { 0, 0 }; // 角色碰撞箱大小, x: 宽度 y: 高度
};

class Enemy : public Role
{
public:
	Enemy() = default;

	Enemy
	(
		tstring bPathL, tstring bPathR, // bPathL: 左移动动画帧路径 bPathR: 右移动动画帧路径
		int lFrNum, int rFrNum, // lFrNum: 左移动帧数 rFrNum: 右移动帧数
		int lFrGap, int rFrGap, // lFrGap: 左移动帧间隔 rFrGap: 右移动帧间隔
		int charW, int charH, // charW: 角色宽度 charH: 角色高度
		float roleSp, float roleX = 0.0f, float roleY = 0.0f // roleSp: 角色速度 roleX: 角色X坐标 roleY: 角色Y坐标
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
		// 将子弹等效为一个点，判断敌人是否被击中
	{
		bool isOverlapX = bullet.bulletPos.x >= rolePos.x && bullet.bulletPos.x <= rolePos.x + roleW;
		bool isOverlapY = bullet.bulletPos.y >= rolePos.y && bullet.bulletPos.y <= rolePos.y + roleH;

		return isOverlapX && isOverlapY;
	}

	bool checkPlayerHit(const Player& player) const
		// 将敌人等效为一个点，判断玩家是否被击中
	{
		auto hitBox = player.getHittingBox(); // 获取玩家的碰撞箱大小, x: 宽度 y: 高度

		// getPlayerSize() 获取玩家的大小, x: 宽度 y: 高度
		// hitBoxPosX: 碰撞箱左上角的X坐标 hitBoxPosY: 碰撞箱子左上角的Y坐标
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
		// 设置敌人的存活状态
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
		// player: 玩家角色, deltaTime: 主循环每帧时间间隔 
		// lockDistance: 锁定距离, minMoveThreshold: 最小移动阈值, slowDownDist: 减速距离
	{
		fPOINT playerPos = player.getRolePos();
		float moveX = playerPos.x - rolePos.x;
		float moveY = playerPos.y - rolePos.y;

		float length = sqrt(moveX * moveX + moveY * moveY);

		if (length >= lockDistance)
		{
			float slowDownFactor = length < slowDownDist ? length / slowDownDist : 1.0f; // 减速因子

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
		// 设置敌人的攻击点
	{
		hittingPoint.x = rolePos.x + static_cast<float>(roleW / 2);
		hittingPoint.y = rolePos.y + static_cast<float>(roleH / 2);
	}

	void Draw(float shRatio = 0, float chTolerance = 0.2f)
		// shRatio: 阴影高度比例 chTolerance: 换向容差
	{
		if (Shadow != nullptr && shadowW != -1 && shadowH != -1)
		{
			int ShadowX = static_cast<int>(rolePos.x + (roleW / 2 - shadowW / 2));
			int ShadowY = static_cast<int>(rolePos.y + roleH - shadowH * shRatio);
			putImage_regBack(ShadowX, ShadowY, Shadow);
		}

		if (fabs(deltaX) > chTolerance) // 换向容差 
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
		if (!enemy->isActive())  // 通过 isActive() 判断敌人是否处于可用状态
		{
			enemy->setActiveStatus(true);  // 设置敌人为可用状态
			enemy->reset();  // 重置敌人的位置
			return enemy;
		}
	}
	return nullptr;  // 如果池子里的敌人都在使用中，可以返回空或者扩展池子
}

void tryGenEnemy
(
	vector<Enemy*>& enemyList, vector<Enemy*>& enemyPool,
	int maxEnemyAmount, int deltaTick, int genGap
)
// enemyList: 敌人列表 maxEnemyAmount: 最大敌人数量 deltaTick: 主循环每帧时间间隔 genGap: 生成敌人间隔
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
// 从敌人列表中移除死亡的敌人
// enemyList: 敌人列表, clip: 子弹列表
{
	auto enemy = enemyList.begin();
	while (enemy != enemyList.end())
	{
		(*enemy)->setActiveStatus(clip); // 设置敌人的存活状态
		if (!(*enemy)->isActive())
		{
			enemy = enemyList.erase(enemy); // 从列表中移除敌人
		}
		else
		{
			enemy++; // 指向下一个敌人
		}
	}
	return true;
}

bool removeEnemyFromList(vector<Enemy*>& enemyList, const vector<Bullet>& clip, int& score)
// 从敌人列表中移除死亡的敌人, 并计分
// enemyList: 敌人列表, clip: 子弹列表, score: 分数
{
	auto enemy = enemyList.begin();
	while (enemy != enemyList.end())
	{
		(*enemy)->setActiveStatus(clip); // 设置敌人的存活状态
		if (!(*enemy)->isActive())
		{
			score++;
			enemy = enemyList.erase(enemy); // 从列表中移除敌人
		}
		else
		{
			enemy++; // 指向下一个敌人
		}
	}
	return true;
}

void DrawPlayerScore(int score)
{
	static TCHAR text[64]; // 定义一个静态字符数组
	_stprintf_s(text, _T("当前玩家得分:%d"), score);// 格式化字符串，将得分插入到 text 中

	setbkmode(TRANSPARENT); // 设置背景模式为透明
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}

void bulletOrbitPlayer
(
	vector<Bullet>& bulletList, const Player& player, float deltaTime,
	float rotateVol = 5.0f, float rotateRadius = 80.0f,
	float amplitude = 20.0f, float frequency = 4.0f
)
// 子弹围绕玩家旋转
// bulletList: 子弹列表, player: 玩家角色, deltaTime: 主循环每帧时间间隔
// rotateVol: 旋转速度(rad/s), rotateRadius: 旋转半径, amplitude: 振幅, frequency: 频率
{
	static float bulletAngle = 0; // 第一颗子弹的角度
	float angleGap = 2 * M_PI / bulletList.size(); // 子弹间隔角度

	for (size_t i = 0; i < bulletList.size(); i++)
	{
		float curAngle = bulletAngle + i * angleGap; // 当前子弹角度

		// 利用子弹的极坐标来计算子弹的位置
		// bulletCentralX = playerCentralX + r(theta) * cos(theta)
		// bulletCentralY = playerCentralY + r(theta) * sin(theta)
		float curveFactor = rotateRadius + amplitude * cos(frequency * curAngle); // 曲线因子

		bulletList[i].bulletPos.x
			= player.getRolePos().x + player.getPlayerSize().x / 2 + curveFactor * cos(curAngle);

		bulletList[i].bulletPos.y
			= player.getRolePos().y + player.getPlayerSize().y / 2 + curveFactor * sin(curAngle);
	}

	bulletAngle += rotateVol * deltaTime; // 通过时间间隔归一化旋转速度与子弹角度增量
	bulletAngle = fmod(bulletAngle, 2 * M_PI); // 使用 fmod 来处理角度的循环, 防止子弹角度大于 2 * PI
}
