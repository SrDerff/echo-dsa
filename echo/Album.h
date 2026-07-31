#pragma once
#include <string>
#include "LinkedList.h"

class Album {
private:
	int idAlbum;
	std::string name;
	std::string author;
	size_t songsCount;
	LinkedList<int>idsSongs;
public:
	Album(){}
	Album(
		int _idAlbum,
		std::string _name,
		std::string _author,
		size_t _size,
		LinkedList<int>_idsSongs
	){
		idAlbum = _idAlbum;
		name = _name;
		author = _author;
		songsCount = _size;
		idsSongs = _idsSongs;
	}

	void insertSong(int idSong) {
		idsSongs.pushBack(idSong);
	}

	bool removeSong(int idSong) {
		return idsSongs.remove(idSong);
	}

	std::string getName() { return name; }
	std::string getAuthor() { return author; }
	size_t getSize() { return songsCount; }

	int getIdAlbum() { return idAlbum; }
	LinkedList<int>& getIdsSongs() { return idsSongs; }

	void setName(std::string _name) { name = _name; }
	void setAuthor(std::string _author) { author = _author; }
	void setSize(size_t _size) { songsCount = _size; }
};