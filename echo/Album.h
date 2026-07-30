#pragma once
#include <string>
#include "LinkedList.h"

class Album {
private:
	std::string name;
	std::string author;
	size_t size;
	LinkedList<int>idsSongs;
public:
	Album(){}
	Album(std::string _name, std::string _author, size_t _size) {
		name = _name;
		author = _author;
		size = _size;
	}

	void insertSong(int idSong) {
		idsSongs.pushBack(idSong);
	}

	bool removeSong(int idSong) {
		return idsSongs.remove(idSong);
	}

	std::string getName() { return name; }
	std::string getAuthor() { return author; }
	size_t getSize() { return size; }

	void setName(std::string _name) { name = _name; }
	void setAuthor(std::string _author) { author = _author; }
	void setSize(size_t _size) { size = _size; }
};