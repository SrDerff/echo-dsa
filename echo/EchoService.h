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
	SoundAPI soundAPI;
public:
	EchoService() {
		loadGeneralService();
		library.buildIndex();
		activeTab = Tab::LIBRARY;
		playerState = PlayerState::PAUSED;
		buildViewData();
	}
	~EchoService() {
		viewData.player.songName = "No song playing";
		viewData.player.artist = "No artist";
		viewData.player.state = playerState;
		viewData.player.length = 0.0f;
		viewData.player.position = 0.0f;
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

	void unloadAccountService() {
		FileManager::savePersonalData(currAccount);
	}

	Song& getSongById(int index) {
		return library.getSongById(index);
	}

	Tab getActiveTab() {
		return activeTab;
	}

	void setActiveTab(const Tab& tab) {
		activeTab = tab;
		buildViewData();
	}	

	void playSong(int idSong) {
		if (soundAPI.load(getSongById(idSong).getSourcePath())) { }
		else std::cout << "error";   
		if (soundAPI.play()) { }
	}

	void stopSong() {
		if (soundAPI.stop()) { }
		else std::cout << "error";
	}

	void resumeSong() {
		if (soundAPI.resume()) { }
		else std::cout << "error";
	}

	void pauseSong() {
		if (soundAPI.pause()) { }
		else std::cout << "error";
	}

	int getIdAtRow(int row) {
		return library.getIdAtRow(row);
	}

	void quit() {

	}

	//ACCOUNT
	Account& getCurrentAccount() {
		return currAccount;
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
		viewData.playlistOpen = (activeTab == Tab::PLAYLISTS && currAccount.isPlaylistOpen());

		viewData.rows.clear();
		std::vector<RowData> _rows;
		if (activeTab == Tab::LIBRARY) {
			buildLibraryView(_rows);
		}
		else if(activeTab == Tab::PLAYLISTS) {
			buildPlaylistViewGeneral(_rows);
		}
		else if(activeTab == Tab::LIKES) {
			buildLikesView(_rows);
		}
		else if(activeTab == Tab::RECOMMENDED) {
		
		}
		else if(activeTab == Tab::HISTORIAL) {
		
		}
		else if(activeTab == Tab::SEARCH) {
		
		}
		viewData.rows = _rows;
	}

	void buildLibraryView(std::vector<RowData>& _rows) {
		std::vector<int> songOrder = library.getSongOrder();
		library.buildIndex();
		for (size_t i = 0; i < songOrder.size(); i++) {
			Song& song = library.getSongById(songOrder[i]);
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

		viewData.selectedIndex = library.getTopRowIndex() + library.getCurrentRowIndex() + 1;
		viewData.topRowIndex = library.getTopRowIndex() + 1;
	}

	void buildPlaylistViewGeneral(std::vector<RowData>& _rows) {
		if (currAccount.isPlaylistOpen()) {
			buildPlaylistViewOpen(_rows);
			return;
		}

		for (Playlist& pl : currAccount.getPlaylists()) {
			RowData row;
			row.title = pl.getName();
			row.artist = "Playlist";
			row.genre = "Playlist";
			row.likes = -1;
			row.isLiked = false;
			row.duration = -1.0f;
			row.isPlaying = false;
			row.isPlaylist = true;
			row.playlistSize = pl.getCount();
			_rows.push_back(row);
		}

		viewData.selectedIndex = currAccount.getCurrentRowIndexPlaylist() + currAccount.getTopRowIndexPlaylist() + 1;
		viewData.topRowIndex = currAccount.getTopRowIndexPlaylist() + 1;
	}

	void buildPlaylistViewOpen(std::vector<RowData>& _rows) {
		Playlist* pl = getOpenPlaylist();
		if (!pl) return;

		for (int idSong : pl->getSongsIds()) {
			Song& song = library.getSongById(idSong);
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

		viewData.selectedIndex = pl->getCurrentRowIndex() + pl->getTopRowIndex() + 1;
		viewData.topRowIndex = pl->getTopRowIndex() + 1;
	}

	void buildLikesView(std::vector<RowData>& _rows) {
	
	}

	void updatePlayerData(const PlayerData& playerData) {
		viewData.player = playerData;
	}

	ViewData& getViewData() {
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

	//INDEXS
	int getVisibleRows() { 
		if (activeTab == Tab::LIBRARY) {
			return library.getVisibleRows(); 
		}
		if (activeTab == Tab::PLAYLISTS) {
			Playlist* pl = getOpenPlaylist();
			if (pl) return pl->getVisibleRows();
			return currAccount.getVisibleRowsPlaylist();
		}
		return 0;
	}

	int getTopRowIndex() {
		if(activeTab == Tab::LIBRARY) {
			return library.getTopRowIndex();
		}
		if(activeTab == Tab::PLAYLISTS) {
			Playlist* pl = getOpenPlaylist();
			if (pl) return pl->getTopRowIndex();
			return currAccount.getTopRowIndexPlaylist();
		}
		return 0;
	}

	int getCurrentRowIndex() {
		if(activeTab == Tab::LIBRARY) {
			return library.getCurrentRowIndex();
		}
		if(activeTab == Tab::PLAYLISTS) {
			Playlist* pl = getOpenPlaylist();
			if (pl) return pl->getCurrentRowIndex();
			return currAccount.getCurrentRowIndexPlaylist();
		}
		return 0;
	}

	int getCurrentSongId() { 
		return library.getCurrentSongId(); 
	
	}

	void setVisibleRows(int rows) { library.setVisibleRows(rows); }
	void setTopRowIndex(int row) { library.setTopRowIndex(row); }
	void setCurrentRowIndex(int row) { library.setCurrentRowIndex(row); }

	int getLibrarySize() const { return library.getCount(); }

	Playlist* getOpenPlaylist() {
		if (!currAccount.isPlaylistOpen()) return nullptr;
		int idx = currAccount.getIndexOpenPlaylist();
		if (idx < 0 || idx >= (int)currAccount.getPlaylists().size()) return nullptr;
		return &currAccount.getPlaylists()[idx];
	}

	void moveSelection(int delta) {
		if (activeTab == Tab::LIBRARY) {
			library.moveCursor(delta);
		}
		else if (activeTab == Tab::PLAYLISTS && !currAccount.isPlaylistOpen()) {
			currAccount.movePlaylistCursor(delta);
		}
		else if (Playlist* pl = getOpenPlaylist()) {
			pl->moveSongCursor(delta);
		}
		buildViewData();
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

	bool openPlaylistAt(int index) {
		int count = (int)currAccount.getPlaylists().size();
		if (index < 0 || index >= count) return false;
		currAccount.setPlaylistOpen(true);
		currAccount.setIndexOpenPlaylist(index);
		buildViewData();
		return true;
	}

	void closePlaylist() {
		currAccount.setPlaylistOpen(false);
		currAccount.setIndexOpenPlaylist(-1);
		buildViewData();
	}

	//ALBUMS

	void openAlbum() {

	}

	void closeAlbum() {
	
	}

	//RECOMMENDATIONS


	//SEARCH


};