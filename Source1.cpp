#include <Windows.h>
#include <iostream>
#include <time.h>
#include <thread>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>

using namespace std;

// Hàm thay đổi kích cỡ khung cmd
void resizeConsole(int width, int height)
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, width, height, TRUE);
}

// Hàm tô màu
void textcolor(int x)
{
	HANDLE mau;
	mau = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(mau, x);
}

void EraseCars();
void MoDau();

void FixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX)& ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void GotoXY(int x, int y) {
	static HANDLE  h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}


//Hằng số
#define MAX_CAR 17
#define MAX_CAR_LENGTH 6
#define MAX_SPEED 5

//Biến toàn cục
POINT* *X; //Mảng chứa MAX_CAR xe
POINT Y; // Đại diện người qua đường
POINT Nguoi;
int cnt = 0;//Biến hỗ trợ trong quá trình tăng tốc độ xe di chuyển
int MOVING;//Biến xác định hướng di chuyển của người
int SPEED ;// Tốc độ xe chạy (xem như level)
int HEIGH_CONSOLE = 20, WIDTH_CONSOLE = 60;// Độ rộng và độ cao của màn hình console
bool STATE; // Trạng thái sống/chết của người qua đường
int *TestFinish = new int[WIDTH_CONSOLE];           
int Tocdo;
int Flag;
int count1;
int *SoNguoi = NULL;
int dung = 0;
int Light = 34;

//Hàm khởi tạo dữ liệu mặc định ban đầu
void ResetData() {

	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải
	if (Flag == 0)
	{

		SPEED = 1; // Tốc độ lúc đầu
		Y = { 18, 19 }; // Vị trí lúc đầu của người
	}
	else if (Flag == 1)
	{
		SPEED = Tocdo;
		Y = Nguoi;
		for (int i = 0; i <= count1;i++)
		{
			GotoXY(SoNguoi[i], 1);
			printf("Y");
			TestFinish[SoNguoi[i]] = 1;
		}
	}
	// Tạo mảng xe chạy
	if (X == NULL) {
		X = new POINT*[MAX_CAR];
		for (int i = 0; i < MAX_CAR; i++)
			X[i] = new POINT[MAX_CAR_LENGTH];
		for (int i = 0; i < MAX_CAR; i++)
		{
			int temp = (rand() % (WIDTH_CONSOLE - MAX_CAR_LENGTH)) + 1;
			for (int j = 0; j < MAX_CAR_LENGTH; j++)
			{
				X[i][j].x = temp + j + 1;
				X[i][j].y = 2 + i;
			}
		}
	}

}


void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	GotoXY(x, y); printf("%c", 218);
	for (int i = 1; i < width; i++)printf("%c", 196);        // Vẽ hàng ngang trên
	printf("%c", 191);
	GotoXY(x, height + y);
	printf("%c", 192);
	for (int i = 1; i < width; i++)printf("%c", 196);        // Vẽ hình ngang dưới
	printf("%c", 217);
	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i); printf("%c", 179);                     // Vẽ cột dọc trái
		GotoXY(x + width, i); printf("%c", 179);             // Vẽ cột dọc phải
	}
	GotoXY(curPosX, curPosY);
}

void StartGame() {
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE); // Vẽ màn hình game
	GotoXY(WIDTH_CONSOLE + 12, 3);
	printf("Press 'Q' to back menu");
	GotoXY(WIDTH_CONSOLE + 12, 4);
	printf("Press 'P' to pause game");
	GotoXY(WIDTH_CONSOLE + 12, 5);
	printf("Press 'L' to save game");
	GotoXY(WIDTH_CONSOLE + 12, 6);
	printf("Press 'E' to exit game");
	GotoXY(WIDTH_CONSOLE + 4, 10);
	printf("Traffic Light");
	GotoXY(WIDTH_CONSOLE + 7 ,11);
	textcolor(34);
	printf("|||||");
	textcolor(7);
	STATE = true;//Bắt đầu cho Thread chạy
}

