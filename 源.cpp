#include<iostream>
#include<graphics.h>
#include<string>
#include<vector>
#include<cstdlib>

int idx_current_anim = 0;

const


#pragma comment(lib,"MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")

inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		interval_ms = interval;
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	~Animation()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i];
	}
	void play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}

private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};

//Animation anim_sunflower(_T("SunFlower/SunFlower_%d.png"),SUNFLOWER_ANIM_NUM,144);


/*void LoadAnimation()
{
	for (size_t i = 0; i < SUNFLOWER_ANIM_NUM; i++)
	{
		std::wstring path = L"SunFlower/SunFlower_" + std::to_wstring(i) + L".png";
		loadimage(&img_sunflower[i], path.c_str());
	}
}*/
class Player {
public:
	Player() {
		loadimage(&img_shadow, _T("SunFlower/shadow_sunflower.png"), FLOWER_WIDTH);
		anim_sunflower = new Animation(_T("SunFlower/SunFlower_%d.png"), SUNFLOWER_ANIM_NUM, 144);
	}
	~Player()
	{
		delete anim_sunflower;
	}
	const POINT& GetPosition()const
	{
		return flower_pos;
	}
	void ProcessEvent(const ExMessage& msg)
	{
		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			}
		}
		else if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
		}


	}
	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			flower_pos.x += (int)(normalized_x * PLAYER_SPEED);
			flower_pos.y += (int)(normalized_y * PLAYER_SPEED);
		}
		if (flower_pos.x < 0) flower_pos.x = 0;
		if (flower_pos.y < 0) flower_pos.y = 0;
		if (flower_pos.x + FLOWER_WIDTH > 1400) flower_pos.x = 1400 - FLOWER_WIDTH;
		if (flower_pos.y + FLOWER_HEIGHT > 600) flower_pos.y = 600 - FLOWER_HEIGHT;
	}
	void Draw(int delta)
	{
		int pos_shadow_x = flower_pos.x;
		int pos_shadow_y = flower_pos.y + FLOWER_HEIGHT - 20;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		anim_sunflower->play(flower_pos.x, flower_pos.y, delta);
	}
	POINT flower_pos = { 500,500 };
	const int FLOWER_WIDTH = 73;
	const int FLOWER_HEIGHT = 74;
private:
	const double PLAYER_SPEED = 7;
	const int SHADOW_WIDTH = FLOWER_WIDTH;
private:
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_right = false;
	bool is_move_left = false;

	IMAGE img_shadow;
	const int SUNFLOWER_ANIM_NUM = 18;
	Animation* anim_sunflower;
};

Player Sunflower;
class Bullet {
public:
	POINT position = { 0,0 };
	const int hurt_per_bullet = 30;
public:
	Bullet(const Player& player)
	{
		anim_bullet = new Animation(_T("Bullet/SnowBullet%d.png"), BULLET_ANIM_NUM, 144);
		position = player.GetPosition();
	}
	~Bullet()
	{
		delete anim_bullet;
	}
	void Move()
	{
		position.x += (int)BULLET_SPEED;
	}
	void Draw(int delta)
	{
		anim_bullet->play(position.x, position.y, delta);
	}
	void hit()
	{
		hit_enemy = true;
	}
	bool CheckAilve()
	{
		return hit_enemy;
	}
private:
	Animation* anim_bullet;
	Player* player;
	const int BULLET_ANIM_NUM = 4;
	const double BULLET_SPEED = 10;
	bool hit_enemy = false;
};
class Enemy {
public:
	Enemy() {
		loadimage(&img_shadow, _T("SunFlower/shadow_sunflower.png"));
		anim_zombie = new Animation(_T("NormalZombie/Zombie/Zombie_%d.png"), ZOMBIE_ANIM_NUM, 144);
		zombie_pos.x = 1000;

		zombie_pos.y = (rand() % 60) / 8 * 60 + 20;
	}
	~Enemy()
	{
		delete anim_zombie;
	}
	bool CheckPlayerCollision(const Player& player)
	{
		POINT check_position = { zombie_pos.x + 100,zombie_pos.y + 82 };
		POINT player_postion = player.GetPosition();
		bool crush_x = check_position.x >= player_postion.x && check_position.x <= player_postion.x + player.FLOWER_WIDTH;
		bool crush_y = check_position.y >= player_postion.y && check_position.y <= player_postion.y + player.FLOWER_HEIGHT;
		return crush_x && crush_y;
	}
	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool crush_x = bullet.position.x + 13 >= zombie_pos.x + 70 && bullet.position.x + 13 <= zombie_pos.x + ZOMBIE_WIDTH - 20;
		bool crush_y = bullet.position.y + 13 >= zombie_pos.y + 30 && bullet.position.y + 13 <= zombie_pos.y + ZOMBIE_HEIGHT - 20;
		return crush_x && crush_y;
	}
	void Move()
	{
		zombie_pos.x -= (int)ZOMBIE_SPEED;
	}
	void Draw(int delta)
	{
		int pos_shadow_x = zombie_pos.x + 91;
		int pos_shadow_y = zombie_pos.y + ZOMBIE_HEIGHT - 25;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		anim_zombie->play(zombie_pos.x, zombie_pos.y, delta);
	}
	void Hurt(const Bullet& bullet)
	{
		live -= bullet.hurt_per_bullet;
	}
	bool CheckAilve()
	{
		if (live <= 0)
			return false;
		return true;
	}
	POINT zombie_pos = { 500,500 };
