#pragma once
#include "FileManager.h"
#include "Library.h"
#include "Account.h"
#include "Album.h"
#include "SoundAPI.h"
#include "Interface.h"

class EchoService {
private:
	/*
	TO-DO
	*/
	Library library;
	std::vector<Album>albums;
	Account currAccount;
	ViewData viewData;
	Tab activeTab;
	PlayerState playerState;
public:
	EchoService() {
		loadGeneralService();
		library.buildIndex();
		activeTab = Tab::LIBRARY;
		playerState = PlayerState::PAUSED;
		buildViewData();
	}
	~EchoService() {

	}

	//GENERAL

	void loadGeneralService() {
		FileManager::loadGeneralData(library.getAllSongs(), albums);
	}

	void loadAccountService() {
		FileManager::loadPersonalData(currAccount);
	}

	void saveGeneralService() {
		FileManager::saveGeneralData(library.getAllSongs(), albums);
	}

	void savePersonalService() {
		FileManager::savePersonalData(currAccount);
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

	void quit() {

	}

	//SESSION

	bool login(const std::string& user, const std::string& pass) {
		std::string storedPass;
		int id = -1;
		if (!FileManager::findUser(user, storedPass, id)) return false;
		if (storedPass != pass) return false;

		currAccount = Account(id, user, storedPass, false);
		loadAccountService();
		buildViewData();
		return true;
	}

	bool registerAccount(const std::string& user, const std::string& pass) {
		if (user.empty() || pass.empty()) return false;

		std::string dummy;
		int dummyId;
		if (FileManager::findUser(user, dummy, dummyId)) return false;

		int id = FileManager::getNextUserId();
		currAccount = Account(id, user, pass, false);
		FileManager::savePersonalData(currAccount);
		loadAccountService();
		buildViewData();
		return true;
	}

	void logout() {
		currAccount = Account();
		buildViewData();
	}

	//VIEW DATA
	void buildViewData() {
		viewData.activeTab = activeTab;
		viewData.username = currAccount.getUsername();
		
		viewData.rows.clear();
		std::vector<RowData> _rows;
		if (activeTab == Tab::LIBRARY) {
			HashTable<int, Song>& allSongs = library.getAllSongs();
			library.buildIndex();
			for(auto it= allSongs.begin(); it != allSongs.end(); ++it) {
				Song& song = it.getValue();
				RowData row;
				row.title = song.getName();
				row.artist = song.getAuthor();
				row.genre = song.getGenre();
				row.likes = song.getLikesCount();
				row.isLiked = currAccount.getLikedSongs().contains(song.getIdSong());
				row.duration = song.getLength();
				row.isPlaying = (song.getIdSong() == library.getCurrentSongId());
				row.isPlaylist = false;
				row.playlistSize = -1;

				_rows.push_back(row);
			}
		}
		viewData.rows = _rows;

		viewData.selectedIndex = 1;
		viewData.topRowIndex = 1;

		//test
		PlayerData _playerData;
		_playerData.songName = "Test Song";
		_playerData.artist = "Test Artist";
		_playerData.state = playerState;
		_playerData.length = 180.0f;
		_playerData.position = 45.0f;

		viewData.player = _playerData;
	}

	const ViewData& getViewData() {
		return viewData;
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