//Hàm dọn dẹp tài nguyên
void GabageCollect()
{
	for (int i = 0; i < MAX_CAR; i++)
	{
		delete[] X[i];
	}
	delete[] X;
	delete[] TestFinish;             //  Kiểm tra kết thúc
	delete[] SoNguoi;       
} 
//Hàm thoát game
void ExitGame(HANDLE t) {
	system("cls");
	TerminateThread(t, 0);
	GabageCollect();

}
//Hàm dừng game
void PauseGame(HANDLE t) {

	SuspendThread(t);
}

//Hàm xử lý khi người đụng xe
void ProcessDead() {

	printf("\a");
	GotoXY(Y.x, Y.y);
	textcolor(10);
	printf("Y");

	Sleep(200);
	GotoXY(Y.x, Y.y);
	printf(" ");

	Sleep(200);
	GotoXY(Y.x, Y.y);
	textcolor(13);
	printf("Y");

	Sleep(200);
	GotoXY(Y.x, Y.y);
	printf(" ");

	Sleep(200);

	textcolor(14);
	GotoXY(10, 9);
	printf(" ___  ___  _____  ___  ___  _ _  ___  ___ "); GotoXY(10, 10);
	printf("| . || .'||     || -_|| . || | || -_||  _|"); GotoXY(10, 11);
	printf("|_  ||__,||_|_|_||___||___| \\_/ |___||_|  "); GotoXY(10, 12);
	printf("|___|                                     "); GotoXY(10, 13);
	textcolor(7);

	STATE = 0;
	GotoXY(0, HEIGH_CONSOLE + 2);
	printf("Do you want to play again ? (Y/N)  ");

}
//Hàm xử lý khi người băng qua đường thành công
void ProcessFinish(POINT& p)
{
	SPEED == MAX_SPEED ? SPEED = 1 : SPEED++;
	p = { 18, 19 }; // Vị trí lúc đầu của người
	MOVING = 'D'; // Ban đầu cho người di chuyển sang phải
}

//Hàm vẽ các toa xe
void DrawCars(char* s)
{
	for (int i = 0; i < MAX_CAR; i++) {
		for (int j = 0; j < MAX_CAR_LENGTH; j++)
		{
			GotoXY(X[i][j].x, X[i][j].y);
			printf(".");
		}
	}
}
//Hàm vẽ người qua đường
void DrawSticker(const POINT& p, char* s) {
	GotoXY(p.x, p.y);
	printf(s);

}

//Hàm kiểm tra xem người qua đường có đụng xe không
bool IsImpact(const POINT& p, int d)
{
	if (d == 1)
	{
		if (TestFinish[Y.x] == 1)
			return true;
		return false;
	}
	else if (d == 19)
		return false;
	for (int i = 0; i < MAX_CAR_LENGTH; i++)
	{
		if (p.x == X[d - 2][i].x && p.y == X[d - 2][i].y) return true;
	}
	return false;
}


void MoveCars() {
	
	for (int i = 1; i < MAX_CAR; i += 2)
	{
		cnt = 0;
		do {
			cnt++;
			for (int j = 0; j < MAX_CAR_LENGTH - 1; j++) {
				X[i][j] = X[i][j + 1];
				
			}
			X[i][MAX_CAR_LENGTH - 1].x + 1 == WIDTH_CONSOLE ? X[i][MAX_CAR_LENGTH - 1].x = 1 : X[i][MAX_CAR_LENGTH - 1].x++;
			// Kiểm tra xem xe có đụng màn hình không

		} while (cnt < SPEED);
	}
	//int a1 = rand() % MAX_CAR;
	for (int i = 0; i < MAX_CAR; i += 2)
	{
		cnt = 0;
		do {
			cnt++;
			for (int j = MAX_CAR_LENGTH - 1; j > 0; j--)
			{
				X[i][j] = X[i][j - 1];
			}
			X[i][0].x - 1 == 0 ? X[i][0].x = WIDTH_CONSOLE - 1 : X[i][0].x--;// Kiểm tra xem xe có đụng màn hình không

		} while (cnt < SPEED);
	}

}


