#pragma once
#include "EchoService.h"
#include "Interface.h"
#include <conio.h>
#include <iostream>

enum class Screen { WELCOME, LOGIN, REGISTER, MAIN };

class AppController {
private:
	EchoService echoService;
	Interface ui;

	Screen screen = Screen::WELCOME;
	int welcomeIndex = 0;
public:
	AppController() {
		ui.init();
		ui.renderWelcome(welcomeIndex);
	}

	void run() {
		while (true) {
			switch (screen) {
			case Screen::WELCOME:
				if (!welcomeRun()) return;
				break;
			case Screen::LOGIN:
				authFlow(false);
				break;
			case Screen::REGISTER:
				authFlow(true);
				break;
			case Screen::MAIN:
				if (!mainRun()) {
					screen = Screen::WELCOME;
					ui.renderWelcome(0);
					welcomeIndex = 0;
				}
				break;
			}
		}
	}

private:
	int readKey() {
		int key = _getch();
		if (key == 0 || key == 0xE0) key = _getch(); // teclas de extension (flechas, etc.)
		return key;
	}

	bool welcomeRun() {
		while (screen == Screen::WELCOME) {
			int key = readKey();
			if (key == 13) {                          // Enter
				switch (welcomeIndex) {
				case 0: screen = Screen::LOGIN; break;
				case 1: screen = Screen::REGISTER; break;
				case 2: { // salir
					echoService.saveGeneralService();
					echoService.loadAccountService();
					return false;
				}                
				}
			}
			else if (key == 27) { // Esc
				echoService.saveGeneralService();
				echoService.unloadAccountService();
				return false;
			}
			else if (key == 72 && welcomeIndex > 0) {  // flecha arriba
				int old = welcomeIndex;
				welcomeIndex--;
				ui.updateWelcome(old, welcomeIndex);
			}
			else if (key == 80 && welcomeIndex < 2) { // flecha abajo
				int old = welcomeIndex;
				welcomeIndex++;
				ui.updateWelcome(old, welcomeIndex);
			}
		}
		return true;
	}

	bool mainRun() {
		while (screen == Screen::MAIN) {
			int key = readKey();
			switch (echoService.getActiveTab()) {
				case Tab::LIBRARY:
					return mainLibraryRun(key);
					break;
				case Tab::PLAYLISTS:
					// Implement playlist handling here
					break;
				case Tab::LIKES:
					// Implement likes handling here
					break;
				case Tab::RECOMMENDED:
					// Implement recommended handling here
					break;
				case Tab::HISTORIAL:
					// Implement historial handling here
					break;
				case Tab::SEARCH:
					// Implement search handling here
					break;
			}
		}
		return true;
	}

	bool mainLibraryRun(int key) {
		if (key == 27) {                          // Esc: salir guardando la cuenta
			echoService.stopSong();
			echoService.unloadAccountService();
			echoService.saveGeneralService();
			echoService.logout();
			return false;
		}
		if (key == 13) {                          // Enter: reproducir cancion
			echoService.playSong(echoService.getCurrentSongId());
			echoService.updatePlayerData(
				PlayerData{
					echoService.getSongById(echoService.getCurrentSongId()).getName(),
					echoService.getSongById(echoService.getCurrentSongId()).getAuthor(),
					PlayerState::PLAYING,
					echoService.getSongById(echoService.getCurrentSongId()).getLength(),
					0.0f
				}
			);
			ui.updateHud(echoService.getViewData());
		}
		if (key == 72) { // flecha arriba
			int oldAbs = echoService.getTopRowIndex() + echoService.getCurrentRowIndex();
			int oldTop = echoService.getTopRowIndex();
			echoService.moveSelection(-1);
			const ViewData& vd = echoService.getViewData();
			if (echoService.getTopRowIndex() != oldTop) {
				ui.updateRows(vd);
			}
			else {
				ui.updateRow(vd, oldAbs + 1);
				ui.updateRow(vd, echoService.getTopRowIndex() + echoService.getCurrentRowIndex() + 1);
			}
		}
		if (key == 80) { // flecha abajo
			int oldAbs = echoService.getTopRowIndex() + echoService.getCurrentRowIndex();
			int oldTop = echoService.getTopRowIndex();
			echoService.moveSelection(1);
			const ViewData& vd = echoService.getViewData();
			if (echoService.getTopRowIndex() != oldTop) {
				ui.updateRows(vd);
			}
			else {
				ui.updateRow(vd, oldAbs + 1);
				ui.updateRow(vd, echoService.getTopRowIndex() + echoService.getCurrentRowIndex() + 1);
			}
		}
		if (key == 32) { // barra espaciadora: pausar/reanudar
			if(echoService.getViewData().player.state == PlayerState::PLAYING
				&& echoService.getViewData().player.songName == echoService.getSongById(echoService.getCurrentSongId()).getName()
				) {
				echoService.pauseSong();
				echoService.getViewData().player.state = PlayerState::PAUSED;
			}
			else if(echoService.getViewData().player.state == PlayerState::PAUSED
				&& echoService.getViewData().player.songName == echoService.getSongById(echoService.getCurrentSongId()).getName()
				) {
				echoService.resumeSong();
				echoService.getViewData().player.state = PlayerState::PLAYING;
			}
		}

		return true;
	}

	void authFlow(bool isRegister) {
		if (isRegister) ui.renderRegister("");
		else ui.renderLogin("");

		while (true) {
			std::string user, pass;

			ui.placeAuthCursor(0);
			std::getline(std::cin, user);
			if (user.empty()) {                       // usuario vacio: volver al menu
				screen = Screen::WELCOME;
				ui.renderWelcome(welcomeIndex);
				return;
			}

			ui.placeAuthCursor(1);
			std::getline(std::cin, pass);

			bool ok = isRegister
				? echoService.registerAccount(user, pass)
				: echoService.login(user, pass);
			if (ok) {
				screen = Screen::MAIN;
				ui.render(echoService.getViewData());
				return;
			}

			ui.clearAuthFields();
			ui.updateAuthError(isRegister
				? "El nombre de usuario ya existe."
				: "Usuario o contrasena incorrectos.");
		}
	}
};