#pragma once

#include <string>

class Artist {
private:
	int idArtist;
	std::string name;
	/*
	TO-DO
	album
	*/
public:
	int getIdArtist() { return idArtist; }
	std::string getName() { return name; }
};