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
	std::vector<std::string> columnTitles; // etiquetas de columnas por tab
	std::vector<RowData> rows;
	int selectedIndex;
	int topRowIndex;
	PlayerData player;
};

class Interface {
private:
	static const int OUTER = 120;          // ancho total de la ventana
	static const int VISIBLE_ROWS = 17;
	static const int BOX_L = 5;            // columna borde izq de la caja
	static const int BOX_R = 112;           // columna borde der de la caja
	static const int BOX = 104;             // ancho del contenido de la fila

	inline static const std::string RESET = "\x1b[0m";
	inline static const std::string BOLD = "\x1b[1m";
	inline static const std::string DIM = "\x1b[2m";
	inline static const std::string CYAN = "\x1b[36m";
	inline static const std::string GREEN = "\x1b[32m";
	inline static const std::string YELLOW = "\x1b[33m";
	inline static const std::string RED = "\x1b[31m";
	inline static const std::string MAGENTA = "\x1b[95m";
	inline static const std::string BRIGHT_GREEN = "\x1b[92m";
	inline static const std::string REVERSE = "\x1b[7m";
	inline static const std::string FRAME = "\x1b[90m";

	inline static const std::string PLAY_SYM = "\xE2\x96\xB6";
	inline static const std::string NOTE_SYM = "\xE2\x99\xAA";
	inline static const std::string HEART_SYM = "\xE2\x99\xA5";
	inline static const std::string PREV_SYM = "\xE2\x8F\xAE";
	inline static const std::string NEXT_SYM = "\xE2\x8F\xAD";
	inline static const std::string PAUSE_SYM = "\xE2\x8F\xB8";
	inline static const std::string BLOCK_SYM = "\xE2\x96\x88";
	inline static const std::string SHADE_SYM = "\xE2\x96\x91";
	inline static const std::string ELLIPSIS = "\xE2\x80\xA6";

	inline static const std::string C_HD = "\xE2\x94\x80";
	inline static const std::string C_VD = "\xE2\x94\x82";
	inline static const std::string C_TL = "\xE2\x94\x8C";
	inline static const std::string C_TR = "\xE2\x94\x90";
	inline static const std::string C_BL = "\xE2\x94\x94";
	inline static const std::string C_BR = "\xE2\x94\x98";
	inline static const std::string C_LF = "\xE2\x94\x9C";
	inline static const std::string C_RT = "\xE2\x94\xA4";

	// ==== utilidades de texto ====

	static int visWidth(const std::string& s) {
		int w = 0;
		size_t i = 0;
		while (i < s.size()) {
			unsigned char c = (unsigned char)s[i];
			if (c < 0x80) { w += 1; i += 1; }
			else if ((c & 0xE0) == 0xC0) { w += 2; i += 2; }
			else if ((c & 0xF0) == 0xE0) {
				unsigned cp = ((c & 0x0F) << 12)
					| (((unsigned char)s[i + 1] & 0x3F) << 6)
					| ((unsigned char)s[i + 2] & 0x3F);
				w += (cp >= 0x2500 && cp <= 0x25FF) ? 1 : 2;
				i += 3;
			}
			else { w += 2; i += 4; }
		}
		return w;
	}

	// Recorta texto a un ancho maximo en columnas, agregando "..." si corta.
	static std::string fit(const std::string& s, int cols) {
		if (visWidth(s) <= cols) return s;
		std::string out;
		int w = 0;
		size_t i = 0;
		while (i < s.size()) {
			unsigned char c = (unsigned char)s[i];
			int clen = (c < 0x80) ? 1 : ((c & 0xF0) == 0xF0 ? 4 : ((c & 0xE0) == 0xE0 ? 3 : 2));
			std::string g = s.substr(i, clen);
			int gw = visWidth(g);
			if (w + gw > cols - 2) break;
			out += g;
			w += gw;
			i += clen;
		}
		return out + ELLIPSIS;
	}

	// Rellena texto a una anchura fija (en columnas visibles).
	static std::string padCol(const std::string& s, int cols) {
		std::string p = fit(s, cols);
		int w = visWidth(p);
		if (w < cols) p += std::string(cols - w, ' ');
		return p;
	}

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

	static std::string fmtTime(float seconds) {
		int total = (int)seconds;
		int m = total / 60;
		int s = total % 60;
		return std::to_string(m) + ":" + (s < 10 ? "0" + std::to_string(s) : std::to_string(s));
	}

	// ==== composicion por columnas ====

	inline static int gCol = 0;

	// Imprime texto (con optativo estilo ANSI) y suma columnas visibles.
	static void put(const std::string& style, const std::string& text) {
		if (!style.empty()) std::cout << style;
		std::cout << text;
		if (!style.empty()) std::cout << RESET;
		gCol += visWidth(text);
	}

