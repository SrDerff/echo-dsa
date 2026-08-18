#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

class SoundAPI {
private:
	std::string deviceAlias;
	bool deviceOpen;
	bool paused;
	bool stopped;
	long long volume;
public:
	SoundAPI() : deviceAlias("echo_media"), deviceOpen(false), paused(false), stopped(true), volume(50) {}

	~SoundAPI() {
		close();
	}

	bool load(const std::string& sourcePath) {
		if (deviceOpen) close();

		std::string command = "open \"" + sourcePath + "\"";
		if (sourcePath.size() >= 4) {
			std::string ext = sourcePath.substr(sourcePath.size() - 4);
			for (auto& c : ext) c = (char)tolower(c);
			if (ext == ".wav") command += " type waveaudio";
			else if (ext == ".mp3") command += " type mpegvideo";
		}
		command += " alias " + deviceAlias;

		if (sendCommand(command) != 0) return false;

		std::string timeFormat = "set " + deviceAlias + " time format milliseconds";
		sendCommand(timeFormat);

		deviceOpen = true;
		paused = false;
		stopped = true;
		return true;
	}

	bool play() {
		if (!deviceOpen) return false;

		std::string command;
		if (paused) {
			command = "play " + deviceAlias;
		}
		else {
			command = "play " + deviceAlias + " from 0";
		}

		if (sendCommand(command) != 0) return false;

		paused = false;
		stopped = false;
		return true;
	}

	bool resume() {
		if (!deviceOpen || !paused) return false;

		std::string command = "play " + deviceAlias;
		if (sendCommand(command) != 0) return false;

		paused = false;
		stopped = false;
		return true;
	}

	bool pause() {
		if (!deviceOpen || paused) return false;

		std::string command = "pause " + deviceAlias;
		if (sendCommand(command) != 0) return false;

		paused = true;
		return true;
	}

	bool stop() {
		if (!deviceOpen) return false;

		std::string command = "stop " + deviceAlias;
		if (sendCommand(command) != 0) return false;

		paused = false;
		stopped = true;
		return true;
	}

	bool close() {
		if (!deviceOpen) return false;

		std::string command = "close " + deviceAlias;
		if (sendCommand(command) != 0) return false;

		deviceOpen = false;
		paused = false;
		stopped = true;
		return true;
	}

	bool isPlaying() {
		return getMode() == "playing";
	}

	bool isPaused() {
		return paused;
	}

	bool isLoaded() {
		return deviceOpen;
	}

	bool isStopped() {
		return stopped;
	}

	long long getPosition() {
		if (!deviceOpen) return 0;
		return queryCommand("status " + deviceAlias + " position");
	}

	long long getLength() {
		if (!deviceOpen) return 0;
		return queryCommand("status " + deviceAlias + " length");
	}

	void setVolume(long long vol) {
		if (vol < 0) vol = 0;
		if (vol > 100) vol = 100;
		volume = vol;

		if (!deviceOpen) return;

		std::string command = "setaudio " + deviceAlias + " volume to " + std::to_string(volume * 10);
		sendCommand(command);
	}

	long long getVolume() {
		return volume;
	}

	std::string getLastError() {
		return lastError;
	}

private:
	std::string lastError;

	MCIERROR sendCommand(const std::string& command) {
		MCIERROR result = mciSendStringA(command.c_str(), nullptr, 0, nullptr);
		if (result != 0) setLastError(result);
		return result;
	}

	long long queryCommand(const std::string& command) {
		char buffer[128];
		MCIERROR result = mciSendStringA(command.c_str(), buffer, sizeof(buffer), nullptr);
		if (result != 0) {
			setLastError(result);
			return 0;
		}
		return atoll(buffer);
	}

	std::string getMode() {
		char buffer[128];
		std::string command = "status " + deviceAlias + " mode";
		MCIERROR result = mciSendStringA(command.c_str(), buffer, sizeof(buffer), nullptr);
		if (result != 0) {
			setLastError(result);
			return "";
		}
		return buffer;
	}

	void setLastError(MCIERROR error) {
		char buffer[256];
		mciGetErrorStringA(error, buffer, sizeof(buffer));
		lastError = buffer;
	}
};