private:
	const int ZOMBIE_WIDTH = 166;
	const int ZOMBIE_HEIGHT = 144;
	const double ZOMBIE_SPEED = 4;
	const int SHADOW_WIDTH = ZOMBIE_WIDTH;
private:
	IMAGE img_shadow;
	const int ZOMBIE_ANIM_NUM = 22;
	int live = 100;
	Animation* anim_zombie;
};

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int interval = 10;
	static int counter = 0;
	if ((++counter) % interval == 0)
		enemy_list.push_back(new Enemy());
}
void TryGenerateBullet(std::vector<Bullet*>& bullet_list)
{
	bullet_list.push_back(new Bullet(Sunflower));
}
void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("当前玩家得分：%d"), score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(0, 0, 0));
	settextstyle(30, 0, _T("Consolas"));
	outtextxy(10, 10, text);
}
int shoot_last_time = 0;
bool WhetherGenerateBullet(int interval)
{
	
	int shoot_this_time = GetTickCount();
	int delta_time = shoot_this_time - shoot_last_time;
	if (delta_time > interval)
	{
		shoot_last_time = shoot_this_time;
		return true;
	}
	return false;
}
int main()
{
	int score = 0;
	mciSendString(_T("open mus/bk_bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	initgraph(1400, 600);
	srand((unsigned)time(NULL));
	bool running = true;
	IMAGE img_background;
	loadimage(&img_background, _T("Game/background.bmp"));
	BeginBatchDraw();
	ExMessage msg;
	DWORD bullet_time = 0;
	//LoadAnimation();
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet*> bullet_list;

	while (running)
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			Sunflower.ProcessEvent(msg);
			if (msg.vkcode == VK_RETURN && msg.message == WM_KEYDOWN)
			{
				if (WhetherGenerateBullet(500))
				{
					TryGenerateBullet(bullet_list);
					break;
				}
			}
		}
		Sunflower.Move();
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move();
		for (Bullet* bullet : bullet_list)
			bullet->Move();
		for (Enemy* enemy : enemy_list)
			if (enemy->CheckPlayerCollision(Sunflower))
			{
				mciSendString(_T("close bgm"), NULL, 0, NULL);
				static TCHAR text[64];
				_stprintf_s(text, _T("最终得分：%d"), score);
				MessageBox(GetHWnd(), _T("菜就多练！"), text, MB_OK);
				running = false;
				break;
			}
		for (Enemy* enemy : enemy_list)
		{
			for (Bullet* bullet : bullet_list)
				if (enemy->CheckBulletCollision(*bullet))
				{
					enemy->Hurt(*bullet);

					bullet->hit();
				}
		}
		for (size_t i = 0; i < enemy_list.size(); i++)
		{
			Enemy* enemy = enemy_list[i];
			if (!enemy->CheckAilve()||enemy->zombie_pos.x<-200)
			{
				score++;
				std::swap(enemy_list[i], enemy_list.back());
				enemy_list.pop_back();
				delete enemy;
			}
		}
		for (size_t i = 0; i < bullet_list.size(); i++)
		{
			Bullet* bullet = bullet_list[i];
			if (bullet->CheckAilve()||bullet->position.x>1400)
			{
				std::swap(bullet_list[i], bullet_list.back());
				bullet_list.pop_back();
				delete bullet;
			}
		}
		cleardevice();

		putimage_alpha(0, 0, &img_background);
		//putimage_alpha(flower_pos.x,flower_pos.y, &img_sunflower[idx_current_anim]);
		for (Enemy* enemy : enemy_list)
			enemy->Draw(700);
		for (Bullet* bullet : bullet_list)
			bullet->Draw(500);
		Sunflower.Draw(150);
		DrawPlayerScore(score);
		FlushBatchDraw();
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 30)
		{
			Sleep(30 - delta_time);
		}

	}
	EndBatchDraw();
	return 0;
}