	static void spaces(int n) {
		if (n > 0) {
			std::cout << std::string(n, ' ');
			gCol += n;
		}
	}

	// Inicia un renglon con los bordes exteriores ("│ ").
	static void lineStart() {
		std::cout << FRAME << C_VD << RESET << " ";
		gCol = 2;
	}

	// Rellena hasta el borde derecho del marco y cierra el renglon.
	static void lineEnd() {
		while (gCol < OUTER - 1) spaces(1);
		std::cout << FRAME << C_VD << RESET << "\n";
		gCol = 0;
	}

	// Borde superior/inferior completo del marco.
	static void outerCorner(const std::string& L, const std::string& R) {
		std::cout << FRAME << L;
		gCol = 1;
		while (gCol < OUTER - 1) { std::cout << C_HD; ++gCol; }
		std::cout << R << RESET << "\n";
		gCol = 0;
	}

	// Renglon horizontal de la caja interior (usado para esquinas).
	static void boxH(const std::string& L, const std::string& R) {
		lineStart();
		spaces(3);                       // hasta BOX_L (5)
		put(FRAME, L);
		while (gCol < BOX_R) put(FRAME, C_HD);
		put(FRAME, R);
		lineEnd();
	}

	// Renglon de la caja con celdas (estilo, texto de ancho BOX).
	static void boxRow(const std::vector<std::pair<std::string, std::string>>& cells) {
		lineStart();
		spaces(3);                       // BOX_L = 5
		put(FRAME, C_VD);
		spaces(1);                       // colon hasta 7
		int start = gCol;
		for (auto& c : cells) put(c.first, c.second);
		while (gCol < start + BOX) spaces(1);
		spaces(1);                       // hueco antes del borde derecho
		put(FRAME, C_VD);                // borde derecho col BOX_R-1 == 91
		lineEnd();
	}

	// Renglon del marco con texto centrado.
	static void centeredLine(const std::string& style, const std::string& text) {
		lineStart();
		int w = visWidth(text);
		int p = (OUTER - 2 - w) / 2;
		if (p > 0) spaces(p);
		put(style, text);
		lineEnd();
	}

	// ==== cabecera ====

	static void drawHeader(const ViewData& data) {
		lineStart();
		put(GREEN + BOLD, " " + PLAY_SYM + " ECHO ");
		int lw = visWidth(" " + PLAY_SYM + " ECHO ");
		int rw = visWidth("usuario: ") + visWidth(data.username) + visWidth("[Q] salir") + 3;
		int fill = OUTER - 2 - lw - rw;
		if (fill < 2) fill = 2;
		put(FRAME, std::string(fill, '-'));
		put(CYAN + BOLD, " usuario: " + data.username + " ");
		put(DIM, "[Q] salir");
		lineEnd();
	}

	static void drawTabs(const ViewData& data) {
		Tab tabs[] = { Tab::LIBRARY, Tab::PLAYLISTS, Tab::LIKES, Tab::DISLIKES, Tab::RECOMMENDED };

		int w = 0;
		for (int i = 0; i < 5; ++i) {
			w += visWidth(tabName(tabs[i])) + 2;
			if (i < 4) w += 5;
		}
		int pad = (OUTER - 2 - w) / 2;
		lineStart();
		if (pad > 0) spaces(pad);

		for (int i = 0; i < 5; ++i) {
			if (i > 0) spaces(5);
			std::string n = tabName(tabs[i]);
			if (tabs[i] == data.activeTab)
				put(CYAN + BOLD, "[" + n + "]");
			else
				put(DIM, "  " + n + "  ");
		}
		lineEnd();
	}

	// ==== celdas de las filas ====

	// Etiqueta de columna (colIndex 1..4) o el default.
	static std::string colLabel(const ViewData& d, int index, const std::string& def) {
		if (index >= 1 && (int)d.columnTitles.size() >= index && !d.columnTitles[index - 1].empty())
			return d.columnTitles[index - 1];
		return def;
	}

	typedef std::vector<std::pair<std::string, std::string>> Cells;

	// Rellena la ultima celda para que la fila ocupe exactamente BOX columnas.
	static void appendPad(Cells& c) {
		int used = 0;
		for (auto& e : c) used += visWidth(e.second);
		if (used < BOX) c.emplace_back("", std::string(BOX - used, ' '));
	}

