#include <iostream>
#include <iomanip>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <chrono>

#define CON_WIDTH 42
#define CON_HEIGHT 20

using namespace std;

enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};
HANDLE g_hout = GetStdHandle(STD_OUTPUT_HANDLE);
inline void carpos(short x, short y)
{
	SetConsoleCursorPosition(g_hout, { x, y });
}

void TextColor(int text_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bg_color << 4) | text_color);
}

void print_time(long long seconds)
{
	int m = seconds / 60;
	int s = seconds % 60;

	if (m > 9) cout << m;
	else cout << '0' << m;
	cout << ':';
	if (s > 9) cout << s;
	else cout << '0' << s;

}

void records_update(int level_number, double speed)
{
	ifstream ifl("records.dat", ios::binary);
	double levels_speed[3] = { 0.0, 0.0, 0.0 };

	if (ifl)
	{
		ifl.read(reinterpret_cast<char*>(levels_speed), sizeof(double) * 3);
		if (levels_speed[level_number] < speed) levels_speed[level_number] = speed;
	}
	else
	{
		levels_speed[level_number] = speed;
	}

	ofstream ofl("records.dat", ios::binary);
	ofl.write(reinterpret_cast<char*>(levels_speed), sizeof(double) * 4);
}

void level_start(int level_number)
{
	system("cls & color 30");

	HDC hdc = GetDC(GetConsoleWindow());

	HBITMAP hbm = (HBITMAP)LoadImageA(0, "body.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObjectA(hbm, sizeof(BITMAP), &bm);
	HDC dc_buf = CreateCompatibleDC(hdc);
	SelectObject(dc_buf, hbm);

	char fname[] = "level_X.dat";
	fname[6] = '0' + level_number;
	ifstream ifl(fname);
	char text[1024];
	int text_pos = 0;
	ifl.getline(text, 1024, '\0');
	int text_len = strlen(text);

	const int width = CON_WIDTH - 2;

	bool game = true, win = false, render = true;
	int ch = 0;
	long long timer = 0, dur;
	auto start = chrono::high_resolution_clock::now();

	while (game)
	{

		if (_kbhit())
		{
			ch = _getch();

			if (ch == text[text_pos])
			{
				render = true;
				text_pos++;

				if (text_pos == text_len -1)
				{
					win = false;
				}

				if (text_pos == text_len)
				{
					win = true;
					game = false;
					records_update(level_number, text_len / (dur / 1000.0));
				}
			}
			else if (ch == 27) break;
			else cout << '\a';
		}


		auto now = chrono::high_resolution_clock::now();
		dur = chrono::duration_cast<chrono::milliseconds>(now - start).count();

		if (dur - timer > 1000)
		{
			render = true;
			timer = dur;
		}
		

		if (render)
		{
			carpos(0, 0);

			auto seconds = timer / 1000;
			cout << endl;
			cout << " Level " << (level_number + 1) << "           ";
			print_time(seconds);
			cout << "       Count: " << (text_pos+1) << endl;
			for (int i = 0; i < 42; i++) cout << "\xC4";

			carpos(1, 5);
			TextColor(Black, DarkGray);
			for (int i = 0; i < width; i++)
			{
				if (i < width / 2)
				{
					if (text_pos + i - width / 2 >= 0) cout << text[text_pos + i - width / 2];
					else cout << ' ';
				}
				else if (i == width / 2)
				{
					TextColor(Blue, Red);
					cout << text[text_pos];
				}
				else
				{
					TextColor(Black, White);
					if (text_pos + i - width / 2 < text_len) cout << text[text_pos + i - width / 2];
					else cout << ' ';
				}

			}

			TextColor(Red, Cyan);
			carpos(width / 2 + 1, 6); cout << '^';
			TextColor(Black, Cyan);

			TextColor(Blue, Cyan);
			carpos(1, 19); cout << "EXIT: Esc";
			TextColor(Black, Cyan);
			carpos(41, 19);
			render = false;
		}

		BitBlt(hdc, 0, 380, bm.bmWidth, bm.bmHeight, dc_buf, 0, 0, SRCCOPY);

	}
	
	render = true;

	while (win)
	{
		if (_kbhit())
		{
			ch = _getch();

			if (ch == 27) break;

			render = true;
		}

		if (render)
		{
			cout.setf(ios::fixed);
			cout.precision(2);

			TextColor(Blue, Cyan);
			carpos(13, 8); cout << "Level " << (level_number+1) << " complete!";
			carpos(14, 9); cout << "Speed: " << text_len / (dur / 1000.0) << " LpS";
			TextColor(Black, Cyan);
			carpos(41, 19);

			render = false;
		}

		BitBlt(hdc, 0, 380, bm.bmWidth, bm.bmHeight, dc_buf, 0, 0, SRCCOPY);

	}
	

	system("cls & color 30");
}

void levels_records()
{
	system("cls & color 30");

	double levels_speed[3] = { 0.0, 0.0, 0.0 };

	ifstream ifl("records.dat", ios::binary);
	if (ifl)
	{
		ifl.read(reinterpret_cast<char*>(levels_speed), sizeof(double) * 3);
	}

	cout.setf(ios::fixed);
	cout.precision(2);

	carpos(0, 5);
	for (int i = 0; i < 3; i++)
	{
		TextColor(Blue, Cyan); 
		cout << "            Level " << (i + 1) << ": ";
		TextColor(Black, Cyan);
		if (levels_speed[i] > 0.0) cout << levels_speed[i] << " LpS\n\n\n";
		else cout << "no data\n\n\n\n";
	}
	cout << "        (LpS \xC4 lettres per second)";

	TextColor(Blue, Cyan);
	carpos(1, 19); cout << "EXIT: Esc";
	TextColor(Black, Cyan);
	carpos(41, 19);

	HDC hdc = GetDC(GetConsoleWindow());

	HBITMAP hbm = (HBITMAP)LoadImageA(0, "header-4-LR.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObjectA(hbm, sizeof(BITMAP), &bm);
	HDC dc_buf = CreateCompatibleDC(hdc);
	SelectObject(dc_buf, hbm);

	while (true)
	{
		if (_kbhit())
		{
			if (_getch() == 27) break;
		}
		BitBlt(hdc, 0, 20, bm.bmWidth, bm.bmHeight, dc_buf, 0, 0, SRCCOPY);

	}

	system("cls & color 30");
}

void level_menu()
{
	system("cls & color 30");


	bool level_menu = true, render = true;
	int switcher = 0;
	int ch;

	while (level_menu)
	{

		// if keyboard hit
		if (_kbhit())
		{
			// get character
			ch = _getch();

			if (ch == 224)
			{
				ch = _getch();

				if (ch == 80) switcher = (switcher + 1) % 4;
				else if (ch == 72) switcher = (switcher + 3) % 4;
			}
			else if (ch == 13)
			{
				if (switcher == 0) level_start(0);
				else if (switcher == 1) level_start(1);
				else if (switcher == 2) level_start(2);
				break;
			}

			render = true;
		}

		// render frame
		if (render)
		{

			// print menu buttons
			if (switcher == 0) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			carpos(14, 2); cout << "           ";
			carpos(14, 3); cout << "  LEVEL 1  ";
			carpos(14, 4); cout << "           ";

			if (switcher == 1) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			carpos(14, 6); cout << "           ";
			carpos(14, 7); cout << "  LEVEL 2  ";
			carpos(14, 8); cout << "           ";

			if (switcher == 2) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			carpos(14, 10); cout << "           ";
			carpos(14, 11); cout << "  LEVEL 3  ";
			carpos(14, 12); cout << "           ";

			if (switcher == 3) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			carpos(14, 14); cout << "           ";
			carpos(14, 15); cout << "   <BACK   ";
			carpos(14, 16); cout << "           ";


			


			render = false;
		}
		carpos(41, 19);
	}
	system("cls & color 30");
}

int main()
{
	// set up
	CONSOLE_FONT_INFOEX cfi = {};

	cfi.dwFontSize = { 15, 25 };
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	cfi.FontWeight = FW_BOLD;
	wcscpy_s(cfi.FaceName, L"Lucida console");
	SetCurrentConsoleFontEx(g_hout, false, &cfi);

	system("mode con cols=42 lines=20");
	system("title Keyboard Trainer");
	system("color 30");

	bool menu = true, render = true;
	int switcher = 0;
	int ch;
	

	/*
	Menu:
	-Train
	-Best results
	-Exit

	Train
	-Level 1
	-Level 2
	-Level 3
	-Level 4
	-Level 5

	
	*/

	HDC hdc = GetDC(GetConsoleWindow());

	HBITMAP hbm = (HBITMAP)LoadImageA(0, "header-4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObjectA(hbm, sizeof(BITMAP), &bm);
	HDC dc_buf = CreateCompatibleDC(hdc);
	SelectObject(dc_buf, hbm);

	



	while (menu)
	{

		// if keyboard hit
		if (_kbhit())
		{
			// get character
			ch = _getch();

			if (ch == 224)
			{
				ch = _getch();

				if (ch == 80) switcher = (switcher + 1) % 3;
				else if (ch == 72) switcher = (switcher + 2) % 3;
			}
			else if (ch == 13)
			{
				if (switcher == 0) level_menu();
				else if (switcher == 1) levels_records();
				else if (switcher == 2)
				{
					system("color 0F & cls");
					return 0;
				}
			}

			render = true;
		}

		// render frame
		if (render)
		{
			


			
			SetConsoleCursorPosition(g_hout, { 0, 5 });

			//cout << "\n\n\n\n";

			// set carriage to (0, 0)

			// print menu buttons
			if (switcher == 0) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			SetConsoleCursorPosition(g_hout, { 14, 5 }); cout << "           ";
			SetConsoleCursorPosition(g_hout, { 14, 6 }); cout << "   TRAIN   ";
			SetConsoleCursorPosition(g_hout, { 14, 7 }); cout << "           ";
			
			cout << endl;
			if (switcher == 1) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			SetConsoleCursorPosition(g_hout, { 14, 9 }); cout << "           ";
			SetConsoleCursorPosition(g_hout, { 14, 10 }); cout << "  RECORDS  ";
			SetConsoleCursorPosition(g_hout, { 14, 11 }); cout << "           ";

			cout << endl;
			if (switcher == 2) TextColor(LightMagenta, Blue);
			else TextColor(Black, LightBlue);
			SetConsoleCursorPosition(g_hout, { 14, 13 }); cout << "           ";
			SetConsoleCursorPosition(g_hout, { 14, 14 }); cout << "   CLOSE   ";
			SetConsoleCursorPosition(g_hout, { 14, 15 }); cout << "           ";
			carpos(41, 19);


			render = false;
		}
		BitBlt(hdc, 0, 30, bm.bmWidth, bm.bmHeight, dc_buf, 0, 0, SRCCOPY);


		
	}

}