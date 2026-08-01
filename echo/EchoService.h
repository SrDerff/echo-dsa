#pragma once
#include "FileManager.h"
#include "Library.h"
#include "Account.h"
#include "Album.h"
#include "SoundAPI.h"

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

	void playSong() {
		
	}

	//LIKES

	bool likeSong() {

	}

	bool dislikeSong() {
	
	}

	bool removeLikeSong() {

	}

	bool removeDislikeSong() {

	}

	//PLAYLISTS

	bool addSongToPlaylist() {

	}

	bool removeSongFromPlaylist() {

	}

	bool createPlaylist() {

	}

	bool deletePlaylist() {

	}

	void openPlaylist() {

	}

	void closePlaylist() {
	
	}

	//ALBUMS

	void openAlbum() {

	}

	void closeAlbum() {
	
	}

	//RECOMMENDATIONS


	//SEARCH

};