	// Fila de cancion (con estilos) o plana si selected (para resaltar inverso).
	static Cells songCells(const RowData& rd, int row, bool selected) {
		Cells c;

		// marca: "▶" seleccionado, "♪" reproduciendose.
		std::string mark = "  ";
		std::string markStyle;
		if (selected) { mark = "▶ "; markStyle = YELLOW + BOLD; }
		else if (rd.isPlaying) { mark = NOTE_SYM + " "; markStyle = GREEN + BOLD; }
		c.emplace_back(markStyle, mark);

		c.emplace_back("", " ");
		c.emplace_back("", padCol(std::to_string(row) + ".", 3));
		c.emplace_back("", " ");
		c.emplace_back(BOLD, padCol(fit(rd.title, 24), 24));
		c.emplace_back("", " ");
		c.emplace_back(MAGENTA, padCol(fit(rd.artist, 18), 18));
		c.emplace_back("", " ");
		c.emplace_back(BRIGHT_GREEN, padCol(fit(rd.genre, 12), 12));
		c.emplace_back("", "  ");
		c.emplace_back(RED, padCol(HEART_SYM + " " + std::to_string(rd.likes), 8));
		c.emplace_back("", "  ");
		c.emplace_back(RED, rd.isLiked ? HEART_SYM : " ");

		appendPad(c);
		return c;
	}

	// Fila de cancion plana (sin estilos) para el resaltado inverso de la seleccion.
	static Cells songCellsFlat(const RowData& rd, int row) {
		Cells c;
		c.emplace_back("", "▶ ");
		c.emplace_back("", " ");
		c.emplace_back("", padCol(std::to_string(row) + ".", 3));
		c.emplace_back("", " ");
		c.emplace_back("", padCol(fit(rd.title, 24), 24));
		c.emplace_back("", " ");
		c.emplace_back("", padCol(fit(rd.artist, 18), 18));
		c.emplace_back("", " ");
		c.emplace_back("", padCol(fit(rd.genre, 12), 12));
		c.emplace_back("", "  ");
		c.emplace_back("", padCol(HEART_SYM + " " + std::to_string(rd.likes), 8));
		c.emplace_back("", "  ");
		c.emplace_back("", " ");
		appendPad(c);
		return c;
	}

	// Fila de playlist.
	static Cells playlistCells(const RowData& rd, int row, bool selected) {
		Cells c;
		std::string mark = selected ? "▶ " : "  ";
		c.emplace_back(selected ? YELLOW + BOLD : "", mark);
		c.emplace_back("", " ");
		c.emplace_back("", padCol(std::to_string(row) + ".", 3));
		c.emplace_back("", "  ");
		c.emplace_back(BOLD, padCol(fit(rd.title, 40), 40));
		c.emplace_back("", "  ");
		c.emplace_back(CYAN, padCol(std::to_string(rd.playlistSize) + " canciones", 14));
		appendPad(c);
		return c;
	}

	// Cabecera de columnas de la lista.
	static Cells headerCells(const ViewData& data, bool playlists) {
		Cells c;
		if (playlists) {
			std::string name = colLabel(data, 1, "PLAYLIST");
			std::string songs = colLabel(data, 2, "SONGS");
			c.emplace_back("", "  ");
			c.emplace_back("", " ");
			c.emplace_back(DIM, padCol("No.", 3));
			c.emplace_back("", "  ");
			c.emplace_back(CYAN + BOLD, padCol(fit(name, 40), 40));
			c.emplace_back("", "  ");
			c.emplace_back(CYAN + BOLD, padCol(fit(songs, 14), 14));
			appendPad(c);
			return c;
		}
		c.emplace_back("", "  ");
		c.emplace_back("", " ");
		c.emplace_back(DIM, padCol("No.", 3));
		c.emplace_back("", " ");
		c.emplace_back(CYAN + BOLD, padCol(fit(colLabel(data, 1, "TITLE"), 24), 24));
		c.emplace_back("", " ");
		c.emplace_back(CYAN + BOLD, padCol(fit(colLabel(data, 2, "ARTIST"), 18), 18));
		c.emplace_back("", " ");
		c.emplace_back(CYAN + BOLD, padCol(fit(colLabel(data, 3, "GENRE"), 12), 12));
		c.emplace_back("", "  ");
		c.emplace_back(CYAN + BOLD, padCol(fit(colLabel(data, 4, "LIKES"), 8), 8));
		c.emplace_back("", "  ");
		appendPad(c);
		return c;
	}

	// ==== listas ====

	static void drawListTitle(const ViewData& data) {
		lineStart();
		put(CYAN + BOLD, " " + tabName(data.activeTab) + " ");
		put(DIM, " · " + std::to_string((int)data.rows.size()) + " canciones");
		lineEnd();
	}

