#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Tabs del HUD superior (todos visibles, el activo se resalta).
enum class Tab { LIBRARY, PLAYLISTS, LIKES, DISLIKES, RECOMMENDED };

// Estado del reproductor reflejado en la barra inferior.
enum class PlayerState { STOPPED, PLAYING, PAUSED };

// Una fila del panel central: cancion o playlist segun isPlaylist.
struct RowData {
	std::string title;
	std::string artist;
	std::string genre;
	long long likes;
	bool isLiked;
	bool isPlaylist;
	long long playlistSize;
	bool isPlaying;
};

// Datos de la cancion activa + estado + progreso del reproductor.
struct PlayerData {
	std::string songName;
	std::string artist;
	PlayerState state;
	float length;
	float position;
};

// Contrato de datos que la UI imprime en cada frame (todo llega junto).
struct ViewData {
	Tab activeTab;
	std::string username;
	std::vector<RowData> rows;
	int selectedIndex;
	int topRowIndex;
	PlayerData player;
};

class Interface {
private:
	static const int PANEL_WIDTH = 90;
	static const int VISIBLE_ROWS = 17;

	// Codigos de color / formato ANSI.
	inline static const std::string RESET = "\x1b[0m";
	inline static const std::string BOLD = "\x1b[1m";
	inline static const std::string DIM = "\x1b[2m";
	inline static const std::string CYAN = "\x1b[36m";
	inline static const std::string GREEN = "\x1b[32m";
	inline static const std::string YELLOW = "\x1b[33m";
	inline static const std::string RED = "\x1b[31m";
	inline static const std::string REVERSE = "\x1b[7m";

	// Simbolos UTF-8: play, nota musical, corazon, anterior/siguiente, pausa, bloque de barra, sombra de barra, ellipsis.
	inline static const std::string PLAY_SYM = "\xE2\x96\xB6";
	inline static const std::string NOTE_SYM = "\xE2\x99\xAA";
	inline static const std::string HEART_SYM = "\xE2\x99\xA5";
	inline static const std::string PREV_SYM = "\xE2\x8F\xAE";
	inline static const std::string NEXT_SYM = "\xE2\x8F\xAD";
	inline static const std::string PAUSE_SYM = "\xE2\x8F\xB8";
	inline static const std::string BLOCK_SYM = "\xE2\x96\x88";
	inline static const std::string SHADE_SYM = "\xE2\x96\x91";
	inline static const std::string ELLIPSIS = "\xE2\x80\xA6";

	// Nombre legible de cada tab.
	static std::string tabName(Tab t) {
		switch (t) {
		case Tab::LIBRARY: return "LIBRARY";
		case Tab::PLAYLISTS: return "PLAYLISTS";
		case Tab::LIKES: return "LIKES";
		case Tab::DISLIKES: return "DISLIKES";
		case Tab::RECOMMENDED: return "RECOMMEN";
		}
		return "?";
	}
	
	// Rellena o recorta a ancho fijo (por bytes).
	static std::string pad(const std::string& s, size_t width) {
		if (s.size() >= width) return s.substr(0, width);
		return s + std::string(width - s.size(), ' ');
	}

	// Corta con "..." si excede el ancho.
	static std::string truncate(const std::string& s, size_t width) {
		if (s.size() <= width) return s;
		if (width <= 1) return s.substr(0, width);
		return s.substr(0, width - 1) + ELLIPSIS;
	}

	// Segundos -> formato m:ss.
	static std::string fmtTime(float seconds) {
		int total = (int)seconds;
		int m = total / 60;
		int s = total % 60;
		std::string ss = (s < 10) ? "0" + std::to_string(s) : std::to_string(s);
		return std::to_string(m) + ":" + ss;
	}

		// HUD superior: logo + usuario + tabs fijos, el activo resaltado.
	static void drawTabs(const ViewData& data) {
		std::cout << " " << GREEN << PLAY_SYM << " Echo" << RESET
			<< std::string(40, ' ')
			<< DIM << "usuario: " << RESET << data.username
			<< DIM << "   [salir: Q]" << RESET << "\n";

		std::cout << " ";
		Tab tabs[] = { Tab::LIBRARY, Tab::PLAYLISTS, Tab::LIKES, Tab::DISLIKES, Tab::RECOMMENDED };
		for (Tab t : tabs) {
			if (t == data.activeTab) {
				std::cout << CYAN << BOLD << "[" << tabName(t) << "]" << RESET << "  ";
			}
			else {
				std::cout << DIM << " " << tabName(t) << " " << RESET << "  ";
			}
		}
		std::cout << "\n";
	}

