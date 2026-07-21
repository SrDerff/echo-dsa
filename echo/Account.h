#pragma once

#include <string>

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
public:
	int getIdAccount() { return idAccount; }
	std::string getUsername() { return username; }
	std::string getPasswordHash() { return passwordHash; }
	bool isVip() { return vip; }
};