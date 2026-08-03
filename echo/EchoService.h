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

	bool likeSong(int idSong) {
		return currAccount.likeSong(idSong);
	}

	bool dislikeSong(int idSong) {
		return currAccount.dislikeSong(idSong);	
	}

	bool removeLikeSong(int idSong) {
		return currAccount.unlikeSong(idSong);
	}

	bool removeDislikeSong(int idSong) {
		return currAccount.removeDislikeSong(idSong);
	}

	//PLAYLISTS

	bool addSongToPlaylist(std::string plName, int idSong) {
		return currAccount.addSongToPlaylist(plName, idSong);
	}

	bool removeSongFromPlaylist(std::string plName, int idSong) {
		return currAccount.removeSongFromPlaylist(plName, idSong);
	}

	bool createPlaylist(std::string plName) {
		return currAccount.addPlaylist(plName);
	}

	bool deletePlaylist(std::string plName) {
		return currAccount.removePlaylist(plName);
	}

	void openPlaylist(std::string plName) {

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