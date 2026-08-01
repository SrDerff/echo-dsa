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
		FileManager::loadPersonalData(currAccount);
	}

	void saveGeneralService() {
		FileManager::saveGeneralData(library.getAllSongs(), albums);
	}

	void unloadAccountService() {
		FileManager::savePersonalData(currAccount);
		currAccount = Account();
	}

	Song& getSongById(int index) {
		return library.getSongById(index);
	}



};