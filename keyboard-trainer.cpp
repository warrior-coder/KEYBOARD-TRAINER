#include <iostream>
#include <iomanip>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <chrono>


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


void TextColor(int text_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bg_color << 4) | text_color);
}

int main()
{
	// set up
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX cfi = {};

	cfi.dwFontSize = { 15, 25 };
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	cfi.FontWeight = FW_BOLD;
	wcscpy_s(cfi.FaceName, L"Lucida console");
	SetCurrentConsoleFontEx(hout, false, &cfi);

	system("mode con cols=42 lines=20");
	system("title Keyboard Trainer");
	system("color 30");



	ifstream ofl("level_1.dat");
	char text[1024];
	int level_number = 1;
	int pos = 0;
	ofl.getline(text, 1024, '\0');
	int text_length = strlen(text);

	bool game = true, render = true;
	int ch = 0;
	long long timer = 0;
	auto start = chrono::high_resolution_clock::now();
	const auto width = 40;

	while (game)
	{

		if (_kbhit())
		{
			ch = _getch();

			if (ch == text[pos])
			{
				render = true;
				pos++;

				if (pos == text_length) game = false;
			}
			else if (ch == 27) break;
			else cout << '\a';
		}



		auto now = chrono::high_resolution_clock::now();
		auto dur = chrono::duration_cast<chrono::milliseconds>(now - start).count();

		if (dur - timer > 1000)
		{
			render = true;
			timer = dur;
		}

		if (render)
		{
			SetConsoleCursorPosition(hout, { 0, 0 });

			auto seconds = timer / 1000;
			cout << " Level " << level_number << "         Time: " << (seconds / 60) << ':' << (seconds % 60) << endl;
			for (int i = 0; i < 42; i++) cout << "\xC4";
			cout << endl;
			cout << endl;

			cout << ' ';
			TextColor(Black, DarkGray);
			for (int i = 0; i < width; i++)
			{
				if (i < width / 2)
				{
					if (pos + i - width / 2 >= 0) cout << text[pos + i - width / 2];
					else cout << ' ';
				}
				else if (i == width / 2)
				{
					TextColor(Blue, Red);
					cout << text[pos];
				}
				else
				{
					TextColor(Black, White);
					if (pos + i - width / 2 < text_length) cout << text[pos + i - width / 2];
					else cout << ' ';
				}

			}
			cout << endl;

			TextColor(Red, Cyan);
			cout << setw(width / 2 + 2) << '^';


			TextColor(Black, Cyan);
			cout << "\n\n\n ";

			TextColor(Cyan, Blue);
			cout << " Esc - exit " << endl;
			TextColor(Black, Cyan);
			cout << "\n\n\n" << endl;

			render = false;
		}


	}


	return 0;
}