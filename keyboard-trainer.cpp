#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <chrono>
#pragma comment(lib, "winmm.lib")

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

HANDLE g_hout = NULL;
HWND g_hwnd = NULL;
HDC g_hdc = NULL;
HDC g_dc_buf = NULL;

inline void cr_pos(SHORT x, SHORT y)
{
	SetConsoleCursorPosition(g_hout, { x, y });
}

inline void pl_snd(LPCWSTR fname)
{
	PlaySoundW(fname, NULL, SND_FILENAME | SND_ASYNC);
}

inline void ch_col(BYTE text_color, BYTE bg_color)
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

	ofl.write(reinterpret_cast<char*>(levels_speed), sizeof(double) * 3);
}

void level_start(int level_number)
{
	system("cls & color 30");

	pl_snd(L"snd-3.wav");

	HBITMAP hbm = (HBITMAP)LoadImageA(0, "body.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObjectA(hbm, sizeof(BITMAP), &bm);
	SelectObject(g_dc_buf, hbm);

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
	long long timer = 0, dur = 0;
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
			cr_pos(0, 0);

			auto seconds = timer / 1000;

			cout << endl;
			cout << " Level " << (level_number + 1) << "           ";
			print_time(seconds);
			cout << "       Count: " << (text_pos+1) << endl;
			for (int i = 0; i < 42; i++) cout << "\xC4";

			cr_pos(1, 5);
			ch_col(Black, DarkGray);

			for (int i = 0; i < width; i++)
			{
				if (i < width / 2)
				{
					if (text_pos + i - width / 2 >= 0) cout << text[text_pos + i - width / 2];
					else cout << ' ';
				}
				else if (i == width / 2)
				{
					ch_col(Blue, Red);
					cout << text[text_pos];
				}
				else
				{
					ch_col(Black, White);
					if (text_pos + i - width / 2 < text_len) cout << text[text_pos + i - width / 2];
					else cout << ' ';
				}
			}

			ch_col(Red, Cyan);
			cr_pos(width / 2 + 1, 6); cout << '^';
			ch_col(Black, Cyan);

			ch_col(Blue, Cyan);
			cr_pos(1, 19); cout << "EXIT: Esc";
			ch_col(Black, Cyan);
			
			render = false;
		}
		
		cr_pos(41, 19);

		BitBlt(g_hdc, 0, 380, bm.bmWidth, bm.bmHeight, g_dc_buf, 0, 0, SRCCOPY);
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

			ch_col(Blue, Cyan);
			cr_pos(13, 8); cout << "Level " << (level_number+1) << " complete!";
			cr_pos(14, 9); cout << "Speed: " << text_len / (dur / 1000.0) << " LpS";
			ch_col(Black, Cyan);
			
			render = false;
		}

		cr_pos(41, 19);

		BitBlt(g_hdc, 0, 380, bm.bmWidth, bm.bmHeight, g_dc_buf, 0, 0, SRCCOPY);

	}
	
	pl_snd(L"snd-2.wav");

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

	cr_pos(0, 5);
	for (int i = 0; i < 3; i++)
	{
		ch_col(Blue, Cyan); 
		cout << "            Level " << (i + 1) << ": ";
		ch_col(Black, Cyan);

		if (levels_speed[i] > 0.0) cout << levels_speed[i] << " LpS\n\n\n";
		else cout << "no data\n\n\n\n";
	}
	cout << "        (LpS \xC4 lettres per second)";

	ch_col(Blue, Cyan);
	cr_pos(1, 19); cout << "EXIT: Esc";
	ch_col(Black, Cyan);
	
	HBITMAP hbm = (HBITMAP)LoadImageA(0, "header-4-LR.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObjectA(hbm, sizeof(BITMAP), &bm);
	SelectObject(g_dc_buf, hbm);

	while (true)
	{
		if (_kbhit())
		{
			pl_snd(L"snd-2.wav");

			if (_getch() == 27) break;
		}

		cr_pos(41, 19);

		BitBlt(g_hdc, 0, 20, bm.bmWidth, bm.bmHeight, g_dc_buf, 0, 0, SRCCOPY);

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

		if (_kbhit())
		{
			ch = _getch();

			pl_snd(L"snd-2.wav");

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

		if (render)
		{
			if (switcher == 0) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos(14, 2); cout << "           ";
			cr_pos(14, 3); cout << "  LEVEL 1  ";
			cr_pos(14, 4); cout << "           ";

			if (switcher == 1) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos(14, 6); cout << "           ";
			cr_pos(14, 7); cout << "  LEVEL 2  ";
			cr_pos(14, 8); cout << "           ";

			if (switcher == 2) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos(14, 10); cout << "           ";
			cr_pos(14, 11); cout << "  LEVEL 3  ";
			cr_pos(14, 12); cout << "           ";

			if (switcher == 3) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos(14, 14); cout << "           ";
			cr_pos(14, 15); cout << "   <BACK   ";
			cr_pos(14, 16); cout << "           ";

			render = false;
		}
		cr_pos(41, 19);
	}
	system("cls & color 30");
}

int main()
{
	g_hout = GetStdHandle(STD_OUTPUT_HANDLE);
	g_hwnd = GetConsoleWindow();
	g_hdc = GetDC(g_hwnd);
	g_dc_buf = CreateCompatibleDC(g_hdc);

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
	
	HBITMAP hbm = (HBITMAP)LoadImageA(0, "header-4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bm;
	GetObjectA(hbm, sizeof(BITMAP), &bm);
	SelectObject(g_dc_buf, hbm);

	while (menu)
	{
		if (_kbhit())
		{
			ch = _getch();

			pl_snd(L"snd-2.wav");

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

				SelectObject(g_dc_buf, hbm);
			}
			
			render = true;
		}

		if (render)
		{
			if (switcher == 0) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos( 14, 5 ); cout << "           ";
			cr_pos( 14, 6 ); cout << "   TRAIN   ";
			cr_pos( 14, 7 ); cout << "           ";
			
			cout << endl;
			if (switcher == 1) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos( 14, 9 ); cout << "           ";
			cr_pos( 14, 10 ); cout << "  RECORDS  ";
			cr_pos( 14, 11 ); cout << "           ";

			cout << endl;
			if (switcher == 2) ch_col(LightMagenta, Blue);
			else ch_col(Black, LightBlue);
			cr_pos( 14, 13 ); cout << "           ";
			cr_pos( 14, 14 ); cout << "   CLOSE   ";
			cr_pos( 14, 15 ); cout << "           ";


			render = false;
		}
		cr_pos(41, 19);

		BitBlt(g_hdc, 0, 30, bm.bmWidth, bm.bmHeight, g_dc_buf, 0, 0, SRCCOPY);
	}
}