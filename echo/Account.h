#pragma once

#include <string>
#include "Playlist.h"
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

public:
	Account() {
		idAccount = -1;
		username = "null";
		passwordHash = "null";
		vip = false;
	}
	
	Account(int _id, string _user, string _password, bool _vip) {
		idAccount = _id;
		username = _user;
		passwordHash = _password;
		vip = _vip;
	}
	
	int getIdAccount() { return idAccount; }
	std::string getUsername() { return username; }
	std::string getPasswordHash() { return passwordHash; }
	bool isVip() { return vip; }

	bool addPlaylist(){}
};