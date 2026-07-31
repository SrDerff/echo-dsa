#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include "json.hpp"
#include <string>
#include "Library.h"
#include "Album.h"
#include "Account.h"

using json = nlohmann::json;
const std::string PATH_LIBRARY = ".\\..\\data\\songList.json";
const std::string PATH_ALBUMS = ".\\..\\data\\albumsList.json";
const std::string PATH_ACCOUNT = ".\\..\\data\\users\\usersList.json";

class FileManager {
public:
	static void loadGeneralData(HashTable<int, Song>& allSongs, std::vector<Album>&albums) {
		loadLibrary(allSongs);
		loadAlbums(albums);
	}

	static void saveGeneralData(HashTable<int, Song>& allSongs, std::vector<Album>&albums) {
		saveLibrary(allSongs);
		saveAlbums(albums);
	}

    static void loadLibrary(HashTable<int, Song>& allSongs) {
        std::ifstream file(PATH_LIBRARY);

        if (!file.is_open()) {
            std::cerr << "No se pudo abrir el archivo de canciones.\n";
            return;
        }

        json data;
        file >> data;

        for (const auto& songData : data) {
            int idSong = songData["id"];
            std::string name = songData["name"];
            std::string author = songData["author"];
            float length = songData["length"];
            long long playCount = songData["play_count"];
            long long likesCount = songData["likes_count"];
            long long dislikesCount = songData["dislikes_count"];
            std::string sourcePath = songData["path"];
            std::string genre = songData["genre"];

            allSongs.insert(
                idSong,
                Song(
                    idSong,
                    name,
                    author,
                    length,
                    playCount,
                    likesCount,
                    dislikesCount,
                    genre,
                    sourcePath
                )
            );
        }
    }

	static void saveLibrary(HashTable<int, Song>& allSongs) {
		json data = json::array();

		for (auto it = allSongs.begin(); it != allSongs.end(); ++it) {
			Song& song = it.getValue();
			json songJson;
			songJson["id"] = song.getIdSong();
			songJson["name"] = song.getName();
			songJson["author"] = song.getAuthor();
			songJson["length"] = song.getLength();
			songJson["play_count"] = song.getPlayCount();
			songJson["likes_count"] = song.getLikesCount();
			songJson["dislikes_count"] = song.getDislikesCount();
			songJson["genre"] = song.getGenre();
			songJson["path"] = song.getSourcePath();
			data.push_back(songJson);
		}

		std::ofstream file(PATH_LIBRARY);
		if (!file.is_open()) {
			std::cerr << "No se pudo escribir el archivo de canciones.\n";
			return;
		}
		file << data.dump(4);
	}

	static void loadAlbums(std::vector<Album>&albums) {
		std::ifstream file(PATH_ALBUMS);

        if (!file.is_open()) {
            std::cerr << "No se pudo abrir el archivo de albums.\n";
            return;
        }

        json data;
        file >> data;

        for (const auto& albumData : data) {
            int idAlbum = albumData["id"];
            std::string name = albumData["name"];
            std::string author = albumData["author"];
            size_t songsCount = albumData["songsCount"];
            LinkedList<int>idSongs;
            for (const auto& songId : albumData["songsIds"]) {
                idSongs.pushBack(songId);
            }
            albums.emplace_back(
                Album(
                    idAlbum,
                    name,
                    author,
                    songsCount,
                    idSongs
                )
            );
        }
	}

	static void saveAlbums(std::vector<Album>& albums) {
		json data = json::array();

		for (Album& album : albums) {
			json albumJson;
			albumJson["id"] = album.getIdAlbum();
			albumJson["name"] = album.getName();
			albumJson["author"] = album.getAuthor();
			albumJson["songsCount"] = album.getSize();
			albumJson["songsIds"] = json::array();
			for (int songId : album.getIdsSongs()) {
				albumJson["songsIds"].push_back(songId);
			}
			data.push_back(albumJson);
		}

		std::ofstream file(PATH_ALBUMS);
		if (!file.is_open()) {
			std::cerr << "No se pudo escribir el archivo de albums.\n";
			return;
		}
		file << data.dump(4);
	}

	static void loadPersonalData(Account& currAccount) {
        std::ifstream file(PATH_ACCOUNT);
        if (!file.is_open()) {
            std::cerr << "No se pudo abrir el archivo de usuarios.\n";
            return;
        }

        json data;
        file >> data;

        int targetId = currAccount.getIdAccount();

        for (const auto& acc : data) {
            if ((int)acc["id"] != targetId) continue;

            currAccount.setUsername(acc["user"]);
            currAccount.setPasswordHash(acc["password"]);
            currAccount.setVip(acc["vip"]);

            for (const auto& pl : acc["playlists"]) {
                std::string playlistName = pl["name"];
                int playlistId = pl.value("id", 0);
                if (currAccount.addPlaylist(playlistName)) {
                    currAccount.getPlaylists().back().setIdPlaylist(playlistId);
                    for (const auto& songId : pl["songs"]) {
                        currAccount.addSongToPlaylist(playlistName, songId);
                    }
                }
            }
            for (const auto& idSong : acc["likedSongs"]) {
                currAccount.likeSong(idSong);
            }
            for (const auto& idSong : acc["dislikedSongs"]) {
                currAccount.dislikeSong(idSong);
            }
            return;
        }
	}

	static void savePersonalData(Account& currAccount) {
		json data = json::array();

		std::ifstream file(PATH_ACCOUNT);
		if (file.is_open()) {
			file >> data;
		}

		bool found = false;
		for (auto& acc : data) {
			if ((int)acc["id"] == currAccount.getIdAccount()) {
				acc = buildAccountJson(currAccount);
				found = true;
				break;
			}
		}
		if (!found) {
			data.push_back(buildAccountJson(currAccount));
		}

		std::ofstream outFile(PATH_ACCOUNT);
		if (!outFile.is_open()) {
			std::cerr << "No se pudo escribir el archivo de usuarios.\n";
			return;
		}
		outFile << data.dump(4);
	}

private:
	static json buildAccountJson(Account& acc) {
		json accJson;
		accJson["id"] = acc.getIdAccount();
		accJson["user"] = acc.getUsername();
		accJson["password"] = acc.getPasswordHash();
		accJson["vip"] = acc.isVip();

		accJson["playlists"] = json::array();
		for (Playlist& pl : acc.getPlaylists()) {
			json plJson;
			plJson["id"] = pl.getIdPlaylist();
			plJson["name"] = pl.getName();
			plJson["songs"] = json::array();
			for (int songId : pl.getSongsIds()) {
				plJson["songs"].push_back(songId);
			}
			accJson["playlists"].push_back(plJson);
		}

		accJson["likedSongs"] = json::array();
		for (int idSong : acc.getLikedSongs()) {
			accJson["likedSongs"].push_back(idSong);
		}

		accJson["dislikedSongs"] = json::array();
		for (int idSong : acc.getDislikedSongs()) {
			accJson["dislikedSongs"].push_back(idSong);
		}

		return accJson;
	}
};
