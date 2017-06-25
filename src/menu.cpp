#include "menu.h"
#include "2dqdefs.h"
#include "keyboard.h"

static BYTE mnuLanguage = 0;

BYTE ShowScreenImage(char *FileName)
{
	FILE *Source;
	if ((Source = fopen(FileName, "rb")) == NULL) {
		printf("unable to find file %s", FileName);
		abort();
		return 0;
	}
	constexpr int width = 320;
	constexpr int height = 200;
	BYTE img[width * height];
	for (int i = 0; i < sizeof(img); i++) {
		fread(&img[i], 1, 1, Source);
	}
	fclose(Source);

	for (int y = 0; y < screen::h; y++) {
		for (int x = 0; x < screen::w; x++) {
			int x0 = x * width / screen::w;
			int y0 = y * height / screen::h;
			BYTE color = img[y0 * width + x0];
			SetPixelColor(x, y, color);
		}
	}
	return 1;
}

void InputMenu(char *dst, char *msg, Si16 dstSize)
{
	while (true) {
		ShowScreenImage("IMAGES\\MENU.RAW");
		SetColor(MENU_COL3);
		OutText(screen::cx + 2, screen::cy - 10 + 2, msg, &g_font); // was 162 92
		SetColor(MENU_COL1);
		OutText(screen::cx, screen::cy - 10, msg, &g_font); // was 160 90
		dst[strlen(dst) + 1] = 0;
		dst[strlen(dst)] = '_';
		SetColor(MENU_COL3);
		OutText(screen::cx + 2, screen::cy + 10 + 2, dst, &g_font); // was 162 112
		SetColor(MENU_COL2);
		OutText(screen::cx, screen::cy + 10, dst, &g_font); // was 160 110
		dst[strlen(dst) - 1] = 0;
		ShowScreen();

		if (KF(kKeyEscape)) {
			KeyDown(kKeyEscape);
			break;
		}
		if (KF(kKeyEnter)) {
			KeyDown(kKeyEnter);
			break;
		}
		if (KF(kKeyCapsLock)) {
			KeyDown(kKeyCapsLock);
			mnuLanguage = !mnuLanguage;
		}
		if (KF(kKeyBackspace) && strlen(dst)) {
			KeyDown(kKeyBackspace);
			dst[strlen(dst) - 1] = 0;
		}
		if ((Si16)strlen(dst) < dstSize) {
			for (char c = 0; c < 127; c++) {
				if ((c >= 'a' && c <= 'z') ||
					(c >= '0' && c <= '9') ||
					c == '-' ||
					c == '_' ||
					c == '.' ||
					c == ',' ||
					c == '\\' ||
					c == '/')
				{
					if (IsKey(c)) {
						KeyDown(c);
						dst[strlen(dst) + 1] = 0;
						dst[strlen(dst)] = (IsKey(kKeyShift) ? toupper(c) : c);
					}
				}
			}
			/*
			for (Si16 i = 0; i < 58; i++) {
			if (KF(i)) {
			KeyDown(i);
			dst[strlen(dst) + 1] = 0;
			if (mnuLanguage)dst[strlen(dst)] = EnglishKeyName[i];
			else dst[strlen(dst)] = RussianKeyName[i];
			}
			}
			*/
		}
	}
}

void MessageMenu(const char *msg)
{
	while (true) {
		ShowScreenImage("IMAGES\\MENU.RAW");
		SetColor(MENU_COL3);
		OutText(screen::cx + 2, screen::cy + 2, msg, &g_font);
		SetColor(MENU_COL1);
		OutText(screen::cx, screen::cy, msg, &g_font);
		ShowScreen();

		if (KF(kKeyEscape)) {
			KeyDown(kKeyEscape);
			break;
		}
		if (KF(kKeyEnter)) {
			KeyDown(kKeyEnter);
			break;
		}
	}
}