// Hàm xóa xe (xóa có nghĩa là không vẽ)
void EraseCars()
{
	for (int i = 0; i < MAX_CAR; i += 2) {
		cnt = 0;
		do {
			GotoXY(X[i][MAX_CAR_LENGTH - 1 - cnt].x, X[i][MAX_CAR_LENGTH - 1 - cnt].y);
			printf(" ");
			cnt++;

		} while (cnt < SPEED);

	}
	for (int i = 1; i < MAX_CAR; i += 2) {
		cnt = 0;
		do {
			GotoXY(X[i][0 + cnt].x, X[i][0 + cnt].y);
			printf(" ");
			cnt++;

		} while (cnt < SPEED);
	}
}


void MoveRight() {
	if (Y.x < WIDTH_CONSOLE - 1)
	{
		DrawSticker(Y, " ");
		Y.x++;
		DrawSticker(Y, "Y");
	}
}
void MoveLeft() {
	if (Y.x > 1) {
		DrawSticker(Y, " ");
		Y.x--;
		DrawSticker(Y, "Y");

	}

}
void MoveDown() {
	if (Y.y < HEIGH_CONSOLE - 1)
	{
		DrawSticker(Y, " ");
		Y.y++;
		DrawSticker(Y, "Y");
	}

}
void MoveUp() {
	if (Y.y > 1) {
		DrawSticker(Y, " ");
		Y.y--;
		DrawSticker(Y, "Y");

	}

}

void SubThread()
{
	count1 = 0;
	while (1) {
		if (STATE) //Nếu người vẫn còn sống
		{
			switch (MOVING) //Kiểm tra biến moving
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'W':
				MoveUp();
				break;
			case 'S':
				MoveDown();
				break;
			}
			MOVING = ' ';// Tạm khóa không cho di chuyển, chờ nhận phím từ hàm main

			if (dung < 60)
			{

				EraseCars();
				MoveCars();
				DrawCars(".");
			}
			else
			{
				Sleep(50);
				if (dung == 100)
					dung = 0;
			}

			if (dung == 1)
			{
				GotoXY(WIDTH_CONSOLE + 7, 11);
				textcolor(34);
				printf("|||||");
				textcolor(7);
			}
			if (dung == 61)
			{
				GotoXY(WIDTH_CONSOLE + 7, 11);
				textcolor(68);
				printf("|||||");
				textcolor(7);
			}

			if (IsImpact(Y, Y.y))
			{
				ProcessDead(); // Kiểm tra xe có đụng không
			}

			if (Y.y == 1)
			{

				TestFinish[Y.x] = 1;
				ProcessFinish(Y);// Kiểm tra xem về đích chưa
			}
			dung++;
			Sleep(100);//Hàm ngủ theo tốc độ SPEED
		}

	}

}

void SaveGame()
{
	//NHẬP TEN FILE
	char filename[30];
	GotoXY(0, 22);
	printf("Input file to save game :");
	gets(filename);
	strcat(filename, ".txt");

	//LƯU THÔNG TIN VÀO FILE
	FILE* fp;
	fopen_s(&fp, filename, "w");
	if (fp != NULL)
	{
		fwrite(&Y, sizeof(POINT), 2, fp);

		fwrite(&SPEED, sizeof(int), 1, fp);
		for (int i = 1; i < WIDTH_CONSOLE; i++)
		{
			if (TestFinish[i] == 1)
			{
				count1++;
			}
		}
		fwrite(&count1, sizeof(int), 1, fp);
		
		for (int i = 1; i < WIDTH_CONSOLE; i++)
		{
			if (TestFinish[i] == 1)
			{
				fwrite(&i, sizeof(int), 1, fp);

			}
		}
		fclose(fp);
	}
	GotoXY(0, 22);
	textcolor(14);
	printf("               Successful!           \n");
	textcolor(7);
	printf("Press 'D','S','A' or 'W' to continue");
	
	Sleep(2000);
	GotoXY(0, 22);
	printf("                                     \n");
	printf("                                       ");
}

