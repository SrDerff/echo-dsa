#pragma once
#include "FileManager.h"
#include "Library.h"
#include "Account.h"
#include "Album.h"

class EchoService {
private:
	/*
	TO-DO
	*/
	Library library;
	std::vector<Album>albums;
	Account currAccount;
public:
	EchoService() {
		loadGeneralService();
	}
	~EchoService() {

	}

	void loadGeneralService() {
		FileManager::loadGeneralData(library.getAllSongs(), albums);

	}

	void loadAccountService() {

	}

	Song& getSongById(int index) {
		return library.getSongById(index);
	}

};