	// Compone el texto ASCII de una fila (cancion o playlist).
	static std::string buildRowText(const RowData& rd, int row) {
		std::string num = pad(std::to_string(row), 3);

		if (rd.isPlaylist) {
			return " " + num + ". " + pad(truncate(rd.title, 30), 30)
				+ "(" + std::to_string(rd.playlistSize) + " canciones)";
		}

		std::string title = pad(truncate(rd.title, 26), 26);
		std::string artist = pad(truncate(rd.artist, 18), 18);
		std::string genre = pad(truncate(rd.genre, 12), 12);
		std::string likes = pad(HEART_SYM + " " + std::to_string(rd.likes), 7);
		std::string likeMark = rd.isLiked ? " " + HEART_SYM : "    ";
		std::string playMark = rd.isPlaying ? " " + NOTE_SYM : "    ";

		return " " + num + ". " + title + " " + artist + " " + genre + " " + likes + likeMark + playMark;
	}

	// Pinta una fila; resalta en inverso la seleccionada.
	static void drawRow(const ViewData& data, int row) {
		std::string line = buildRowText(data.rows[row - 1], row);
		if (row == data.selectedIndex) {
			std::cout << REVERSE << pad(line, PANEL_WIDTH) << RESET << "\n";
		}
		else {
			std::cout << pad(line, PANEL_WIDTH) << "\n";
		}
	}

	// Lista central con scroll (desde topRowIndex, max VISIBLE_ROWS filas).
	static void drawPanel(const ViewData& data) {
		std::cout << " " << CYAN << BOLD << tabName(data.activeTab) << RESET
			<< DIM << "   (↑/↓ mover | Enter abrir/reproducir | Esc volver)" << RESET << "\n";
		std::cout << " " << std::string(PANEL_WIDTH, '-') << "\n";

		int count = (int)data.rows.size();
		if (count == 0) {
			std::cout << DIM << " " << pad("(sin canciones)", PANEL_WIDTH - 1) << RESET << "\n";
		}

		for (int i = 0; i < VISIBLE_ROWS; ++i) {
			int row = data.topRowIndex + i;
			if (row > count) break;
			drawRow(data, row);
		}

		std::cout << " " << std::string(PANEL_WIDTH, '-') << "\n";
	}

	// Barra █/░ segun proporcion posicion/duracion (ancho 20).
	static std::string buildProgressBar(float pos, float length) {
		int filled = 0;
		if (length > 0) {
			filled = (int)((pos / length) * 20);
			if (filled < 0) filled = 0;
			if (filled > 20) filled = 20;
		}
		std::string bar;
		for (int i = 0; i < 20; ++i) bar += (i < filled) ? BLOCK_SYM : SHADE_SYM;
		return bar;
	}

	// Barra fija inferior: cancion actual + controles + estado + progreso.
	static void drawPlayer(const PlayerData& p) {
		std::cout << " " << std::string(PANEL_WIDTH, '=') << "\n";

		if (p.state == PlayerState::STOPPED || p.songName.empty()) {
			std::cout << " " << DIM << PLAY_SYM << " NOW: --- sin canción ---" << RESET
				<< std::string(30, ' ') << "   " << DIM << PREV_SYM << "  " << PLAY_SYM << "  " << NEXT_SYM << RESET << "\n";
		}
		else {
			std::cout << " " << GREEN << PLAY_SYM << " NOW: " << RESET
				<< BOLD << truncate(p.songName, 28) << RESET
				<< DIM << " - " << RESET << truncate(p.artist, 18)
				<< std::string(12, ' ')
				<< DIM << PREV_SYM << "  " << RESET
				<< ((p.state == PlayerState::PAUSED) ? GREEN + PLAY_SYM : YELLOW + PAUSE_SYM) << RESET
				<< DIM << "  " << NEXT_SYM << RESET << "\n";
		}

		std::string stateText;
		std::string color;
		switch (p.state) {
		case PlayerState::PLAYING: stateText = "REPRODUCIENDO"; color = GREEN; break;
		case PlayerState::PAUSED: stateText = "PAUSADO"; color = YELLOW; break;
		default: stateText = "DETENIDO"; color = DIM; break;
		}

		std::cout << "     " << DIM << "ESTADO: " << RESET << color << BOLD << stateText << RESET
			<< "  " << buildProgressBar(p.position, p.length)
			<< "  " << DIM << fmtTime(p.position) << " / " << fmtTime(p.length) << RESET << "\n";
	}

public:
	// Configura la consola (UTF-8 + ANSI) y oculta el cursor. Se llama una vez al inicio.
	static void init() {
		SetConsoleOutputCP(CP_UTF8);
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;
		if (GetConsoleMode(h, &mode)) {
			SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
		std::cout << "\x1b[?25l";
	}

	// Limpia la pantalla y dibuja las 3 zonas (tabs, panel, reproductor).
	void render(const ViewData& data) {
		std::cout << "\x1b[2J\x1b[H";
		drawTabs(data);
		drawPanel(data);
		drawPlayer(data.player);
		std::cout << "\n " << DIM
			<< "L=Library  P=Playlists  K=Likes  D=Dislikes  R=Recomm  |  M=Me gusta  N=No me gusta  Espacio=Play/Pausa  Q=Salir"
			<< RESET << "\n";
	}
};
