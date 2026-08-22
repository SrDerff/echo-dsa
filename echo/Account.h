#pragma once

#include <string>
#include "Playlist.h"
#include "Set.h"
#include <vector>

class Account {
private:
	int idAccount;
	std::string username;
	std::string passwordHash;
	bool vip;
	/*
	TO-DO
	playlists,likes,dislikes,etc
	*/
	std::vector<Playlist> playlists;
	Set<int>idsLikedSongs;
	Set<int>idsDislikedSongs;

	bool plsOpen;
	int indexOpenPlaylist;

	int currentRowIndexPlaylist;
	int topRowIndexPlaylist;
	int visibleRowsPlaylist;

	int currentRowIndexLikes;
	int topRowIndexLikes;
	int visibleRowsLikes;
public:
	Account() {
		idAccount = -1;
		username = "null";
		passwordHash = "null";
		vip = false;
		plsOpen = false;
		indexOpenPlaylist = -1;
		currentRowIndexPlaylist = 0;
		topRowIndexPlaylist = 0;
		visibleRowsPlaylist = 17;
		currentRowIndexLikes = 0;
		topRowIndexLikes = 0;
		visibleRowsLikes = 0;
	}
	
	Account(int _id, std::string _user, std::string _password, bool _vip) {
		idAccount = _id;
		username = _user;
		passwordHash = _password;
		vip = _vip;
		plsOpen = false;
		indexOpenPlaylist = -1;
		currentRowIndexPlaylist = 0;
		topRowIndexPlaylist = 0;
		visibleRowsPlaylist = 17;
		currentRowIndexLikes = 0;
		topRowIndexLikes = 0;
		visibleRowsLikes = 0;
	}
	
	int getIdAccount() { return idAccount; }
	std::string getUsername() { return username; }
	std::string getPasswordHash() { return passwordHash; }
	bool isVip() { return vip; }
	bool isPlaylistOpen() { return plsOpen; }
	int getIndexOpenPlaylist() const { return indexOpenPlaylist; }

	int getCurrentRowIndexPlaylist() const { return currentRowIndexPlaylist; }
	int getTopRowIndexPlaylist() const { return topRowIndexPlaylist; }
	int getVisibleRowsPlaylist() const { return visibleRowsPlaylist; }

	int getCurrentRowIndexLikes() const { return currentRowIndexLikes; }
	int getTopRowIndexLikes() const { return topRowIndexLikes; }
	int getVisibleRowsLikes() const { return visibleRowsLikes; }


	void setIdAccount(int _id) { idAccount = _id; }
	void setUsername(std::string _user) { username = _user; }
	void setPasswordHash(std::string _password) { passwordHash = _password; }
	void setVip(bool _vip) { vip = _vip; }
	void setPlaylistOpen(bool _open) { plsOpen = _open; }
	void setIndexOpenPlaylist(int _index) { indexOpenPlaylist = _index; }
	void setVisibleRowsPlaylist(int rows) { visibleRowsPlaylist = rows; }
	void setTopRowIndexPlaylist(int row) { topRowIndexPlaylist = row; }
	void setCurrentRowIndexPlaylist(int row) { currentRowIndexPlaylist = row; }

	// Misma semantica que Library::moveCursor (clamp + scroll de ventana).
	void movePlaylistCursor(int delta) {
		int count = (int)playlists.size();
		if (count <= 0) return;

		int abs = topRowIndexPlaylist + currentRowIndexPlaylist + delta;
		if (abs < 0) abs = 0;
		if (abs >= count) abs = count - 1;

		if (abs < topRowIndexPlaylist) {
			topRowIndexPlaylist = abs;
			currentRowIndexPlaylist = 0;
		}
		else if (abs >= topRowIndexPlaylist + visibleRowsPlaylist) {
			topRowIndexPlaylist = abs - visibleRowsPlaylist + 1;
			currentRowIndexPlaylist = visibleRowsPlaylist - 1;
		}
		else {
			currentRowIndexPlaylist = abs - topRowIndexPlaylist;
		}
	}

	std::vector<Playlist>& getPlaylists() { return playlists; }
	Set<int>& getLikedSongs() { return idsLikedSongs; }
	Set<int>& getDislikedSongs() { return idsDislikedSongs; }

	int getIndexPlaylistByName(std::string name) {
		int index = 0;
		for (Playlist& pl : playlists) {
			if (pl.getName() == name) return index;
			index++;
		}
		return -1;
	}

	bool addPlaylist(std::string name){
		if (getIndexPlaylistByName(name) != -1) return false;
		playlists.push_back(Playlist(name));
		return true;
	}

	bool removePlaylist(std::string name) {
		int index = getIndexPlaylistByName(name);
		if (index == -1) return false;
		playlists.erase(playlists.begin() + index);
		return true;
	}

	bool addSongToPlaylist(std::string playlistName, int idSong) {
		long long index = getIndexPlaylistByName(playlistName);
		if (index == -1) return false;
		playlists[index].addSong(idSong);
		return true;
	}

	bool removeSongFromPlaylist(std::string playlistName, int idSong) {
		long long index = getIndexPlaylistByName(playlistName);
		if (index == -1) return false;
		return playlists[index].removeSong(idSong);
	}

	bool likeSong(int idSong) {
		idsDislikedSongs.remove(idSong);
		return idsLikedSongs.insert(idSong);
	}

	bool unlikeSong(int idSong) {
		return idsLikedSongs.remove(idSong);
	}

	bool dislikeSong(int idSong) {
		idsLikedSongs.remove(idSong);
		return idsDislikedSongs.insert(idSong);
	}

	bool removeDislikeSong(int idSong) {
		return idsDislikedSongs.remove(idSong);
	}
	
	std::vector<int> getLikedSongsOrdered() {
		std::vector<int> order;
		for (auto it = idsLikedSongs.begin(); it != idsLikedSongs.end(); ++it) {
			order.push_back(*it);
		}
		return order;
	}

	std::vector<int> getDislikedSongsOrdered() {
		std::vector<int> order;
		for (auto it = idsDislikedSongs.begin(); it != idsDislikedSongs.end(); ++it) {
			order.push_back(*it);
		}
		return order;
	}
};