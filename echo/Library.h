#pragma once
#include "HashTable.h"
#include "Song.h"

class Library {
private:
	HashTable<int, Song>allSongs;
	int visibleRows;
	int topRowIndex;
	int currentRowIndex;
	int currentSongId;
public:
	Library() : visibleRows(17), topRowIndex(17), currentRowIndex(1) {

	}

	HashTable<int, Song>& getAllSongs() { return allSongs; }
	int getVisibleRows() { return visibleRows; }
	int getTopRowIndex() { return topRowIndex; }
	int getCurrentRowIndex() { return currentRowIndex; }

	Song& getSongById(int idSong) {
		return allSongs.getElement(idSong);
	}

	int getCurrentSongId() {
		return currentSongId;
	}
};