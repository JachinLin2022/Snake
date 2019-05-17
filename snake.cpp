#include <iostream>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
using namespace std;
#define RESET "\033[0m"
#define RED "\033[31m" /* Red */
int snake_length = 1,border[200][200];
int food_num = 0, food_left = 0,level=1;
int p_num = 0,score=0,dead_num=0,max_score[4];
struct Settings//游戏设置
{
	short screen_width = 50;
	short screen_height = 20;
	int speed = 200;
}settings;
const HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
void gotoxy(const HANDLE hout, const int X, const int Y)
{
	COORD coord;
	coord.X = X;
	coord.Y = Y;
	SetConsoleCursorPosition(hout, coord);
}
void showxy(const HANDLE hout, const int X, const int Y,char a)
{
	gotoxy(hout,X, Y);
	cout << a;
}
void showxycolor(const HANDLE hout, const int X, const int Y, char a)
{
	gotoxy(hout, X, Y);
	cout <<RED<<a<<RESET;
}
class Screen
{
public:
	Screen(int a,int b)
	{
		width = a;
		height = b;
	}
	void init_border()
	{
		for (int i = 1; i < height; i++)
		{
			border[0][i] = 1;
			border[width][i] = 1;
		}
		for (int i = 0; i <= width; i++)
		{
			border[i][0] = 1;
		}		
		for (int i = 0; i <= width; i++)
		{
			border[i][height] = 1;
		}	
	}
	void show_border()
	{
		for (int x = 0; x <= width; x++)
			for (int y = 0; y <= height; y++)
			{
				if (border[x][y] == 1)
				{
					if (x == 0 || x == width || y == 0 || y == height)
						showxy(hout, x, y, '*');
					else showxycolor(hout, x, y, '*');
				}
				if (border[x][y] == 2)
					showxycolor(hout, x, y, '$');
				if (border[x][y] == 3)
					showxy(hout, x, y, ' ');
			}		
	}
private:
	short height;
	short width;
};
class Food
{
public:
	int x;
	int y;
	int eaten;
	Food()
	{
		x = 0;
		y = 0;
		eaten = 0;
		srand((unsigned int)(time(0)));
	}
	void yield()
	{
		x = rand() % (settings.screen_width-1)+1;
		y = rand() % (settings.screen_height-1)+1;
		eaten = 0;
	}
	void show()
	{
		showxy(hout, x, y,'$');
	}
};
class Snake
{
public:
	int x;
	int y;
	char direction;
	Snake()
	{
		x = 0;
		y = 0;
		direction = 'u';
		srand((unsigned int)(time(0)));
	}
	void init()
	{
		x = rand() % (settings.screen_width - 1) + 1;
		y = rand() % (settings.screen_height - 1) + 1;
		direction = 'u';
	}
	void show()
	{
		showxy(hout, x, y, '0');
	}
	void move()
	{
		showxy(hout, x, y, ' ');
		if (direction == 'u') y--;
		if (direction == 'd') y++;
		if (direction == 'l') x--;
		if (direction == 'r') x++;
		showxy(hout, x, y, '0');
	}
};
void menu()//打印菜单
{
	for (int i = 0; i < 50; i++)
		cout << '*';
	cout << endl;
	for (int i = 1; i <= 6; i++)
	{
		switch (i)
		{
		case 1:cout << "                " << "1.入门版" << endl; break;
		case 2:cout << "                " << "2.进阶版" << endl; break;
		case 3:cout << "                " << "3.高级版" << endl; break;
		case 4:cout << "                " << "4.放飞版" << endl; break;
		case 5:cout << "                " << "5.保存结果（内置于游戏中）" << endl; break;
		case 6:cout << "                " << "6.退出游戏" << endl; break;
		}
	}
	for (int i = 0; i < 50; i++)
		cout << '*';
} 
char check_key_event(char dir)//检测键盘活动
{
	char n;
	if (_kbhit())
	{
		if (n = _getch())
		{
			n = _getch();
			switch (n)
			{
			case 72:dir = 'u'; break;
			case 80:dir = 'd'; break;
			case 75:dir = 'l'; break;
			case 77:dir = 'r'; break;
			}
		}
	}	
	return dir;
}
void yield_food(Food food[])//生成食物
{
	int n = rand() % 5 + 1;
	for (int i = 0; i < n; i++)
	{
		food[i].yield();
	}
	food_num = n;
	food_left = n;
}
void show_food(Food food[])//显示食物
{
	for (int i = 0; i < food_num; i++)
	{
		if (food[i].eaten == 0)
			food[i].show();
	}	
}
int check_fail(Snake snake[],int flag=0)//检测游戏失败
{
	if (border[snake[0].x][snake[0].y] == 1)
	{
		for (int i = 1; i < snake_length; i++)
			border[snake[i].x][snake[i].y] = 1 + flag;
		switch (snake[snake_length - 1].direction)
		{
		case 'u':border[snake[snake_length - 1].x][snake[snake_length - 1].y + 1] = 1 + flag; break;
		case 'd':border[snake[snake_length - 1].x][snake[snake_length - 1].y - 1] = 1 + flag; break;
		case 'l':border[snake[snake_length - 1].x + 1][snake[snake_length - 1].y] = 1 + flag; break;
		case 'r':border[snake[snake_length - 1].x - 1][snake[snake_length - 1].y] = 1 + flag; break;
		}
		return 1;
	}
	for (int i = 1; i < snake_length; i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			for (int i = 1; i < snake_length; i++)
				border[snake[i].x][snake[i].y] = 1+ flag;
			switch (snake[snake_length - 1].direction)
			{
			case 'u':border[snake[snake_length - 1].x][snake[snake_length - 1].y + 1] = 1+ flag; break;
			case 'd':border[snake[snake_length - 1].x][snake[snake_length - 1].y - 1] = 1+ flag; break;
			case 'l':border[snake[snake_length - 1].x + 1][snake[snake_length - 1].y] = 1+ flag; break;
			case 'r':border[snake[snake_length - 1].x - 1][snake[snake_length - 1].y] = 1+ flag; break;
			}
			return 1;
		}
			
	}
	return 0;
}
int check_eat(Snake head,Food food[],int food_num)//检测蛇头是否吃到
{
	for (int i = 0; i <= food_num-1; i++)
	{
		if (head.x == food[i].x&& head.y == food[i].y&&food[i].eaten==0)
		{
			food[i].eaten = 1;
			score += 2;
			snake_length++;
			food_left--;
			return 1;
		}
	}
	if (border[head.x][head.y] == 2)
	{
		border[head.x][head.y] = 0;
		score += 1;
		snake_length++;
		return 1;
	}
	return 0;
}
void add_body(Snake snake[])//添加蛇身
{
	switch (snake[snake_length - 2].direction)
	{
	case 'u':
		{
		snake[snake_length - 1].x = snake[snake_length - 2].x;
		snake[snake_length - 1].y = snake[snake_length - 2].y+1;
		break;
		}
	case 'd':
		{
		snake[snake_length - 1].x = snake[snake_length - 2].x;
		snake[snake_length - 1].y = snake[snake_length - 2].y - 1;
		break;
		}
	case 'l':
		{
		snake[snake_length - 1].x = snake[snake_length - 2].x+1;
		snake[snake_length - 1].y = snake[snake_length - 2].y;
		break;
		}
	case 'r':
		{
		snake[snake_length - 1].x = snake[snake_length - 2].x-1;
		snake[snake_length - 1].y = snake[snake_length - 2].y;
		break;
		}
	}
	snake[snake_length - 1].direction = snake[snake_length - 2].direction;
}
void move_snake(Snake snake[],Snake change_poi[])//移动蛇
{
	for (int i = 1; i <= snake_length - 1; i++)
	{
		for(int j=0;j<p_num;j++)
			if (snake[i].x == change_poi[j].x && snake[i].y == change_poi[j].y&&change_poi[j].x!=0)
			{
				snake[i].direction = change_poi[j].direction;
				if (i == snake_length - 1)
				{
					for (int j = 1; j < p_num; j++)
						change_poi[j - 1] = change_poi[j];
					p_num--;
				}
				break;
			}
		snake[i].move();
	}
}
void change_direction(Snake snake[],char dir,Snake change_poi[])//记录拐点并改变蛇头方向
{
	if ((snake[0].direction) == dir) return;
	if (snake[0].direction == 'u' && dir == 'd') return;
	if (snake[0].direction == 'd' && dir == 'u') return;
	if (snake[0].direction == 'l' && dir == 'r') return;
	if (snake[0].direction == 'r' && dir == 'l') return;
	if (snake_length != 1)
	{
		change_poi[p_num].x = snake[0].x;
		change_poi[p_num].y = snake[0].y;
		change_poi[p_num].direction = dir;
		p_num++;
	}
	snake[0].direction = dir;
}
void showinf(Settings settings, int version, int flag = 0)//显示信息
{
	gotoxy(hout, settings.screen_width + 15, 0);
	cout <<snake_length<<"   ";
	gotoxy(hout, settings.screen_width + 15, 1);
	cout << score;
	if (version == 1)
	{
		gotoxy(hout, settings.screen_width+15, 2);
		cout<<max_score[0]<<"    ";
	}
	else if (version == 2)
	{
		gotoxy(hout, settings.screen_width+15, 2);
		cout << max_score[1] << "    ";
	}
	else if (version==3)
	{
		gotoxy(hout, settings.screen_width + 15, 2);
		cout << max_score[2] << "      ";
	}
	else
	{
		gotoxy(hout, settings.screen_width + 15, 2);
		cout << max_score[3] << "      ";

	}
	gotoxy(hout, 0, settings.screen_height + 1);
	cout <<"食物总数为："<<food_num << endl <<"剩余食物为："<<food_left;
	if (flag)
	{
		gotoxy(hout, settings.screen_width + 2, 3);
		cout << "生命值：";
		for (int i = 1; i <= 5 - dead_num; i++)
			cout << "* ";
		cout << "    ";
		gotoxy(hout, settings.screen_width + 2, 4);
		cout << "Level：" << level;
	}
}
void init()//初始化数据
{
	snake_length = 1;
	food_num = 0;
	food_left = 0;
	p_num = 0;
	score = 0;
	dead_num = 0;
	memset(border, 0, sizeof(border));
	level = 1;
}
void check_food_body_collision(Snake snake[], Food food[])//检测食物是否生成在蛇身上
{
	for (int i = 0; i < food_num; i++)
	{
		for (int j = 0; j < snake_length; j++)
		{
			if (food[i].x == snake[j].x && food[i].y == snake[j].y)
			{
				food[i].eaten = 1;
				food_left--;
			}
		}
	}
}
void check_food_dead_body_collision(Food food[])//检测食物与尸体的冲突
{
	for (int i = 0; i < food_num; i++)
		if (border[food[i].x][food[i].y] == 1)
		{
			food[i].eaten = 1;
			food_left--;
		}	
}
int check_head_space(Snake snake[])//检测蛇头空间
{
	if (border[snake[0].x][snake[0].y] == 1)
		return 0;
	return 1;
}
int game_over()//检测游戏是否结束
{
	int space = 0;
	for (int x = 0; x <= settings.screen_width; x++)
		for (int y = 0; y <= settings.screen_height; y++)
			if (border[x][y] == 0)
				space++;
	if (space <= 1) return 1;
	if (dead_num == 5) return 1;
	return 0;
}
void new_snake(Snake snake[])//生成新蛇
{
	snake_length = 1;
	p_num = 0;
	while (1)
	{
		snake[0].init();//初始化蛇头
		if (check_head_space(snake)) break;
	}
}
char star(Snake snake[],char dir)
{
	char n;
	while (1)
	{
		if (_kbhit())
		{
			if (n = _getch())
			{
				n = _getch();
				switch (n)
				{
				case 72:dir = 'u'; break;
				case 80:dir = 'd'; break;
				case 75:dir = 'l'; break;
				case 77:dir = 'r'; break;
				}
			}
			snake[0].direction = dir;
			return dir;
		}
	}
}
void keep_record(int version)
{
	gotoxy(hout, 0, settings.screen_height+3);
	cout << "是否保存游戏结果？(y/n)：";
	char ans;
	cin >> ans;
	if (ans == 'n') return;
	ofstream outfile;
	outfile.open("record.txt", ios::app);
	if (!outfile)
	{
		cout << "文件打开失败" << endl;
		return;
	}
	switch (version)
	{
	case 1:outfile << "版本：1.入门版 用户名：root 得分：" << score << endl; break;
	case 2:outfile << "版本：2.进阶版 用户名：root 得分：" << score << endl; break;
	case 3:outfile << "版本：3.高级版 用户名：root 得分：" << score << endl; break;
	case 4:outfile << "版本：4.放飞版 用户名：root 得分：" << score << endl; break;
	}
}
void yield_wall()
{
	int x, y;
	int num = rand() % 5;
	for (int i = 0; i < num; i++)
	{
		x = rand() % (settings.screen_width - 1) + 1;
		y = rand() % (settings.screen_height - 1) + 1;
		border[x][y] = 1;
	}
}
void instruction()
{
	cout << "------------------------------------------------------" << endl;
	cout << "放飞版说明书：" << endl;
	cout << "右侧实时显示了贪吃蛇当前的生命值，生命值耗尽游戏结束" << endl;
	cout << "每吃到4个食物，贪吃蛇升级" << endl;
	cout << "贪吃蛇升级后，蛇的移动速度将变快，并随即生成障碍物" << endl;
	cout << "------------------------------------------------------" << endl;
	cout << "请按方向键开始游戏" << endl;
	while (1)	
		if (_kbhit()) break;
}
void version1()//入门版
{
	system("cls");
	init();
	Settings settings;
	Screen screen(settings.screen_width,settings.screen_height);
	screen.init_border();
	screen.show_border();
	Snake snake[100],change_poi[50];
	snake[0].init();//初始化蛇头
	Food food[5];
	snake[0].show();
	char dir = 'u';
	gotoxy(hout, settings.screen_width + 2, 0);
	cout << "蛇的长度为：";
	gotoxy(hout, settings.screen_width + 2, 1);
	cout << "你的得分为:";
	gotoxy(hout, settings.screen_width + 2, 2);
	cout << "最高得分为:";
	dir=star(snake,dir);
	while (1)
	{
		showinf(settings,1);
		while(food_left == 0)
		{
			yield_food(food);
			check_food_body_collision(snake, food);
			show_food(food);
		}
		dir=check_key_event(dir);
		change_direction(snake, dir,change_poi);
		snake[0].move();
		move_snake(snake,change_poi); 
		if (check_fail(snake))
		{
			if (score > max_score[0]) max_score[0] = score;
			keep_record(1);
			return;
		}		
		if (check_eat(snake[0],food,food_num))
			add_body(snake);
		Sleep(settings.speed);
	}
}
void version2()//进阶版
{
	system("cls");
	init();
	gotoxy(hout, settings.screen_width + 2, 0);
	cout << "蛇的长度为：";
	gotoxy(hout, settings.screen_width + 2, 1);
	cout << "你的得分为:";
	gotoxy(hout, settings.screen_width + 2, 2);
	cout << "最高得分为:";
	char dir = 'u';
	Settings settings;
	Screen screen(settings.screen_width, settings.screen_height);
	screen.init_border();
	screen.show_border();
	Food food[5];
	Snake snake[100], change_poi[50];
	snake[0].init();//初始化蛇头
	snake[0].show();
	dir = star(snake, dir);
	while (1)
	{
		showinf(settings,2);
		if (check_eat(snake[0], food, food_num))
			add_body(snake);
		while (food_left == 0)
		{
			yield_food(food);
			check_food_body_collision(snake, food);
			check_food_dead_body_collision(food);
			show_food(food);
			if (game_over())
			{
				if (score > max_score[1]) max_score[1] = score;
				keep_record(2);
				return;
			}
		}
		dir = check_key_event(dir);
		change_direction(snake, dir, change_poi);
		snake[0].move();
		move_snake(snake, change_poi);
		if (check_fail(snake))
		{
			screen.show_border();
			if (game_over())
			{
				if (score > max_score[1]) max_score[1] = score;
				keep_record(2);
				return;
			}
			new_snake(snake);
			snake[0].show(); dir = star(snake, dir);
			continue;
		}
		Sleep(settings.speed);
	}
}
void version3()
{
	system("cls");
	init();
	gotoxy(hout, settings.screen_width + 2, 0);
	cout << "蛇的长度为：";
	gotoxy(hout, settings.screen_width + 2, 1);
	cout << "你的得分为:";
	gotoxy(hout, settings.screen_width + 2, 2);
	cout << "最高得分为:";
	char dir = 'u';
	Settings settings;
	Screen screen(settings.screen_width, settings.screen_height);
	screen.init_border();
	screen.show_border();
	Food food[5],snake_food[50];
	Snake snake[100], change_poi[50];
	snake[0].init();//初始化蛇头
	snake[0].show();
	dir = star(snake, dir);
	while (1)
	{
		showinf(settings,3);
		if (check_eat(snake[0], food, food_num))
			add_body(snake);
		while (food_left == 0)
		{
			yield_food(food);
			check_food_body_collision(snake, food);
			check_food_dead_body_collision(food);
			show_food(food);
			if (game_over())
			{
				if (score > max_score[2]) max_score[2] = score;
				keep_record(3);
				return;
			}
		}
		dir = check_key_event(dir);
		change_direction(snake, dir, change_poi);
		snake[0].move();
		move_snake(snake, change_poi);
		if (check_fail(snake,1))
		{
			dead_num++;
			screen.show_border();
			if (game_over())
			{
				if (score > max_score[2]) max_score[2] = score;
				keep_record(3);
				return;
			}
			new_snake(snake);
			snake[0].show(); dir = star(snake, dir);
			continue;
		}
		Sleep(settings.speed);
	}
}
void version4()//放飞版
{
	system("cls");
	instruction();
	system("cls");
	init();
	Settings settings;
	Screen screen(settings.screen_width, settings.screen_height);
	screen.init_border();
	screen.show_border();
	Snake snake[100], change_poi[50];
	snake[0].init();//初始化蛇头
	Food food[5];
	snake[0].show();
	char dir = 'u';
	gotoxy(hout, settings.screen_width + 2, 0);
	cout << "蛇的长度为：";
	gotoxy(hout, settings.screen_width + 2, 1);
	cout << "你的得分为:";
	gotoxy(hout, settings.screen_width + 2, 2);
	cout << "最高得分为:";
	dir = star(snake, dir);
	while (1)
	{
		showinf(settings, 4, 1);
		while (food_left == 0)
		{
			yield_food(food);
			check_food_body_collision(snake, food);
			check_food_dead_body_collision(food);
			show_food(food);
		}
		dir = check_key_event(dir);
		change_direction(snake, dir, change_poi);
		snake[0].move();
		move_snake(snake, change_poi);
		if (check_fail(snake,2))
		{
			dead_num++;
			showinf(settings, 4, 1);
			screen.show_border();
			if (game_over())
			{
				if (score > max_score[3]) max_score[3] = score;
				keep_record(4);
				return;
			}
			new_snake(snake);
			snake[0].show(); dir = star(snake, dir);
			continue;
		}
		if (check_eat(snake[0], food, food_num))
		{
			add_body(snake);
			if (snake_length % 4 == 0)
			{
				level++;
				if (settings.speed >= 100)	settings.speed -= 25;
				else settings.speed -= 2;
				yield_wall();
				check_food_dead_body_collision(food);
				screen.show_border();
			}
		}
		Sleep(settings.speed);
	}
}
void read_record()
{
	ifstream infile;
	infile.open("record.txt");
	if (!infile) return;
	string temp;
	while(!infile.eof())
	{
		char sco[10] = { 0 };
		getline(infile, temp);
		if (temp[0] == 0) break;
		for (unsigned int j = 34; j < (temp.length()); j++)
			sco[j - 34] = temp[j];
		switch (temp[6])
		{
		case '1':
			if (max_score[0] < atoi(sco)) max_score[0] = atoi(sco);
			break;
		case '2':
			if (max_score[1] < atoi(sco)) max_score[1] = atoi(sco);
			break;
		case '3':
			if (max_score[2] < atoi(sco)) max_score[2] = atoi(sco);
			break;
		case '4':
			if (max_score[3] < atoi(sco)) max_score[3] = atoi(sco);
			break;
		}
	}
	infile.close();
}
int main()
{
	read_record();
	menu();//打印菜单
	srand((unsigned int)(time(0)));
	while (1)
	{	
		cout << endl << "请输入游戏版本（1-5）：";
		int version;
		cin >> version;
		switch (version)
		{
		case 1:version1(); system("cls"); menu(); break;
		case 2:version2(); system("cls"); menu(); break;
		case 3:version3(); system("cls"); menu(); break;
		case 4:version4(); system("cls"); menu(); break;
		case 6:return 0;
		}
	}
}