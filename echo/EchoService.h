#pragma once
#include "FileManager.h"
#include "Library.h"
#include "Account.h"
class EchoService {
private:
	/*
	TO-DO
	*/
	Library library;
	Account currAccount;
public:
	EchoService() {
		loadGeneralService();
	}
	~EchoService() {

	}
	void loadGeneralService() {
		FileManager::loadGeneralData(library.getAllSongs());
	}
	void loadAccountService() {

	}

};