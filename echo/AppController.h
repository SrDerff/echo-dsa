#pragma once
#include "EchoService.h"
#include "Interface.h"
#include <conio.h>


enum class KEYS {
	QUIT = 27 // ESC
};

class AppController {
private:
	EchoService echoService;
	Interface ui;
public:
	AppController() {
		echoService.loadGeneralService();
		ui.init();
		ui.render(echoService.getViewData());
	}
	~AppController() {

	}
	
	void handleInput(KEYS input) {
		switch (input) {
		case KEYS::QUIT:
			echoService.quit();
			break;
		default:
			break;
		}
	}
	
	void run() {
		char input;
		while (true) {
			if (kbhit()) {
				std::cin >> input;
				handleInput(static_cast<KEYS>(input));
			}
		}
	}
};