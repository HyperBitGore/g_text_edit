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
	std::string fp;
	std::cout << "Input file path \n";
	std::cin >> fp;
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
		return true;
	}
	std::cout << "Not a valid file path\n";
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