	static void drawList(const ViewData& data) {
		bool playlist = !data.rows.empty() && data.rows[0].isPlaylist;

		boxH(C_TL, C_TR);            // tapa de la caja
		boxRow(headerCells(data, playlist));   // etiquetas de columnas
		boxH(C_LF, C_RT);           // separador

		int count = (int)data.rows.size();
		for (int i = 0; i < VISIBLE_ROWS; ++i) {
			int d = data.topRowIndex + i;
			if (d > count) break;
			bool sel = (d == data.selectedIndex);
			if (playlist)
				boxRow(playlistCells(data.rows[d - 1], d, sel));
			else {
				if (sel) {
					// fila plana con fondo invertido
					Cells flat = songCellsFlat(data.rows[d - 1], d);
					std::string str;
					for (auto& e : flat) str += e.second;
					lineStart();
					spaces(3);
					put(FRAME, C_VD);
					spaces(1);
					put(REVERSE, padCol(str, BOX));
					spaces(1);
					put(FRAME, C_VD);
					lineEnd();
				}
				else {
					boxRow(songCells(data.rows[d - 1], d, false));
				}
			}
		}
		boxH(C_BL, C_BR);   // piso
	}

	// ==== reproductor ====

	static void drawPlayer(const PlayerData& p) {
		boxH(C_TL, C_TR);

		// renglon "NOW"
		{
			Cells c;
			if (p.state == PlayerState::STOPPED || p.songName.empty()) {
				c.emplace_back(DIM, " " + PLAY_SYM + "   --- sin cancion ---");
			}
			else {
				c.emplace_back(GREEN + BOLD, " " + PLAY_SYM + "  ");
				c.emplace_back(BOLD, fit(p.songName, 26));
				c.emplace_back(DIM, "  -  " + fit(p.artist, 16));
				// controls a la derecha
				std::string ctrl = PREV_SYM + "  " + (p.state == PlayerState::PAUSED ? PLAY_SYM : PAUSE_SYM) + "  " + NEXT_SYM;
				int used = 4 + (int)visWidth(fit(p.songName, 26)) + 4 + (int)visWidth(fit(p.artist, 16));
				int gutter = BOX - used - visWidth(ctrl) - 6;
				c.emplace_back("", std::string(gutter < 1 ? 1 : gutter, ' '));
				c.emplace_back(BOLD, ctrl);
			}
			appendPad(c);
			boxRow(c);
		}

		// renglon de estado y barra de progreso
		{
			Cells c;
			std::string txt, col;
			switch (p.state) {
			case PlayerState::PLAYING: txt = "REPRODUCIENDO"; col = GREEN + BOLD; break;
			case PlayerState::PAUSED: txt = "PAUSADO"; col = YELLOW + BOLD; break;
			default: txt = "DETENIDO"; col = DIM; break;
			}
			c.emplace_back("", "  ");
			c.emplace_back(DIM, "ESTADO:");
			c.emplace_back("", " ");
			c.emplace_back(col, txt);
			c.emplace_back("", "      ");

			int filled = 0;
			if (p.length > 0) filled = (int)(p.position / p.length * 20);
			if (filled < 0) filled = 0;
			if (filled > 20) filled = 20;
			std::string bar;
			for (int i = 0; i < 20; ++i)
				bar += (i < filled) ? BLOCK_SYM : SHADE_SYM;
			c.emplace_back(GREEN, bar);

			c.emplace_back(DIM, "  " + fmtTime(p.position) + " / " + fmtTime(p.length) + "  ");
			appendPad(c);
			boxRow(c);
		}

		boxH(C_BL, C_BR);
	}

	// ==== footer ====

	static void drawFooter() {
		centeredLine(DIM, "   ↑/↓ mover    ·    L P K D R tabs    ·    Enter abrir/reproducir    ·    Esc volver    ·    Q salir");
		centeredLine(DIM, "   Espacio pausa/reanudar    ·    M me gusta    ·    N no me gusta");
	}

public:
	// Inicializa la consola (UTF-8 + ANSI) y oculta el cursor.
	static void init() {
		SetConsoleOutputCP(CP_UTF8);
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;
		if (GetConsoleMode(h, &mode)) {
			SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
		std::cout << "\x1b[?25l";
	}

	// Dibuja el cuadro completo.
	void render(const ViewData& data) {
		std::cout << "\x1b[2J\x1b[H";
		outerCorner(C_TL, C_TR);
		drawHeader(data);
		lineStart(); lineEnd();          // linea en blanco
		drawTabs(data);
		lineStart(); lineEnd();          // separa tabs del panel
		drawListTitle(data);
		drawList(data);
		lineStart(); lineEnd();
		drawPlayer(data.player);
		lineStart(); lineEnd();
		drawFooter();
		outerCorner(C_BL, C_BR);
	}
};