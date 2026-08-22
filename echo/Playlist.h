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
	Playlist(std::string _name) : idPlaylist(0), visibleRows(17), topRowIndex(0), currentRowIndex(0) {
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
	int getCount() { return songsIds.getSize(); }

	int getSongIdAt(int index) {
		if (index < 0 || index >= (int)songsIds.getSize()) return -1;
		return songsIds.getAt((size_t)index);
	}

	LinkedList<int>& getSongsIds() { return songsIds; }

	void setIdPlaylist(int _id) { idPlaylist = _id; }

	void setVisibleRows(int row) { visibleRows = row; }
	void setTopRowIndex(int row) { topRowIndex = row; }
	void setCurrentRowIndex(int row) { currentRowIndex = row; }
	void setCurrentSongId(int id) { currentSongId = id; }

	// Misma semantica que Library::moveCursor (clamp + scroll de ventana).
	void moveSongCursor(int delta) {
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

	void addSong(int newSongId){
		songsIds.pushBack(newSongId);
	}

	bool removeSong(int songId) {
		return songsIds.remove(songId);
	}

	size_t getSize() { return songsIds.getSize(); }

};