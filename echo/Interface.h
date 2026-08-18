#pragma once

#define NOMINMAX

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Tabs del HUD superior (diseno del modelo de referencia).
enum class Tab { LIBRARY, PLAYLISTS, LIKES, RECOMMENDED, HISTORIAL, SEARCH };

// Estado del reproductor reflejado en la UI.
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
	float duration;      // segundos (columna Duration del modelo)
	bool isPlaying;
};

// Datos de la cancion activa + estado del reproductor.
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
	// ==== paleta RGB 24-bit (modelo de referencia) ====
	static const int BG_R = 24, BG_G = 27, BG_B = 39;
	static const int PANEL_R = 62, PANEL_G = 168, PANEL_B = 210;
	static const int TITLE_R = 66, TITLE_G = 220, TITLE_B = 244;
	static const int TEXT_R = 213, TEXT_G = 214, TEXT_B = 222;
	static const int SOFT_R = 232, SOFT_G = 205, SOFT_B = 152;
	static const int DIM_R = 145, DIM_G = 148, DIM_B = 165;
	static const int ACCENT_R = 198, ACCENT_G = 120, ACCENT_B = 255;
	static const int SELECT_R = 49, SELECT_G = 62, SELECT_B = 83;
	static const int HDR_R = 71, HDR_G = 136, HDR_B = 182;      // cabecera de tabla
	static const int HUD_R = 170, HUD_G = 176, HUD_B = 204;     // borde del HUD
	static const int HUDB_R = 169, HUDB_G = 177, HUDB_B = 204;  // "[Esc]: Salir"
	static const int VOL_R = 245, VOL_G = 166, VOL_B = 84;      // volumen
	static const int HELP_R = 198, HELP_G = 148, HELP_B = 255;  // teclas de ayuda
	static const int HELP_T = 220;                              // texto de ayuda
	static const int CARD_R = 34, CARD_G = 38, CARD_B = 55;     // panel de sesion

	static const int SCREEN_W = 198;
	static const int SCREEN_H = 60;
	static const int VISIBLE_ROWS = 17;

	// ==== glifos UTF-8 ====
	inline static const std::string G_TL = "\xE2\x94\x8C";      // E2 94 8C
	inline static const std::string G_TR = "\xE2\x94\x90";      // E2 94 90
	inline static const std::string G_BL = "\xE2\x94\x94";      // E2 94 94
	inline static const std::string G_BR = "\xE2\x94\x98";      // E2 94 98
	inline static const std::string G_H = "\xE2\x94\x80";       // E2 94 80
	inline static const std::string G_V = "\xE2\x94\x82";       // E2 94 82
	inline static const std::string G_BLOCK = "\xE2\x96\x88";   // E2 96 88

	inline static bool initialized = false;
	inline static int bufW = SCREEN_W;
	inline static int bufH = SCREEN_H;

	// ==== primitivas de dibujo ====
	static void setPos(int x, int y) {
		x = (std::max)(0, (std::min)(x, bufW - 1));
		y = (std::max)(0, (std::min)(y, bufH - 1));
		try { System::Console::SetCursorPosition(x, y); } catch (...) {}
	}

	static void paint(int x, int y, const std::string& s, int fr, int fg, int fb, int br, int bg, int bb) {
		setPos(x, y);
		std::cout << "\x1b[38;2;" << fr << ";" << fg << ";" << fb
			<< ";48;2;" << br << ";" << bg << ";" << bb << "m"
			<< s << "\x1b[0m";
	}

	static void hLine(int x, int y, int len, const std::string& glyph, int fr, int fg, int fb, int br, int bg, int bb) {
		for (int i = 0; i < len; i++) paint(x + i, y, glyph, fr, fg, fb, br, bg, bb);
	}

	static void hLine(int x, int y, int len, const std::string& glyph, int fr, int fg, int fb) {
		for (int i = 0; i < len; i++) paint(x + i, y, glyph, fr, fg, fb, BG_R, BG_G, BG_B);
	}

	static void vLine(int x, int y, int len, const std::string& glyph, int fr, int fg, int fb, int br, int bg, int bb) {
		for (int i = 0; i < len; i++) paint(x, y + i, glyph, fr, fg, fb, br, bg, bb);
	}

	static void fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb, int br, int bg, int bb) {
		std::string row(w, ch);
		for (int i = 0; i < h; i++) paint(x, y + i, row, fr, fg, fb, br, bg, bb);
	}

	static void drawBox(int x, int y, int w, int h, int fr, int fg, int fb) {
		hLine(x + 1, y, w - 2, G_H, fr, fg, fb, BG_R, BG_G, BG_B);
		hLine(x + 1, y + h - 1, w - 2, G_H, fr, fg, fb, BG_R, BG_G, BG_B);
		vLine(x, y + 1, h - 2, G_V, fr, fg, fb, BG_R, BG_G, BG_B);
		vLine(x + w - 1, y + 1, h - 2, G_V, fr, fg, fb, BG_R, BG_G, BG_B);
		paint(x, y, G_TL, fr, fg, fb, BG_R, BG_G, BG_B);
		paint(x + w - 1, y, G_TR, fr, fg, fb, BG_R, BG_G, BG_B);
		paint(x, y + h - 1, G_BL, fr, fg, fb, BG_R, BG_G, BG_B);
		paint(x + w - 1, y + h - 1, G_BR, fr, fg, fb, BG_R, BG_G, BG_B);
	}

	static void clearAll() {
		for (int y = 0; y < SCREEN_H; y++)
			fillRect(0, y, SCREEN_W, 1, ' ', BG_R, BG_G, BG_B, BG_R, BG_G, BG_B);
	}

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
		return out + "\xE2\x80\xA6"; // "..."
	}

	static std::string fmtTime(float seconds) {
		if (seconds <= 0) return "--:--";
		int total = (int)seconds;
		int m = total / 60;
		int s = total % 60;
		return std::to_string(m) + ":" + (s < 10 ? "0" : "") + std::to_string(s);
	}

	static std::string colLabel(const ViewData& d, int index, const std::string& def) {
		if (index >= 1 && (int)d.columnTitles.size() >= index && !d.columnTitles[index - 1].empty())
			return d.columnTitles[index - 1];
		return def;
	}

	static const char* emptyMessage(Tab t) {
		switch (t) {
		case Tab::LIBRARY: return "No hay canciones cargadas en la biblioteca.";
		case Tab::PLAYLISTS: return "No hay playlists creadas.";
		case Tab::LIKES: return "Marca canciones con L para verlas aqui.";
		case Tab::RECOMMENDED: return "Marca canciones con L para generar recomendaciones.";
		case Tab::HISTORIAL: return "El historial esta vacio.";
		default: return "No se encontraron resultados.";
		}
	}

	static std::string thirdValue(const RowData& rd, Tab t) {
		switch (t) {
		case Tab::PLAYLISTS: return std::to_string((std::max)(0LL, rd.playlistSize));
		case Tab::RECOMMENDED: return std::to_string(rd.likes);
		default: return fmtTime(rd.duration);
		}
	}

	// ==== HUD superior ====
	static void drawVolumeBar(int x, int y, int level) {
		std::string bar = "[";
		for (int i = 0; i < 6; i++) bar += (i < level ? '=' : ' ');
		bar += "]";
		paint(x, y, bar, VOL_R, VOL_G, VOL_B, BG_R, BG_G, BG_B);
	}

	static void drawHud(const ViewData& data) {
		drawBox(2, 1, 196, 6, HUD_R, HUD_G, HUD_B);
		paint(4, 3, "[Esc]: Salir", HUDB_R, HUDB_G, HUDB_B, BG_R, BG_G, BG_B);

		const std::string& name = data.player.songName;
		const std::string& artist = data.player.artist;
		if (name.empty()) {
			paint(87, 3, "Sin cancion seleccionada", TITLE_R, TITLE_G, TITLE_B, BG_R, BG_G, BG_B);
		}
		else {
			int nameX = (std::max)(4, 98 - visWidth(name) / 2);
			int artistX = (std::max)(4, 98 - visWidth(artist) / 2);
			paint(nameX, 3, fit(name, 60), TITLE_R, TITLE_G, TITLE_B, BG_R, BG_G, BG_B);
			if (!artist.empty()) paint(artistX, 4, fit(artist, 60), SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
		}

		paint(176, 3, "Volume:", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
		drawVolumeBar(184, 3, 5);
		paint(193, 3, "80%", VOL_R, VOL_G, VOL_B, BG_R, BG_G, BG_B);

		paint(161, 4, "Repeat", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
		paint(168, 4, "/", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
		paint(170, 4, "Random", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
		paint(178, 4, "/", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
		paint(180, 4, "Consume", ACCENT_R, 148, 255, BG_R, BG_G, BG_B);
		paint(188, 4, "/", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
		paint(190, 4, "Single", DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
	}

	// ==== barra de tabs ====
	struct TabInfo {
		Tab tab;
		const char* label;
		int offset;
	};
	inline static const TabInfo kTabs[6] = {
		{ Tab::LIBRARY, "Library", 64 },
		{ Tab::PLAYLISTS, "Playlists", 80 },
		{ Tab::LIKES, "Liked", 100 },
		{ Tab::RECOMMENDED, "Recom.", 111 },
		{ Tab::HISTORIAL, "Historial", 124 },
		{ Tab::SEARCH, "Search", 143 }
	};

	static void drawTabsBar(const ViewData& data) {
		drawBox(2, 8, 196, 3, PANEL_R, PANEL_G, PANEL_B);
		fillRect(3, 9, 194, 1, ' ', PANEL_R, PANEL_G, PANEL_B, PANEL_R, PANEL_G, PANEL_B);

		for (int i = 0; i < 6; i++) {
			const TabInfo& ti = kTabs[i];
			int tabX = 2 + ti.offset;
			int tabWidth = (int)std::string(ti.label).size() + 4;
			bool sel = (ti.tab == data.activeTab);

			if (sel) {
				fillRect(tabX, 9, tabWidth - 2, 1, ' ', 255, 255, 255, 40, 58, 78);
				drawBox(tabX - 1, 8, tabWidth, 3, PANEL_R, PANEL_G, PANEL_B);
				paint(tabX + 1, 9, ti.label, TITLE_R, TITLE_G, TITLE_B, 40, 58, 78);
			}
			else {
				paint(tabX + 1, 9, ti.label, TEXT_R, TEXT_G, TEXT_B, PANEL_R, PANEL_G, PANEL_B);
			}
		}
	}

	// ==== tabla central ====
	static void drawTableHeader(const ViewData& data) {
		bool pl = (data.activeTab == Tab::PLAYLISTS);
		bool rec = (data.activeTab == Tab::RECOMMENDED);
		std::string c1 = colLabel(data, 1, pl ? "User" : "Artist");
		std::string c2 = colLabel(data, 2, "Title");
		std::string c3 = colLabel(data, 3, rec ? "Score" : (pl ? "Count" : "Duration"));
		paint(4, 14, c1, HDR_R, HDR_G, HDR_B, BG_R, BG_G, BG_B);
		paint(41, 14, c2, HDR_R, HDR_G, HDR_B, BG_R, BG_G, BG_B);
		paint(112, 14, c3, HDR_R, HDR_G, HDR_B, BG_R, BG_G, BG_B);
		hLine(4, 15, 116, G_H, PANEL_R, PANEL_G, PANEL_B, BG_R, BG_G, BG_B);
	}

	static void drawRowAt(const ViewData& data, int row) {
		int count = (int)data.rows.size();
		int yy = 17 + (row - data.topRowIndex) * 2;
		if (row > count) {
			fillRect(3, yy, 117, 1, ' ', BG_R, BG_G, BG_B, BG_R, BG_G, BG_B);
			return;
		}

		bool pl = (data.activeTab == Tab::PLAYLISTS);
		const RowData& rd = data.rows[row - 1];
		bool sel = (row == data.selectedIndex);
		int r = sel ? SELECT_R : BG_R;
		int g = sel ? SELECT_G : BG_G;
		int b = sel ? SELECT_B : BG_B;

		fillRect(3, yy, 117, 1, ' ', r, g, b, r, g, b);

		std::string c1 = fit(rd.artist, 30);
		if (pl && rd.artist.empty()) c1 = "You";
		std::string c2 = fit(rd.title, 45);
		std::string c3 = thirdValue(rd, data.activeTab);

		if (sel) {
			paint(4, yy, c1, TITLE_R, TITLE_G, TITLE_B, r, g, b);
			paint(41, yy, c2, TITLE_R, TITLE_G, TITLE_B, r, g, b);
			paint(112, yy, c3, TITLE_R, TITLE_G, TITLE_B, r, g, b);
		}
		else {
			paint(4, yy, c1, SOFT_R, SOFT_G, SOFT_B, r, g, b);
			paint(41, yy, c2, SOFT_R, SOFT_G, SOFT_B, r, g, b);
			paint(112, yy, c3, SOFT_R, SOFT_G, SOFT_B, r, g, b);
		}
	}

	static void drawRows(const ViewData& data) {
		int count = (int)data.rows.size();
		if (count == 0) {
			paint(4, 17, emptyMessage(data.activeTab), DIM_R, DIM_G, DIM_B, BG_R, BG_G, BG_B);
			return;
		}

		for (int i = 0; i < VISIBLE_ROWS; i++) {
			drawRowAt(data, data.topRowIndex + i);
		}
	}

	// ==== panel derecho (portada procedural) ====
	static int artCode(int row, int col) {
		int dx = row - 12;
		int dy = col - 12;
		int d2 = dx * dx + dy * dy;
		if (d2 <= 14) return 24;                 // nucleo blanco
		if (d2 <= 40) return 6;                  // halo lila
		if (d2 <= 80) return 5;                  // halo purpura
		return 2 + ((row * 7 + col * 3) % 17);   // nebulosa exterior
	}

	static void getAlbumArtColor(int code, int& r, int& g, int& b) {
		switch (code) {
		case 1:  r = 0;   g = 0;   b = 0;   break;
		case 2:  r = 20;  g = 15;  b = 45;  break;
		case 3:  r = 45;  g = 70;  b = 140; break;
		case 4:  r = 18;  g = 38;  b = 72;  break;
		case 5:  r = 110; g = 70;  b = 205; break;
		case 6:  r = 185; g = 145; b = 255; break;
		case 7:  r = 125; g = 195; b = 255; break;
		case 8:  r = 28;  g = 105; b = 185; break;
		case 9:  r = 3;   g = 6;   b = 14;  break;
		case 10: r = 12;  g = 16;  b = 32;  break;
		case 11: r = 170; g = 245; b = 255; break;
		case 12: r = 0;   g = 10;  b = 22;  break;
		case 13: r = 45;  g = 200; b = 235; break;
		case 14: r = 0;   g = 55;  b = 95;  break;
		case 15: r = 215; g = 175; b = 255; break;
		case 16: r = 60;  g = 30;  b = 120; break;
		case 17: r = 25;  g = 135; b = 175; break;
		case 18: r = 18;  g = 20;  b = 45;  break;
		case 19: r = 0;   g = 32;  b = 50;  break;
		case 20: r = 45;  g = 28;  b = 90;  break;
		case 21: r = 240; g = 210; b = 255; break;
		case 22: r = 90;  g = 85;  b = 160; break;
		case 23: r = 12;  g = 90;  b = 130; break;
		case 24: r = 255; g = 255; b = 255; break;
		default: r = 0;   g = 0;   b = 0;   break;
		}
	}

	static void paintBlock(int x, int y, int r, int g, int b) {
		setPos(x, y);
		std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m"
			<< G_BLOCK << G_BLOCK << "\x1b[0m";
	}

	static void drawAlbumArt(int x, int y) {
		for (int row = 0; row < 25; row++) {
			for (int col = 0; col < 25; col++) {
				int r, g, b;
				getAlbumArtColor(artCode(row, col), r, g, b);
				paintBlock(x + col * 2, y + row, r, g, b);
			}
		}
	}

	static void drawRightPanel() {
		drawBox(133, 16, 54, 27, PANEL_R, PANEL_G, PANEL_B);
		drawAlbumArt(135, 17);
	}

	// ==== espectro (visual del modelo) ====
	static void drawSpectrum(bool playing) {
		static int frame = 0;
		if (playing) frame++;

		static const int colors[10][3] = {
			{197, 120, 255}, {188, 126, 255}, {177, 138, 255}, {165, 152, 255}, {152, 166, 255},
			{140, 180, 255}, {128, 194, 255}, {112, 206, 255}, {92, 214, 247}, {74, 220, 240}
		};
		const int bars = 21;
		const int maxH = 6;

		for (int i = 0; i < bars; i++) {
			double w1 = std::sin(frame * 0.20 + i * 0.55);
			double w2 = std::sin(frame * 0.11 + i * 0.30);
			int h = 1 + (int)(((w1 + w2 + 2.0) / 4.0) * (maxH - 1));
			int ci = (i < 10) ? i : 9;
			int r = colors[ci][0];
			int g = colors[ci][1];
			int b = colors[ci][2];
			for (int j = 0; j < maxH; j++) {
				setPos(130 + i * 3, 50 - j);
				if (j < h) std::cout << "\x1b[48;2;" << r << ";" << g << ";" << b << "m  \x1b[0m";
				else std::cout << "\x1b[48;2;22;24;37m  \x1b[0m";
			}
		}
	}

	// ==== consola inferior ====
	static void drawConsole() {
		drawBox(2, 52, 196, 5, PANEL_R, PANEL_G, PANEL_B);
		fillRect(3, 53, 50, 3, ' ', PANEL_R, PANEL_G, PANEL_B, PANEL_R, PANEL_G, PANEL_B);
	}

	static void drawHelp() {
		paint(110, 53, "[Enter]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(117, 53, ": Reproducir / Entrar", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
		paint(142, 53, "[Espacio]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(150, 53, ": Pausar / Reanudar", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
		paint(172, 53, "[+]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(175, 53, ": Agregar a playlist", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);

		paint(110, 55, "[L]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(113, 55, ": Like", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
		paint(122, 55, "[A/S/D]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(130, 55, ": Duracion", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
		paint(143, 55, "[R/T]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(150, 55, ": Score", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
		paint(160, 55, "[O/P]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(167, 55, ": Playlists", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
		paint(181, 55, "[N/B]", HELP_R, HELP_G, HELP_B, BG_R, BG_G, BG_B);
		paint(187, 55, ": Sig/Ant", HELP_T, HELP_T, HELP_T, BG_R, BG_G, BG_B);
	}

	// ==== seekbar inferior (progreso del reproductor) ====
	static void drawSeekbar(const PlayerData& p) {
		drawBox(2, 57, 196, 3, PANEL_R, PANEL_G, PANEL_B);

		int inner = (196 - 2) / 2;
		double ratio = (p.length > 0) ? (p.position / p.length) : 0.0;
		ratio = (std::max)(0.0, (std::min)(ratio, 1.0));
		int filled = (int)(inner * ratio);

		for (int i = 0; i < inner; i++) {
			int r, g, b;
			if (i < filled) {
				r = (std::min)(255, 94 + i);
				g = 210;
				b = (std::min)(255, 176 + (i / 2));
			}
			else {
				r = 34; g = 41; b = 61;
			}
			setPos(3 + i * 2, 58);
			std::cout << "\x1b[48;2;" << r << ";" << g << ";" << b << "m  \x1b[0m";
		}
	}

	// ==== pantallas de sesion (fondo por defecto de la consola) ====
	static void clearScreen() {
		std::cout << "\x1b[0m\x1b[2J" << std::flush;
	}

	static void paintDefault(int x, int y, const std::string& s, int fr, int fg, int fb) {
		setPos(x, y);
		std::cout << "\x1b[38;2;" << fr << ";" << fg << ";" << fb << ";49m"
			<< s << "\x1b[0m";
	}

	static void centerDefault(int y, const std::string& text, int fr, int fg, int fb) {
		int x = 98 - visWidth(text) / 2;
		paintDefault(x, y, text, fr, fg, fb);
	}

	static void centerText(int y, const std::string& text, int fr, int fg, int fb, int br, int bg, int bb) {
		int x = 98 - visWidth(text) / 2;
		paint(x, y, text, fr, fg, fb, br, bg, bb);
	}

	static void drawWelcomeOption(int i, bool selected) {
		static const int optCol[3][3] = {
			{137, 172, 118},   // iniciar sesion
			{201, 193, 105},   // registrarse
			{204, 113, 98}     // salir
		};
		static const char* opts[3] = {
			"[1]  Iniciar sesion",
			"[2]  Registrarse",
			"[3]  Salir del programa"
		};
		int y = 31 + i * 4;
		int r = optCol[i][0], g = optCol[i][1], b = optCol[i][2];
		fillRect(57, y - 1, 82, 3, ' ', CARD_R, CARD_G, CARD_B, CARD_R, CARD_G, CARD_B);
		if (selected) {
			fillRect(57, y - 1, 82, 3, ' ', CARD_R, CARD_G, CARD_B, r, g, b);
			paint(59, y, ">", CARD_R, CARD_G, CARD_B, r, g, b);
			paint(62, y, opts[i], CARD_R, CARD_G, CARD_B, r, g, b);
			paint(135, y, ">>", CARD_R, CARD_G, CARD_B, r, g, b);
		}
		else {
			paint(62, y, opts[i], TEXT_R, TEXT_G, TEXT_B, CARD_R, CARD_G, CARD_B);
		}
	}

	static void renderAuthScreen(const std::string& title, const std::string& subtitle,
		const std::string& error) {
		clearScreen();

		const int cR = PANEL_R, cG = PANEL_G, cB = PANEL_B;      // cian
		const int pR = ACCENT_R, pG = ACCENT_G, pB = ACCENT_B;   // morado
		const int tR = TEXT_R, tG = TEXT_G, tB = TEXT_B;         // texto claro
		const int dR = DIM_R, dG = DIM_G, dB = DIM_B;            // texto tenue
		const int cardR = 34, cardG = 38, cardB = 55;            // panel oscuro
		const int lR = 62, lG = 130, lB = 160;                   // linea decorativa

		// titulo decorado
		paintDefault(79, 18, "--------", cR, cG, cB);
		paintDefault(88, 18, "*", cR, cG, cB);
		centerDefault(18, title, pR, pG, pB);
		int titleEnd = 98 + visWidth(title) / 2 + 2;
		paintDefault(titleEnd, 18, "*", cR, cG, cB);
		paintDefault(titleEnd + 2, 18, "--------", cR, cG, cB);

		centerDefault(21, subtitle, tR, tG, tB);

		hLine(62, 24, 34, "-", lR, lG, lB);
		paintDefault(97, 24, "<>", cR, cG, cB);
		hLine(101, 24, 34, "-", lR, lG, lB);

		drawBox(53, 27, 90, 16, cR, cG, cB);
		drawBox(79, 26, 38, 3, cR, cG, cB);
		fillRect(80, 27, 36, 1, ' ', cR, cG, cB, cR, cG, cB);
		centerText(27, "Ingresa tus datos", tR, tG, tB, cR, cG, cB);

		paintDefault(60, 33, "Usuario: ", cR, cG, cB);
		paintDefault(60, 37, "Contrasena: ", cR, cG, cB);
		drawBox(76, 32, 60, 3, cR, cG, cB);
		drawBox(76, 36, 60, 3, cR, cG, cB);

		if (!error.empty()) {
			paint(60, 40, error, 255, 90, 90, cardR, cardG, cardB);
		}

		centerDefault(49, "[Enter] Continuar            [Deja el usuario vacio y pulsa Enter para volver]", dR, dG, dB);
	}

	static void renderWelcomeScreen(int selected) {
		clearScreen();

		const int cR = PANEL_R, cG = PANEL_G, cB = PANEL_B;
		const int pR = ACCENT_R, pG = ACCENT_G, pB = ACCENT_B;
		const int tR = TEXT_R, tG = TEXT_G, tB = TEXT_B;
		const int dimR = DIM_R, dimG = DIM_G, dimB = DIM_B;
		const int lR = 62, lG = 130, lB = 160;
		const int cardR = 34, cardG = 38, cardB = 55;

		paintDefault(81, 18, "--------", cR, cG, cB);
		paintDefault(90, 18, "*", cR, cG, cB);
		centerDefault(18, "Bienvenido", pR, pG, pB);
		int titleEnd = 98 + visWidth("Bienvenido") / 2 + 2;
		paintDefault(titleEnd, 18, "*", cR, cG, cB);
		paintDefault(titleEnd + 2, 18, "--------", cR, cG, cB);

		centerDefault(21, "Disfruta tu musica favorita desde la terminal.", tR, tG, tB);

		hLine(62, 24, 34, "-", lR, lG, lB);
		paintDefault(97, 24, "<>", cR, cG, cB);
		hLine(101, 24, 34, "-", lR, lG, lB);

		drawBox(53, 27, 90, 16, cR, cG, cB);
		drawBox(79, 26, 38, 3, cR, cG, cB);
		fillRect(80, 27, 36, 1, ' ', cR, cG, cB, cR, cG, cB);
		centerText(27, "Selecciona una opcion", tR, tG, tB, cR, cG, cB);

		for (int i = 0; i < 3; i++) {
			drawWelcomeOption(i, i == selected);
		}

		centerDefault(52, "Usa flechas ARRIBA/ABAJO y presiona [Enter] para seleccionar", 200, 190, 230);
		paintDefault(3, 56, "Desarrollado en C++", dimR, dimG, dimB);
		paintDefault(150, 56, "Presiona [Esc] en cualquier momento para salir", dimR, dimG, dimB);
	}

	static void ensureInit() {
		if (initialized) return;
		initialized = true;

		SetConsoleOutputCP(CP_UTF8);
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode = 0;
		if (GetConsoleMode(h, &mode)) {
			SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
		setvbuf(stdout, nullptr, _IONBF, 0);
		std::cout << "\x1b[?25l\x1b[0m";

		// Amplia el buffer a 200x60 (la ventana se ajustara si la pantalla alcanza).
		try { System::Console::BufferWidth = 200; } catch (...) {}
		try { System::Console::WindowWidth = 200; } catch (...) {}
		try { System::Console::WindowHeight = SCREEN_H; } catch (...) {}

		try {
			int bW = System::Console::BufferWidth;
			int bH = System::Console::BufferHeight;
			bufW = (std::min)(bW, SCREEN_W);
			bufH = (std::min)(bH, SCREEN_H);
		}
		catch (...) {}
	}

public:
	static void init() {
		ensureInit();
	}

	void render(const ViewData& data) {
		ensureInit();
		clearAll();

		drawHud(data);
		drawTabsBar(data);

		drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
		drawTableHeader(data);
		drawRows(data);
		vLine(121, 13, 38, G_V, PANEL_R, PANEL_G, PANEL_B, BG_R, BG_G, BG_B);

		if (data.activeTab != Tab::PLAYLISTS) drawRightPanel();

		drawConsole();
		drawHelp();
		drawSeekbar(data.player);
		drawSpectrum(data.player.state == PlayerState::PLAYING);

		std::cout << std::flush;
	}

	void renderWelcome(int selectedIndex) {
		ensureInit();
		renderWelcomeScreen(selectedIndex);
		std::cout << std::flush;
	}

	void renderLogin(const std::string& error) {
		ensureInit();
		renderAuthScreen("Iniciar Sesion", "Ingresa tus credenciales para continuar.", error);
		std::cout << std::flush;
	}

	void renderRegister(const std::string& error) {
		ensureInit();
		renderAuthScreen("Registrarse", "Ingresa tus credenciales para continuar.", error);
		std::cout << std::flush;
	}

	void updateWelcome(int prevIndex, int newIndex) {
		ensureInit();
		if (prevIndex != newIndex) {
			drawWelcomeOption(prevIndex, false);
			drawWelcomeOption(newIndex, true);
		}
		std::cout << std::flush;
	}

	void updateRow(const ViewData& data, int row) {
		ensureInit();
		if (row >= data.topRowIndex && row < data.topRowIndex + VISIBLE_ROWS) {
			drawRowAt(data, row);
		}
		std::cout << std::flush;
	}

	void updateRows(const ViewData& data) {
		ensureInit();
		for (int i = 0; i < VISIBLE_ROWS; i++) {
			drawRowAt(data, data.topRowIndex + i);
		}
		std::cout << std::flush;
	}

	void placeAuthCursor(int field) {
		ensureInit();
		setPos(78, field == 0 ? 33 : 37);
	}

	void clearAuthFields() {
		ensureInit();
		paintDefault(77, 33, "                                                          ", DIM_R, DIM_G, DIM_B);
		paintDefault(77, 37, "                                                          ", DIM_R, DIM_G, DIM_B);
		std::cout << std::flush;
	}

	void updateAuthError(const std::string& error) {
		ensureInit();
		paintDefault(60, 40, "                                                        ", DIM_R, DIM_G, DIM_B);
		if (!error.empty()) paintDefault(60, 40, error, 255, 90, 90);
		std::cout << std::flush;
	}
};