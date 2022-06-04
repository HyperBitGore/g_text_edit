#include "Header.h"
#define BACKSPACE 8
#define ESCAPE 27
#define ENTER 10

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
	//init_pair()
	size_t x = f1.data[f1.data.size() - 1].size() - 1;
	size_t line = f1.data.size() - 1;
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	//add enter
	//add copy and paste
	//maybe add ability to multiple files open
	//maybe add ctrl + s to save
	while (!exitf) {
		bool write = false;
		bool er = false;
		bool nl = false;
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
			er = true;
			write = false;
			break;
		case ENTER:
			nl = true;
			er = false;
			write = false;
			break;
		case ESCAPE:
			exitf = true;
			break;
		default:
			if (key != ERR && key >= 32) {
				write = true;
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
					mvaddch(curline, cx, j);
					attroff(COLOR_PAIR(2));
					if (write) {
						i.insert(i.begin() + x + 1, key);
						x++;
						write = false;
					}
					else if (er) {
						i.erase(i.begin() + x);
						x--;
						er = false;
					}
					else if (nl) {
						
					}
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