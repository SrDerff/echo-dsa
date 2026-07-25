#pragma once
#include <fstream>
#include "json.hpp"
#include <string>
#include "Library.h"

using json = nlohmann::json;

static class FileManager {
private:
	const std::string PATH_LIBRARY = ".\\..\\data\\songsList.json";
public:
	/*TO-DO*/
	void loadGeneralData(HashTable<int, Song>&allSongs) {
		loadLibrary(allSongs);
	}

	void loadLibrary(HashTable<int, Song>& allSongs) {
		std::ifstream file(PATH_LIBRARY);
		json data;
		file >> data;

		int idSong = data["id"];
		std::string name = data["name"];
		std::string author = data["author"];
		float length = data["length"];
		long long playCount = data["play_count"];
		long long likesCount = data["likes_count"];
		long long dislikesCount = data["dislikes_count"];
		std::string sourcePath = data["path"];
		std::string genre = data["genre"];

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

	void loadPersonalData() {

	}
};