#pragma once
#include "EchoService.h"
#include "Interface.h"
#include <conio.h>

enum class Screen { WELCOME, LOGIN, REGISTER, MAIN };

class AppController {
private:
	EchoService echoService;
	Interface ui;

	Screen screen = Screen::WELCOME;
	int welcomeIndex = 0;

	std::string userInput;
	std::string passInput;
	bool focusUser = true;
	std::string errorMsg;

public:
	AppController() {
		echoService.loadGeneralService();
		ui.init();
		ui.renderWelcome(welcomeIndex);
	}

	void run() {
		while (true) {
			int key = _getch();
			if (key == 0 || key == 0xE0) {
				key = _getch(); // teclas de extension (flechas, etc.)
			}
			if (!dispatch(key)) break;
		}
	}

	bool dispatch(int key) {
		switch (screen) {
		case Screen::WELCOME: return welcomeKey(key);
		case Screen::LOGIN: return authKey(key, false);
		case Screen::REGISTER: return authKey(key, true);
		case Screen::MAIN: return mainKey(key);
		}
		return true;
	}

	bool welcomeKey(int key) {
		if (key == 72 && welcomeIndex > 0) {          // flecha arriba
			int old = welcomeIndex;
			welcomeIndex--;
			ui.updateWelcome(old, welcomeIndex);
		}
		else if (key == 80 && welcomeIndex < 2) {     // flecha abajo
			int old = welcomeIndex;
			welcomeIndex++;
			ui.updateWelcome(old, welcomeIndex);
		}
		else if (key == 13) {                          // Enter
			switch (welcomeIndex) {
			case 0: screen = Screen::LOGIN; clearFields(); ui.renderLogin("", "", true, ""); break;
			case 1: screen = Screen::REGISTER; clearFields(); ui.renderRegister("", "", true, ""); break;
			case 2: return false;                      // salir
			}
		}
		else if (key == 27) {                          // Esc
			return false;
		}
		return true;
	}

	bool authKey(int key, bool isRegister) {
		if (key == 13) {                               // Enter
			if (!focusUser) {
				bool ok = isRegister
					? echoService.registerAccount(userInput, passInput)
					: echoService.login(userInput, passInput);
				if (ok) {
					screen = Screen::MAIN;
					ui.render(echoService.getViewData());
				}
				else {
					errorMsg = isRegister
						? "El nombre de usuario ya existe."
						: "Usuario o contrasena incorrectos.";
					ui.updateAuthError(errorMsg);
				}
			}
			else {
				focusUser = false;
				ui.updateAuthInputs(userInput, passInput, focusUser);
			}
		}
		else if (key == 9 || key == 80 || key == 72) { // Tab / flechas: cambiar campo
			focusUser = !focusUser;
			ui.updateAuthInputs(userInput, passInput, focusUser);
		}
		else if (key == 27) {                          // Esc: volver a la bienvenida
			screen = Screen::WELCOME;
			ui.renderWelcome(welcomeIndex);
		}
		else if (key == 8) {                           // Backspace
			std::string& target = focusUser ? userInput : passInput;
			if (!target.empty()) target.pop_back();
			ui.updateAuthInputs(userInput, passInput, focusUser);
		}
		else if (key >= 32 && key <= 126) {            // caracteres visibles
			std::string& target = focusUser ? userInput : passInput;
			if (target.size() < 40) target += (char)key;
			ui.updateAuthInputs(userInput, passInput, focusUser);
		}
		return true;
	}

	bool mainKey(int key) {
		if (key == 27) {                               // Esc: salir guardando la cuenta
			echoService.unloadAccountService();
			return false;
		}
		return true;
	}

	void clearFields() {
		userInput.clear();
		passInput.clear();
		focusUser = true;
		errorMsg.clear();
	}
};
