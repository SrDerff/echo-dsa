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
		echoService.loadGeneralService();
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
					echoService.logout();
					echoService.saveGeneralService();
					echoService.savePersonalService();
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
					echoService.savePersonalService();
					return false;
				}                
				}
			}
			else if (key == 27) { // Esc
				echoService.saveGeneralService();
				echoService.savePersonalService();
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
			if (key == 27) {                          // Esc: salir guardando la cuenta
				echoService.unloadAccountService();
				return false;
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