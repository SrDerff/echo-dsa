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
public:
	Library() : visibleRows(17), topRowIndex(0), currentRowIndex(0) {

	}

	HashTable<int, Song>& getAllSongs() { return allSongs; }
	int getVisibleRows() { return visibleRows; }
	int getTopRowIndex() { return topRowIndex; }
	int getCurrentRowIndex() { return currentRowIndex; }
	int getCurrentSongId() {
		if (songOrder.empty()) return -1;
		int idx = currentRowIndex + topRowIndex;
		if (idx < 0) idx = 0;
		if (idx >= (int)songOrder.size()) idx = (int)songOrder.size() - 1;
		return songOrder[idx];
	}

	void setVisibleRows(int row) { visibleRows = row; }
	void setTopRowIndex(int row) { topRowIndex = row; }
	void setCurrentRowIndex(int row) { currentRowIndex = row; }

	void moveCursor(int delta) {
		int count = getCount();
		if (count <= 0) return;

		int abs = topRowIndex + currentRowIndex + delta;
		if (abs < 0) abs = 0;
		if (abs >= count) abs = count - 1;

		if (abs < topRowIndex) {
			topRowIndex = abs;
			currentRowIndex = 0;
		}
		else if (abs >= topRowIndex + visibleRows) {
			topRowIndex = abs - visibleRows + 1;
			currentRowIndex = visibleRows - 1;
		}
		else {
			currentRowIndex = abs - topRowIndex;
		}
	}

	Song& getSongById(int idSong) {
		return allSongs.getElement(idSong);
	}

	bool containsSong(int idSong) {
		return allSongs.contains(idSong);
	}

	void buildIndex() {
		songOrder.clear();
		for (auto it = allSongs.begin(); it != allSongs.end(); ++it) {
			songOrder.push_back(it.getKey());
		}
	}

	std::vector<int>& getSongOrder() { return songOrder; }

	int getCount() const { return (int)songOrder.size(); }
	int getIdAtRow(int row) const { return songOrder[row - 1]; }

};