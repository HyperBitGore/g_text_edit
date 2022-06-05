#include "Header.h"
#define BACKSPACE 8
#define ESCAPE 27
#define ENTER 10

enum {WRITE = 5, ERASE, ISNEWLINE};

bool exitf = false;

//https://pdcurses.org/docs/MANUAL.html
int main() {
	File f1;
	fileread:
	if (!getFile(&f1)) {
		goto fileread;
	}
	std::cout << std::endl;
	initscr();
	cbreak();
	noecho();
	timeout(0);
	//halfdelay(1);
	keypad(stdscr, TRUE);
	start_color();
	size_t x = f1.data[f1.data.size() - 1].size() - 1;
	size_t line = f1.data.size() - 1;
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	//add copy and paste
	//maybe add ability to multiple files open
	//maybe add ctrl + s to save
	while (!exitf) {
		uint8_t change = 0;
		int key = getch();
		switch (key) {
		case KEY_LEFT:
			x--;
			break;
		case KEY_RIGHT:
			x++;
			break;
		case KEY_UP:
			line--;
			break;
		case KEY_DOWN:
			line++;
			break;
		case BACKSPACE:
			change = ERASE;
			break;
		case ENTER:
			change = ISNEWLINE;
			break;
		case ESCAPE:
			exitf = true;
			break;
		default:
			if (key != ERR && key >= 32) {
				change = WRITE;
			}
			break;
		}
		if (line > f1.data.size() - 1) {
			line = f1.data.size() - 1;
		}
		else if (line < 0) {
			line = 0;
		}
		if (x < 0) {
			x = 0;
		}
		else if (x > f1.data[line].size() - 1) {
			x = f1.data[line].size() - 1;
		}
		switch (change) {
		case WRITE:
			f1.data[line].insert(f1.data[line].begin() + x, key);
			x++;
			break;
		case ERASE:
			f1.data[line].erase(f1.data[line].begin() + x);
			if (f1.data[line].size() == 0) {
				f1.data.erase(f1.data.begin() + line);
				line--;
			}
			else {
				x--;
			}
			break;
		case ISNEWLINE:
			std::string temp;
			int befn = 0;
			for (int i = x; i < f1.data[line].size(); i++) {
				if (f1.data[line][i] == '\n' && i != 0) {
					befn = i;
				}
				temp.push_back(f1.data[line][i]);
			}
			if (temp.size() > 1) {
				f1.data[line].erase(f1.data[line].begin() + x, f1.data[line].begin() + befn);
			}
			f1.data.insert(f1.data.begin() + line + 1, temp);
			line++;
			break;
		}
		
		clear();
		printw("FILE NAME : ");
		printw(f1.name.c_str());
		printw(" Key: ");
		std::string ink = std::to_string(key);
		printw(ink.c_str());
		printw("\n");
		size_t curline = 1;
		size_t cx = 0;
		for (auto& i : f1.data) {
			cx = 0;
			for (auto& j : i) {
				if (curline - 1 == line && cx == x) {
					attron(COLOR_PAIR(2));
					if (j != '\n') {
						mvaddch(curline, cx, j);
					}
					else {
						mvaddch(curline, cx, ' ');
					}
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(1));
					mvaddch(curline, cx, j);
					attroff(COLOR_PAIR(1));
				}
				cx++;
			}
			curline++;
		}
		refresh();
	}
	if (saveFile(&f1)) {
		std::cout << "File saved!" << std::endl;
	}
	f1.~File();
	return 0;
}