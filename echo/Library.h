#pragma once
#include "HashTable.h"
#include "Song.h"

class Library {
private:
	HashTable<int, Song>allSongs;
	std::vector<int>songOrder;
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

	void setVisibleRows(int row) { visibleRows = row; }
	void setTopRowIndex(int row) { topRowIndex = row; }
	void setCurrentRowIndex(int row) { currentRowIndex = row; }

	Song& getSongById(int idSong) {
		return allSongs.getElement(idSong);
	}

	int getCurrentSongId() {
		return currentSongId;
	}

	void buildIndex() {
		songOrder.clear();
		for (auto it = allSongs.begin(); it != allSongs.end(); ++it) {
			songOrder.push_back(it.getKey());
		}
	}
	int getCount() const { return (int)songOrder.size(); }
	int getIdAtRow(int row) const { return songOrder[row - 1]; }
};