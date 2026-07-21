#pragma once

#include <string>

class Song {
private:
	int idSong;
	std::string name;
	std::string author;
	float length;
	long long playCount;
	long long likesCount;
	long long dislikesCount;
	std::string sourcePath;
	std::string genre;
public:
	Song(
		int idSong,
		std::string name,
		std::string author,
		float length,
		long long playCount,
		long long likesCount,
		long long dislikesCount,
		std::string sourcePath,
		std::string genre
	) {
		this->idSong = idSong;
		this->name = author;
		this->length = length;
		this->playCount = playCount;
		this->likesCount = likesCount;
		this->dislikesCount = dislikesCount;
		this->sourcePath = sourcePath;
		this->genre = genre;
	}
	~Song(){}
	int getIdSong() {
	
	}
	std::string getname() { return name; }
	std::string getAuthor() { return author; }
	float getLength() { return length; }
	long long getPlayCount() { return playCount; }
	long long getLikesCount() { return likesCount; }
	long long getDislikesCount() { return dislikesCount; }
	std::string getSourcePath() { return sourcePath; }
	std::string getGenre() { return genre; }
};