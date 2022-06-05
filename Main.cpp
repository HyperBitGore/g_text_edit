#include "Header.h"
#define BACKSPACE 8
#define ESCAPE 27
#define ENTER 10
#define TAB 9
#define CTRL_TAB 482
#define CTRL_N 14
#define S 115
#define CTRL_S 19
#define CTRL_V 22

enum {WRITE = 5, ERASE, ISNEWLINE};


//https://pdcurses.org/docs/MANUAL.html
int main() {
	bool exitf = false;
	initscr();
	cbreak();
	noecho();
	timeout(0);
	//halfdelay(1);
	keypad(stdscr, TRUE);
	start_color();
	FileBundle bu = FileBundle();
	size_t x = bu.curfile->data[bu.curfile->data.size() - 1].size() - 1;
	size_t line = bu.curfile->data.size() - 1;
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	while (!exitf) {
		uint8_t change = 0;
		int key = getch();
		switch (key) {
		case KEY_LEFT:
			//self explanatory
			x--;
			break;
		case KEY_RIGHT:
			//self explanatory
			x++;
			break;
		case KEY_UP:
			//self explanatory
			line--;
			break;
		case KEY_DOWN:
			//self explanatory
			line++;
			break;
		case BACKSPACE:
			//self explanatory
			change = ERASE;
			break;
		case ENTER:
			//self explanatory
			change = ISNEWLINE;
			break;
		case ESCAPE:
			//quick exit
			exitf = true;
			break;
		case CTRL_S:
			//save shortcut
			saveFile(bu.curfile);
			bu.curfile->changes = false;
			break;
		case CTRL_V:
			//paste in clipboard contents to file
			long le;
			char* temp;
			PDC_getclipboard(&temp, &le);
			int ps;
			for (int i = 0, ps = x + 1; i < le; i++, ps++) {
				if (temp[i] != '\n') {
					bu.curfile->data[line].insert(bu.curfile->data[line].begin() + ps, temp[i]);
				}
			}
			for (size_t i = 0; i < bu.curfile->data[line].size(); i++) {
				if (bu.curfile->data[line][i] == '\n') {
					bu.curfile->data[line].erase(bu.curfile->data[line].begin() + i);
					i = bu.curfile->data[line].size();
				}
			}
			bu.curfile->data[line].push_back('\n');
			PDC_freeclipboard(temp);
			break;
		case CTRL_TAB:
			//file change key
			bu.moveForward();
			break;
		case CTRL_N:
			//open new file key
			bu.addFile();
			break;
		default:
			if (key != ERR && key >= 32 || key == TAB) {
				change = WRITE;
			}
			break;
		}
		//making sure current positions are within bounds
		if (line > bu.curfile->data.size() - 1) {
			line = bu.curfile->data.size() - 1;
		}
		else if (line < 0) {
			line = 0;
		}
		if (x < 0) {
			x = 0;
		}
		else if (x > bu.curfile->data[line].size() - 1) {
			x = bu.curfile->data[line].size() - 1;
		}
		//changes that can be made by user
		switch (change) {
		case WRITE:
			bu.curfile->data[line].insert(bu.curfile->data[line].begin() + x, key);
			x++;
			bu.curfile->changes = true;
			break;
		case ERASE:
			bu.curfile->data[line].erase(bu.curfile->data[line].begin() + x);
			if (bu.curfile->data[line].size() == 0) {
				bu.curfile->data.erase(bu.curfile->data.begin() + line);
				line--;
			}
			else {
				x--;
			}
			bu.curfile->changes = true;
			break;
		case ISNEWLINE:
			std::string temp;
			int befn = 0;
			for (size_t i = x; i < bu.curfile->data[line].size(); i++) {
				if (bu.curfile->data[line][i] == '\n' && i != 0) {
					befn = i;
				}
				temp.push_back(bu.curfile->data[line][i]);
			}
			if (temp.size() > 1) {
				bu.curfile->data[line].erase(bu.curfile->data[line].begin() + x, bu.curfile->data[line].begin() + befn);
			}
			bu.curfile->data.insert(bu.curfile->data.begin() + line + 1, temp);
			line++;
			bu.curfile->changes = true;
			break;
		}
		clear();
		//rendering of text
		attron(COLOR_PAIR(2));
		printw("FILE NAME : ");
		printw(bu.curfile->name.c_str());
		printw(" Changes: ");
		std::string ink;
		if (bu.curfile->changes) {
			ink = "Unsaved";
		}
		else {
			ink = "Saved";
		}
		ink.push_back('\n');
		printw(ink.c_str());
		attroff(COLOR_PAIR(2));
		//put in file tab system here
		for (auto& i : bu.getFiles()) {
			std::string temp = i->name + " ";
			if (i == bu.curfile) {
				attron(COLOR_PAIR(2));
				printw(temp.c_str());
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				printw(temp.c_str());
				attroff(COLOR_PAIR(1));
			}
		}
		printw("\n");

		//actual rendering of text file
		size_t curline = 2;
		size_t cx = 0;
		size_t t = 0;
		if (line > 27) {
			t = line - 27;
		}
		for (size_t i = t; i < bu.curfile->data.size(); i++) {
			cx = 0;
			for (auto& j : bu.curfile->data[i]) {
				if (curline - 2 == line - t && cx == x) {
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
	bu.~FileBundle();
	return 0;
}