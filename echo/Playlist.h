#pragma once
#include "Song.h"
#include "LinkedList.h"

class Playlist {
private:
	int idPlaylist;
	std::string name;
	LinkedList<int>songsIds;
	int visibleRows;
	int topRowIndex;
	int currentRowIndex;
	int currentSongId;
public:
	Playlist(std::string _name) : visibleRows(17), topRowIndex(17), currentRowIndex(1) {
		name = _name;
	}
	~Playlist() {

	}

	std::string getName() { return name; }
	int getIdPlaylist() { return idPlaylist; }
	int getVisibleRows() { return visibleRows; }
	int getTopRowIndex() { return topRowIndex; }
	int getCurrentRowIndex() { return currentRowIndex; }
	int getCurrentSongId() { return currentSongId; }

	void setVisibleRows(int row) { visibleRows = row; }
	void setTopRowIndex(int row) { topRowIndex = row; }
	void setCurrentRowIndex(int row) { currentRowIndex = row; }

	void addSong(int newSongId){
		songsIds.pushBack(newSongId);
	}

	bool removeSong(int songId) {
		return songsIds.remove(songId);
	}

	size_t getSize() { return songsIds.getSize(); }

};