void LoadGame()
{
	system("cls");
	char filename[30];
	printf("\nInput file to load game :");
	gets(filename);
	strcat(filename, ".txt");

	FILE* fp;
	fopen_s(&fp, filename, "r");

	if (fp == NULL)
	{
		printf("\nFile not found");
		exit(0);
	}
	int dem_ = 0;

	//ĐỌC TỌA ĐỘ NGƯỜI TỪ FILE
	fread(&Nguoi, sizeof(POINT), 2, fp);
	
	//ĐÕC GIÁ TRỊ SPEED TỪ FILE
	fread(&Tocdo, sizeof(int), 1, fp);

	//ĐẾM CÓ BAO NHIÊU NGƯỜI VỀ ĐÍCH
	fread(&dem_, sizeof(int), 1, fp);
	SoNguoi = new int[dem_];
	for (int i = 0; i <= dem_; i++)
	{
		//ĐỌC VỊ TRÍ NGƯỜI VỀ ĐÍCH
		fread(&SoNguoi[i], sizeof(int), 1, fp);

	}
	fclose(fp);
}

void GioiThieu()
{
	system("cls");
	textcolor(4);
	FILE*fp = fopen("Cross The Road.txt", "r");
	char ch;
	while (!feof(fp))
	{
		ch = fgetc(fp);
		printf("%c", ch);
	}
	textcolor(7);
	GotoXY(10, 10);
	printf("Game Cross The Road Ver 1.0\n");
	
	GotoXY(0, 12);
	printf("Press 'Q' to back menu game\n");
	printf("Press 'E' to exit game\n");

	int temp;
	temp = _getch();
	if (temp == 'Q' || temp == 'q')
	{
		MoDau();
	}
	else if (temp == 'E' || temp == 'e')
	{
		system("cls");
		exit(0);
	}
}

void HuongDan()
{
	system("cls");
	textcolor(4);
	FILE*fp = fopen("Crossing Road.txt", "r");
	char ch;
	while (!feof(fp))
	{
		ch = fgetc(fp);
		printf("%c", ch);
	}
	textcolor(7);
	GotoXY(10, 10);
	printf("Press 'W' 'A' 'S' 'D' to move \n");

	GotoXY(0, 12);
	printf("Press 'Q' to back menu game\n");
	printf("Press 'E' to exit game\n");

	int temp;
	temp = _getch();
	if (temp == 'Q' || temp == 'q')
	{
		MoDau();
	}
	else if (temp == 'E' || temp == 'e')
		exit(0);
}

void MoDau()
{
	system("cls");
	
	FILE*fp = fopen("Crossing Road.txt", "r");
	char ch;
	textcolor(4);
	while (!feof(fp))
	{
		ch = fgetc(fp);
		printf("%c", ch);
	}
	textcolor(7);
	printf("\n");
	
	GotoXY(50, 11); printf("1. Introduction");
	GotoXY(50, 12); printf("2. New Game");
	GotoXY(50, 13); printf("3. Load Game");
	GotoXY(50, 14); printf("4. Intruction");
	GotoXY(50, 15); printf("5. Exit Game");
	
	char n;
	do{
		n = _getch();
		if (n == 49)
		{
			GioiThieu();
		}

		else if (n == 50)
			Flag = 0;
		else if (n == 51)
		{
			Flag = 1;
			LoadGame();
		}
		else if (n == 52)
		{
			HuongDan();
		}
		else if (n == 53)
		{
			system("cls");
			ExitProcess(0);
		}
	} while (n<49 || n >53);
}

void main()
{
	int temp;
	MoDau();	
	resizeConsole(900, 600);
	FixConsoleWindow();
	srand(time(NULL));
	StartGame();
	thread t1(SubThread);

	while (1)
	{
		temp = toupper(getch());
		if (STATE == 1)
		{

			if (temp == 27 || temp =='E') {
				ExitGame(t1.native_handle());
				exit(0);

			}
			else if (temp == 80) {
				PauseGame(t1.native_handle());
			}
			else if (temp == 'Q')
			{
				system("cls");
				ExitGame(t1.native_handle());
				main();
			}
			else if (temp == 'L')
			{
				PauseGame(t1.native_handle());
				SaveGame();
			}
			else
			{
				ResumeThread((HANDLE)t1.native_handle());
				if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
				{
					MOVING = temp;
				}
			}
		}
		else
		{
			if (temp == 'Y'||temp=='y') StartGame();
			else if (temp == 'N'||temp=='n')
			{
				ExitGame(t1.native_handle());
				exit(0);
			}
		}
	}
}
