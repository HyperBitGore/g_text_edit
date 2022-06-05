#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curses.h>


class File {
public:
	std::vector<std::string> data;
	std::string name;
	size_t size;
	bool changes = false;
	File() {
		name = "EMPTY";
		size = 0;
	}
	File(std::string np, std::vector<std::string> dt) {
		name = np;
		data = dt;
		size = 0;
		for (auto& i : data) {
			for (auto& j : i) {
				size++;
			}
		}
	}
	~File() {
		name.clear();
		data.clear();
		size = 0;
	}
};

bool getFile(File* f1) {
	timeout(-1);
	std::string fp;
	char in = 1;
	int n = 0;
	while (in != '\n') {
		if (in == 8) {
			fp.erase(fp.begin() + n);
			n--;
		}else if (in >= 32) {
			fp.push_back(in);
			n++;
		}
		clear();
		printw("Input File Path\n");
		printw(fp.c_str());
		refresh();
		in = getch();
	}
	timeout(0);
	std::fstream file;
	file.open(fp);
	if (file) {
		std::vector<std::string> temp;
		std::string line;
		while (std::getline(file, line)) {
			line.append("\n");
			temp.push_back(line);
		}
		File f(fp, temp);
		*f1 = f;
		file.close();
		return true;
	}
	file.close();
	return false;
}


bool saveFile(File* f) {
	std::ofstream fp;
	fp.open(f->name);
	if (fp) {
		for (auto& i : f->data) {
			fp << i;
		}
		return true;
	}
	return false;
}



class FileBundle {
private:
	std::vector<File*> files;
	size_t index = 0;
public:
	File* curfile;
	FileBundle() {
		addFile();
		curfile = files[0];
	}
	void addFile() {
		File* f = new File;
		getFile(f);
		files.push_back(f);
	}
	void moveForward() {
		index++;
		if (index < files.size()) {
			curfile = files[index];
		}
		else {
			curfile = files[0];
			index = 0;
		}
	}
	std::vector<File*>& getFiles() {
		return files;
	}
	~FileBundle() {
		for (auto& i : files) {
			saveFile(i);
			i->~File();
			delete i;
		}
		files.clear();
	}
};