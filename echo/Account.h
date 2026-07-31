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
public:
	Account() {
		idAccount = -1;
		username = "null";
		passwordHash = "null";
		vip = false;
	}
	
	Account(int _id, std::string _user, std::string _password, bool _vip) {
		idAccount = _id;
		username = _user;
		passwordHash = _password;
		vip = _vip;
	}
	
	int getIdAccount() { return idAccount; }
	std::string getUsername() { return username; }
	std::string getPasswordHash() { return passwordHash; }
	bool isVip() { return vip; }

	void setIdAccount(int _id) { idAccount = _id; }
	void setUsername(std::string _user) { username = _user; }
	void setPasswordHash(std::string _password) { passwordHash = _password; }
	void setVip(bool _vip) { vip = _vip; }

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

};