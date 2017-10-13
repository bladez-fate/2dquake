/*
2D Quake Map Editor v1.01
Copyleft by bladez-fate
19 Oct 2002 - 20 Oct 2002
*/
#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
//#include <process.h>
#include <string.h>
//#include <conio.h>
//#include <dos.h>
#include <math.h>
//#include <conio.h>

#include "graph.h"
#include "keyboard.h"
#include "textmode.h"
#include "menu.h"
#include "2dqdefs.h"

/*
if(strcmp(tmpstr,"Armour")==0) {ITEM(x,y,Armour) continue;}
if(strcmp(tmpstr,"Health")==0) {ITEM(x,y,Health) continue;}
if(strcmp(tmpstr,"Bow")==0) {ITEM(x,y,Bow) continue;}
if(strcmp(tmpstr,"Akm")==0) {ITEM(x,y,Akm) continue;}
if(strcmp(tmpstr,"Napalm")==0) {ITEM(x,y,Napalm) continue;}
if(strcmp(tmpstr,"Launcher")==0) {ITEM(x,y,Launcher) continue;}
if(strcmp(tmpstr,"Laser")==0) {ITEM(x,y,Laser) continue;}
if(strcmp(tmpstr,"Redmer")==0) {ITEM(x,y,Redmer) continue;}
if(strcmp(tmpstr,"Photo")==0) {ITEM(x,y,Photo) continue;}
if(strcmp(tmpstr,"Grenade")==0) {ITEM(x,y,Grenade) continue;}
if(strcmp(tmpstr,"Photo")==0) {ITEM(x,y,Photo) continue;}
if(strcmp(tmpstr,"PistolAmmo")==0) {ITEM(x,y,PistolAmmo) continue;}
if(strcmp(tmpstr,"BowAmmo")==0) {ITEM(x,y,BowAmmo) continue;}
if(strcmp(tmpstr,"AkmAmmo")==0) {ITEM(x,y,AkmAmmo) continue;}
if(strcmp(tmpstr,"NapalmAmmo")==0) {ITEM(x,y,NapalmAmmo) continue;}
if(strcmp(tmpstr,"LauncherAmmo")==0) {ITEM(x,y,LauncherAmmo) continue;}
if(strcmp(tmpstr,"LaserAmmo")==0) {ITEM(x,y,LaserAmmo) continue;}
*/

namespace quake2d_editor {

	struct BOTPOINT {
		Si16 x;
		Si16 y;
		Si16 NPnts;
		Si16 NPnt[8];
	} pnt[1000];

	struct ITEM {
		Si16 x;
		Si16 y;
		BYTE type;
	} itm[500];

	struct VIEWPORT
	{
		char Message[4][64];
		Si16  MsgCounter[4];
		Si16 x;
		Si16 y;
		Si16 scr_x1;
		Si16 scr_y1;
		Si16 scr_x2;
		Si16 scr_y2;
		void Show(void);
		void ShowPoints(void);
		void ShowItems(void);
		void ShowCursor(Si16, Si16, Si16);
		void StepTo(Si16, Si16);
		void MakeMessage(char *);
		void ClearMessages(void);
		VIEWPORT();
	};

	struct CURSOR
	{
		Si16 x;
		Si16 y;
		Si16 Status;
		Si16 tx;
		Si16 ty;
		Si16 ter; // Active Terrain Element
		CURSOR();
	} cr;

	/* Globals */
	static Si16 _MpImgLoaded = 0;
	static tIMAGE FragImg;
	static tIMAGE HealthImg;
	static tIMAGE ArmourImg;
	static tIMAGE MpImg[M_MAPELM];
	static tIMAGE MaskImg[M_MASK];
	static tIMAGE GrImg[4];
	static tIMAGE RkImg[8];
	static tIMAGE RdImg[8];
	static tIMAGE CrpsImg[8];
	static tIMAGE ItemImg[24];
	static tIMAGE Map;
	static BYTE MapNum;
	static BYTE MF_PlrSee[M_MAPELM];
	static BYTE MF_BltMov[M_MAPELM];
	static BYTE MF_PlrMov[M_MAPELM];
	static char BotFileName[80];
	static char ItemFileName[80];
	static Si16 PntAmount;
	static Si16 ItmAmount;
	static Si16 ActPnt;
	static Si16 ActLnk;
	static Si16 TerItm;
	static char ItmName[16][30] = {
		"Armour","Health","Bow","Akm","Napalm","Launcher","Laser","Redmer",
		"Photo","Grenade","PistolAmmo","BowAmmo","AkmAmmo","NapalmAmmo",
		"LauncherAmmo","LaserAmmo"
	};

	/* Structures */
	static VIEWPORT vp;

	/* Functions' Prototypes */
	void DeleteLink(Si16, Si16);
	void DeletePoint(Si16);
	void ShowTools(Si16, Si16);
	void ShowItemTools(Si16, Si16);
	void Start(void);
	Si16 InitializeMap(const char*, char*);
	Si16 SaveMap(const char *);
	Si16 SavePoints(const char *FileName);
	Si16 SaveItems(const char *FileName);
	void CloseMap(void);
	void DrawMenuTitle(void);
	BYTE FF_PlrSee(Si16, Si16);
	BYTE FF_BltMov(Si16, Si16);
	BYTE FF_PlrMov(Si16, Si16);

	/* Functions' Bodies */
	void DeleteLink(Si16 p, Si16 l)
	{
		for (Si16 i = l; i < pnt[p].NPnts - 1; i++)
			pnt[p].NPnt[i] = pnt[p].NPnt[i + 1];
		pnt[p].NPnts--;
	}

	void DeletePoint(Si16 p)
	{
		for (Si16 i = 0; i < PntAmount; i++) {
			for (Si16 j = 0; j < pnt[i].NPnts; j++) {
				if (pnt[i].NPnt[j] == p) {
					DeleteLink(i, j); j--;
				}
				else if (pnt[i].NPnt[j] > p) {
					pnt[i].NPnt[j]--;
				}
			}
		}
		for (Si16 i = p; i < PntAmount - 1; i++) pnt[i] = pnt[i + 1];
		PntAmount--;
	}

	BYTE FF_PlrSee(Si16 ix, Si16 iy) {
		BYTE masknum = MF_PlrSee[Map.Bits[Si16(iy / 20)*Map.Width + Si16(ix / 20)]];
		return MaskImg[masknum].Bits[(iy % 20) * 20 + ix % 20];
	}
	BYTE FF_BltMov(Si16 ix, Si16 iy) {
		BYTE masknum = MF_BltMov[Map.Bits[Si16(iy / 20)*Map.Width + Si16(ix / 20)]];
		return MaskImg[masknum].Bits[(iy % 20) * 20 + ix % 20];
	}
	BYTE FF_PlrMov(Si16 ix, Si16 iy) {
		BYTE masknum = MF_PlrMov[Map.Bits[Si16(iy / 20)*Map.Width + Si16(ix / 20)]];
		return MaskImg[masknum].Bits[(iy % 20) * 20 + ix % 20];
	}

	VIEWPORT::VIEWPORT()
	{
		MsgCounter[0] = 0; MsgCounter[1] = 0;
		MsgCounter[2] = 0; MsgCounter[3] = 0;
	}

	CURSOR::CURSOR() { x = 7; y = 4; ter = 0; tx = 7; ty = 4; Status = 0; }

	void VIEWPORT::ClearMessages(void) { MsgCounter[0] = 0; }
	void VIEWPORT::StepTo(Si16 xx, Si16 yy) {
		xx -= (scr_x2 - scr_x1) / 2;
		yy -= (scr_y2 - scr_y1) / 2;
		if (xx > x) x += 2;
		else if (xx < x) x -= 2;
		if (yy > y) y += 2;
		else if (yy < y) y -= 2;
	}
	void VIEWPORT::MakeMessage(char *MsgText) {
		if (MsgCounter[2]) { strcpy(Message[3], Message[2]); MsgCounter[3] = MsgCounter[2]; }
		if (MsgCounter[1]) { strcpy(Message[2], Message[1]); MsgCounter[2] = MsgCounter[1]; }
		if (MsgCounter[0]) { strcpy(Message[1], Message[0]); MsgCounter[1] = MsgCounter[0]; }
		strcpy(Message[0], MsgText); MsgCounter[0] = MSG_READWAIT;
	}
	void VIEWPORT::ShowCursor(Si16 xx, Si16 yy, Si16 col) {
		MakeBufferWindow(scr_x1, scr_y1, scr_x2, scr_y2);
		SetColor(col);
		FrameRect(xx * 20 - x + scr_x1, yy * 20 - y + scr_y1
			, xx * 20 - x + scr_x1 + 19, yy * 20 - y + scr_y1 + 19);
		NoBufferWindow();
	}
	void VIEWPORT::ShowItems(void) {
		MakeBufferWindow(scr_x1, scr_y1, scr_x2, scr_y2);
		for (Si16 i = 0; i < ItmAmount; i++)
			DrawSWImage(itm[i].x - x - ItemImg[itm[i].type].Width / 2,
				itm[i].y - y - ItemImg[itm[i].type].Height / 2,
				ItemImg + itm[i].type);
		NoBufferWindow();
	}

	void VIEWPORT::ShowPoints(void) {
		MakeBufferWindow(scr_x1, scr_y1, scr_x2, scr_y2);
		SetColor(120);
		for (Si16 i = 0; i < PntAmount; i++) {
			FillRect(pnt[i].x - x - 1, pnt[i].y - y - 1, pnt[i].x - x + 1, pnt[i].y - y + 1);
			for (Si16 j = 0; j < pnt[i].NPnts; j++) {
				Line(pnt[i].x - x, pnt[i].y - y,
					pnt[pnt[i].NPnt[j]].x - x, pnt[pnt[i].NPnt[j]].y - y);
			}
		}
		if (ActPnt >= 0 && ActPnt < PntAmount) {
			SetColor(47);
			FillRect(pnt[ActPnt].x - x - 1, pnt[ActPnt].y - y - 1,
				pnt[ActPnt].x - x + 1, pnt[ActPnt].y - y + 1);
			for (Si16 j = 0; j < pnt[ActPnt].NPnts; j++) {
				Line(pnt[ActPnt].x - x, pnt[ActPnt].y - y,
					pnt[pnt[ActPnt].NPnt[j]].x - x, pnt[pnt[ActPnt].NPnt[j]].y - y);
			}
			if (ActLnk >= 0 && ActLnk < pnt[ActPnt].NPnts) {
				SetColor(79);
				Line(pnt[ActPnt].x - x, pnt[ActPnt].y - y,
					pnt[pnt[ActPnt].NPnt[ActLnk]].x - x,
					pnt[pnt[ActPnt].NPnt[ActLnk]].y - y);
			}
		}

		NoBufferWindow();
	}

	void VIEWPORT::Show(void)
	{
		MakeBufferWindow(scr_x1, scr_y1, scr_x2, scr_y2);
		long i, ix, iy;
		for (iy = y / 20 - 1; iy < (long)(y + scr_y2 - scr_y1) / 20 + 1; iy++)
			for (ix = x / 20 - 1; ix < (long)(x + scr_x2 - scr_x1) / 20 + 1; ix++)
			{
				if (ix >= 0 && iy >= 0 && ix < Map.Width && iy < Map.Height) i = Map.Bits[iy*Map.Width + ix];
				else i = 1;
				DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + i);
			}
		SetTextJust(1, 0);
		SetColor(127);
		{
			Si16 a = 0;
			for (Si16 i = 0; i < 4; i++)
				if (MsgCounter[i])
				{
					if ((Si16)strlen(Message[i]) > (scr_x2 - scr_x1) / 8 - 2)
					{
						char tmpstr[64];
						memcpy(tmpstr, Message[i], strlen(Message[i]) / 2);
						tmpstr[strlen(Message[i]) / 2] = 0;
						OutText((scr_x2 + scr_x1) / 2, scr_y1 + 2 + 10 * a, tmpstr, &g_font);
						a++;
						memcpy(tmpstr, Message[i] + strlen(Message[i]) / 2, strlen(Message[i]) / 2 + strlen(Message[i]) % 2);
						tmpstr[strlen(Message[i]) / 2 + strlen(Message[i]) % 2] = 0;
						OutText((scr_x2 + scr_x1) / 2, scr_y1 + 2 + 10 * a, tmpstr, &g_font);
						a++;
					}
					else {
						OutText((scr_x2 + scr_x1) / 2, scr_y1 + 2 + 10 * a, Message[i], &g_font);
						a++;
					}

					MsgCounter[i]--;
				}
		}

		SetTextJust(2, 0);
		NoBufferWindow();
	}

	void ShowTools(Si16 act, Si16 col)
	{
		for (Si16 i = 0; i < _MpImgLoaded; i++)
			DrawWImage((i % 16) * 20, (i / 16) * 20, MpImg + i);
		SetColor(col);
		FrameRect((act % 16) * 20, (act / 16) * 20,
			(act % 16) * 20 + 19, (act / 16) * 20 + 19);
	}

	void ShowItemTools(Si16 act, Si16 col)
	{
		SetColor(0);
		ClearScreen();
		for (Si16 i = 0; i < 24; i++)
			DrawSWImage((i % 16) * 20 + 10 - ItemImg[i].Width / 2,
			(i / 16) * 20 + 10 - ItemImg[i].Height / 2, ItemImg + i);
		SetColor(col);
		FrameRect((act % 16) * 20, (act / 16) * 20,
			(act % 16) * 20 + 19, (act / 16) * 20 + 19);
	}

	void Start(void)
	{
		TerItm = 0;
		ActPnt = -1;
		ActLnk = -1;
		vp.x = 0;
		vp.y = 0;
		vp.scr_x1 = 1; vp.scr_y1 = 1;
		vp.scr_x2 = 318; vp.scr_y2 = 198;
	}

	void CloseMap(void)
	{
		for (Si16 i = 0; i < _MpImgLoaded; i++)
			FreeImage(MpImg + i);
		FreeImage(&Map);
	}

	Si16 SaveMap(const char *FileName)
	{
		FILE *Dest;
		if ((Dest = fopen(FileName, "wb")) == NULL) return 0;
		fwrite(&Map.Width, 2, 1, Dest);
		fwrite(&Map.Height, 2, 1, Dest);
		fwrite(Map.Bits, Map.Height, Map.Width, Dest);
		fclose(Dest);
		return 1;
	}

	Si16 SavePoints(const char *FileName)
	{
		FILE *Dest;
		if ((Dest = fopen(FileName, "wt")) == NULL) return 0;
		fprintf(Dest, "%hd\n", PntAmount);
		for (Si16 i = 0; i < PntAmount; i++)
			fprintf(Dest, "Graf(%hd,%.1lf,%.1lf,%hd);\n", i,
			(double)(pnt[i].x - 10) / 20,
				(double)(pnt[i].y - 10) / 20, pnt[i].NPnts);
		Si16 LnkAmount = 0;
		for (Si16 i = 0; i < PntAmount; i++)
			for (Si16 j = 0; j < pnt[i].NPnts; j++)
				LnkAmount++;

		fprintf(Dest, "%hd\n", LnkAmount);
		for (Si16 i = 0; i < PntAmount; i++)
			for (Si16 j = 0; j < pnt[i].NPnts; j++) {
				fprintf(Dest, "Gpnt(%hd,%hd,%hd);\n", i, j, pnt[i].NPnt[j]);
			}

		fclose(Dest);
		return 1;
	}

	Si16 SaveItems(const char *FileName)
	{
		FILE *Dest;
		if ((Dest = fopen(FileName, "wt")) == NULL) return 0;
		Si16 PosAmount = 0;
		for (Si16 i = 0; i < ItmAmount; i++) {
			if (itm[i].type < 8) PosAmount++;
		}
		fprintf(Dest, "%hd\n", PosAmount);
		for (Si16 i = 0; i < ItmAmount; i++) {
			if (itm[i].type < 8) {
				fprintf(Dest, "%hd %hd %hd\n", itm[i].x, itm[i].y, Si16(itm[i].type));
			}
		}
		fprintf(Dest, "%hd\n", ItmAmount - PosAmount);
		for (Si16 i = 0; i < ItmAmount; i++) {
			if (itm[i].type >= 8) {
				fprintf(Dest, "%.1lf %.1lf %s\n", (double)(itm[i].x - 10) / 20,
					(double)(itm[i].y - 10) / 20, ItmName[itm[i].type - 8]);
			}
		}
		fclose(Dest);
		return 1;
	}

	Si16 InitializeMap(const char *FileName, char *MapFile)
	{
		FILE *src;
		char tmpstr[64];
		Si16 tmpi;
		if ((src = fopen(FileName, "rt")) == NULL) { return 0; }
		Si16 TerAmount;
		fscanf(src, " %[a-zA-Z]", tmpstr);
		_strlwr(tmpstr);
		if (strcmp(tmpstr, "name") != 0) { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '=') { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		fscanf(src, "%[^\"]", tmpstr);
		vp.MakeMessage(tmpstr);
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		fscanf(src, " %[a-zA-Z]", tmpstr);
		_strlwr(tmpstr);
		if (strcmp(tmpstr, "mapfile") != 0) { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '=') { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		fscanf(src, "%[^\"]", tmpstr);
		if (!LoadImage(tmpstr, &Map)) { fclose(src); return 0; }
		strcpy(MapFile, tmpstr);
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		/* BotFile */
		fscanf(src, " %[a-zA-Z]", tmpstr);
		_strlwr(tmpstr);
		if (strcmp(tmpstr, "botfile") != 0) { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '=') { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		fscanf(src, "%[^\"]", BotFileName);
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		/* ItemFile */
		fscanf(src, " %[a-zA-Z]", tmpstr);
		_strlwr(tmpstr);
		if (strcmp(tmpstr, "itemfile") != 0) { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '=') { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		fscanf(src, "%[^\"]", ItemFileName);
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '"') { fclose(src); return 0; }
		fscanf(src, " %[a-zA-Z]", tmpstr);
		_strlwr(tmpstr);
		if (strcmp(tmpstr, "terrainimageamount") != 0) { fclose(src); return 0; }
		fscanf(src, " %c", tmpstr);
		if (tmpstr[0] != '=') { fclose(src); return 0; }
		fscanf(src, " %hd", &TerAmount);

		for (Si16 i = 0; i < TerAmount; i++)
		{
			fscanf(src, " %[a-zA-Z]", tmpstr);
			_strlwr(tmpstr);
			if (strcmp(tmpstr, "mapimage") != 0) { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '[') { fclose(src); return 0; }
			fscanf(src, " %hd", &tmpi);
			if (i != tmpi) { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != ']') { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '=') { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '"') { fclose(src); return 0; }
			fscanf(src, "%[^\"]", tmpstr);
			if (!LoadImage(tmpstr, MpImg + i)) { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '"') { fclose(src); return 0; }

			fscanf(src, " %[a-zA-Z]", tmpstr);
			_strlwr(tmpstr);
			if (strcmp(tmpstr, "mapmasks") != 0) { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '[') { fclose(src); return 0; }
			fscanf(src, " %hd", &tmpi);
			if (i != tmpi) { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != ']') { fclose(src); return 0; }
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '=') { fclose(src); return 0; }

			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '{') { fclose(src); return 0; }
			fscanf(src, "%hd", &tmpi); MF_PlrSee[i] = tmpi;
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != ',') { fclose(src); return 0; }
			fscanf(src, "%hd", &tmpi); MF_BltMov[i] = tmpi;
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != ',') { fclose(src); return 0; }
			fscanf(src, "%hd", &tmpi); MF_PlrMov[i] = tmpi;
			fscanf(src, " %c", tmpstr);
			if (tmpstr[0] != '}') { fclose(src); return 0; }
		}

		fclose(src);
		_MpImgLoaded = TerAmount;

		/* Load Items & StartLocations */
		if ((src = fopen(ItemFileName, "rt")) == NULL) {
			ItmAmount = 0;
		}
		else {
			Si16 k;
			double ax, ay;
			fscanf(src, " %hd", &ItmAmount);
			Si16 i;
			for (i = 0; i < ItmAmount; i++) {
				Si16 type = 0;
				fscanf(src, " %hd %hd %hd", &itm[i].x, &itm[i].y, &type);
				itm[i].type = type;
			}
			fscanf(src, " %hd", &k); ItmAmount += k;
			for (; i < ItmAmount; i++) {
				fscanf(src, " %lf %lf %s", &ax, &ay, tmpstr);
				itm[i].x = ax * 20 + 10;
				itm[i].y = ay * 20 + 10;
				if (strcmp(tmpstr, "Armour") == 0) { itm[i].type = 8; continue; }
				if (strcmp(tmpstr, "Health") == 0) { itm[i].type = 9; continue; }
				if (strcmp(tmpstr, "Bow") == 0) { itm[i].type = 10; continue; }
				if (strcmp(tmpstr, "Akm") == 0) { itm[i].type = 11; continue; }
				if (strcmp(tmpstr, "Napalm") == 0) { itm[i].type = 12; continue; }
				if (strcmp(tmpstr, "Launcher") == 0) { itm[i].type = 13; continue; }
				if (strcmp(tmpstr, "Laser") == 0) { itm[i].type = 14; continue; }
				if (strcmp(tmpstr, "Redmer") == 0) { itm[i].type = 15; continue; }
				if (strcmp(tmpstr, "Photo") == 0) { itm[i].type = 16; continue; }
				if (strcmp(tmpstr, "Grenade") == 0) { itm[i].type = 17; continue; }
				if (strcmp(tmpstr, "PistolAmmo") == 0) { itm[i].type = 18; continue; }
				if (strcmp(tmpstr, "BowAmmo") == 0) { itm[i].type = 19; continue; }
				if (strcmp(tmpstr, "AkmAmmo") == 0) { itm[i].type = 20; continue; }
				if (strcmp(tmpstr, "NapalmAmmo") == 0) { itm[i].type = 21; continue; }
				if (strcmp(tmpstr, "LauncherAmmo") == 0) { itm[i].type = 22; continue; }
				if (strcmp(tmpstr, "LaserAmmo") == 0) { itm[i].type = 23; continue; }
			}

			fclose(src);
		}

		/* Load or Create Graf */
		if ((src = fopen(BotFileName, "rt")) == NULL) {
			PntAmount = ItmAmount;
			for (Si16 i = 0; i < PntAmount; i++) {
				pnt[i].x = itm[i].x;
				pnt[i].y = itm[i].y;
			}
		}
		else {
			fscanf(src, " %hd", &PntAmount);
			for (Si16 i = 0; i < PntAmount; i++) {
				Si16 k, aa;
				double ax, ay;
				fscanf(src, " Graf( %hd , %lf , %lf , %hd ) ;", &k, &ax, &ay, &aa);
				pnt[k].x = Si16(ax * 20) + 10;
				pnt[k].y = Si16(ay * 20) + 10;
				pnt[k].NPnts = aa;
			}
			Si16 LnkAmount;
			fscanf(src, " %hd", &LnkAmount);
			for (Si16 i = 0; i < LnkAmount; i++) {
				Si16 a1, a2, a3;
				fscanf(src, " Gpnt( %hd , %hd , %hd ) ;", &a1, &a2, &a3);
				pnt[a1].NPnt[a2] = a3;
			}

			fclose(src);
		}

		return 1;
	}

	void ExitEditor(const std::string& exitMessage)
	{
		// Exit editor
		for (Si16 i = 0; i < M_MASK; i++)
			FreeImage(MaskImg + i);
		CloseMap();
		for (Si16 i = 0; i < 4; i++)
			FreeImage(GrImg + i);
		for (Si16 i = 0; i < 8; i++) {
			FreeImage(RdImg + i);
			FreeImage(RkImg + i);
		}
		g_palette.Load("system/menu.pal");
		g_palette.Set();
		if (!exitMessage.empty()) {
			MessageMenu(exitMessage.c_str());
		}
	}

	/************/
	/*** MAIN ***/
	/************/
	void main(void)
	{
		/* Get All Info */
		Si16 Mode = 0, Input = 0, BEAct = 0, BESave;
		std::string exitMessage;
		/*
		  Modes:  0 - Standart
				  1 - Ter Selection
				  2 - Bot's Graf Editor
				  3 - Item Editor (Edit)
				  4 - Item Editor (Select Item)
		*/
		char FileName[32] = { 0 };
		char MapFile[32];
		Cscanf("%33s", FileName);
		InputMenu(FileName, "Enter Map Name", sizeof(FileName) - 1);
		if (strlen(FileName) == 0) {
			return;
		}

		if (!InitializeMap(FileName, MapFile)) {
			MessageMenu("Loading Error"); 
			return;
		}

		/* Loading */
		if (!InitGraph()) { printf("Can't Initialize Graphics Mode 13H"); exit(1); }

		Si16 error = 0;
		error += !g_palette.Load("system/2dquake.pal");
		if (!error) {
			g_palette.Set();
			SetTextJust(1, 1);
			SetColor(127);
			OutText(160, 100, "‡   £ à ã § ª   . . .", &g_font);
			ShowScreen();
		}
		error += !LoadImage("images/frag.raw", &FragImg);
		error += !LoadImage("images/health.raw", &HealthImg);
		error += !LoadImage("images/armour.raw", &ArmourImg);
		for (Si16 i = 0; i < M_MASK; i++) {
			char tmpstr[32];
			sprintf(tmpstr, "images/mask%hd.raw", i + 1);
			error += !LoadImage(tmpstr, MaskImg + i);
		}

		for (Si16 i = 0; i < 4; i++) {
			char tmpstr[32];
			sprintf(tmpstr, "images/grfrm%hd.raw", i);
			error += !LoadImage(tmpstr, GrImg + i);
		}
		for (Si16 i = 0; i < 8; i++) {
			char tmpstr[32];
			sprintf(tmpstr, "images/rdfrm%hd.raw", i);
			error += !LoadImage(tmpstr, RdImg + i);
			sprintf(tmpstr, "images/rkfrm%hd.raw", i);
			error += !LoadImage(tmpstr, RkImg + i);
			sprintf(tmpstr, "images/corpse%hd.raw", i);
			error += !LoadImage(tmpstr, CrpsImg + i);
		}

		for (Si16 i = 0; i < 8; i++) {
			char tmpstr[32];
			sprintf(tmpstr, "images/player%hd.raw", i * 2);
			error += !LoadImage(tmpstr, ItemImg + i);
		}
		error += !LoadImage("images/hlth.raw", ItemImg + 9);
		error += !LoadImage("images/armr.raw", ItemImg + 8);
		error += !LoadImage("images/pistola.raw", ItemImg + 18);
		error += !LoadImage("images/bow.raw", ItemImg + 10);
		error += !LoadImage("images/bowa.raw", ItemImg + 19);
		error += !LoadImage("images/akm.raw", ItemImg + 11);
		error += !LoadImage("images/akma.raw", ItemImg + 20);
		error += !LoadImage("images/napalm.raw", ItemImg + 12);
		error += !LoadImage("images/napalma.raw", ItemImg + 21);
		error += !LoadImage("images/lncher.raw", ItemImg + 13);
		error += !LoadImage("images/lnchera.raw", ItemImg + 22);
		error += !LoadImage("images/photo.raw", ItemImg + 16);
		error += !LoadImage("images/grenade.raw", ItemImg + 17);
		error += !LoadImage("images/redmer.raw", ItemImg + 15);
		error += !LoadImage("images/laser.raw", ItemImg + 14);
		error += !LoadImage("images/lasera.raw", ItemImg + 23);

		if (error) {
			exitMessage = "Disk Loading or Memory Error(s)";
			ExitEditor(exitMessage);
			return;
		}

		Start();
		/* Nuclon */
		SetTextJust(2, 0);
		SetColor(0);
		ClearScreen();
		bool exiting = false;
		while (!exiting) {
			if (Input == 1) {
				vp.MsgCounter[0] = 1; vp.MsgCounter[1] = 1; vp.MsgCounter[2] = 1;
				sprintf(vp.Message[0], "Width=%hd Height=%hd", Map.Width, Map.Height);
				sprintf(vp.Message[1], "Enter New Size:");
				Si16 l = (Si16)strlen(vp.Message[2]);

				if (KF(kKeyEscape)) { KeyDown(kKeyEscape); Input = 0; continue; }
				if (l < 60) {
					if (KF('0')) {
						KeyDown('0');
						vp.Message[2][l - 1] = '0';
						vp.Message[2][l] = '_';
						vp.Message[2][l + 1] = 0;
						l++;
					}
					if (KF(' ')) {
						KeyDown(' ');
						vp.Message[2][l - 1] = ' ';
						vp.Message[2][l] = '_';
						vp.Message[2][l + 1] = 0;
						l++;
					}
					for (char i = '0'; i <= '9'; i++)
						if (KF(i)) {
							KeyDown(i);
							vp.Message[2][l - 1] = i;
							vp.Message[2][l] = '_';
							vp.Message[2][l + 1] = 0;
							l++;
						}
				}
				if (l > 1 && KF(kKeyBackspace)) {
					KeyDown(kKeyBackspace);
					l--;
					vp.Message[2][l] = 0;
					vp.Message[2][l - 1] = '_';
				}
				if (KF(kKeyEnter)) {
					KeyDown(kKeyEnter);
					Si16 neww, newh, sx = 0, sy = 0;
					// Resive and move content
					if (sscanf(vp.Message[2], "%hd %hd %hd %hd _", &neww, &newh, &sx, &sy) >= 2)
					{
						BYTE *tmpp;
						if ((tmpp = (BYTE  *)malloc(newh*neww)) == NULL)
						{
							vp.MakeMessage("Out of Memory"); Input = 0; continue;
						}
						for (Si16 iy = 0; iy < newh; iy++) {
							for (Si16 ix = 0; ix < neww; ix++) {
								if (iy >= sy && ix >= sx && iy - sy < Map.Height && ix - sx < Map.Width)
									tmpp[iy*neww + ix] = Map.Bits[(iy - sy)*Map.Width + ix - sx];
								else
									tmpp[iy*neww + ix] = cr.ter;
							}
						}
						free(Map.Bits);
						Map.Width = neww; Map.Height = newh;
						Map.Bits = tmpp;
						Input = 0; continue;
					}
				}

				vp.Show();
				ShowScreen();
				continue;
			}
			if (KF(kKeySpace)) {  // Cange Mode 0-1 //
				if (Mode == 0 || Mode == 1)
				{
					SetColor(0); ClearScreen(); KeyDown(kKeySpace); Mode = !Mode;
				}
				if (Mode == 3 || Mode == 4) {
					SetColor(0); ClearScreen(); KeyDown(kKeySpace);
					if (Mode == 3) Mode = 4;
					else Mode = 3;
				}
			}
			if (KF(kKeyF4)) {
				KeyDown(kKeyF4); // F4 //
				if (Mode == 0) { Mode = 2; vp.MakeMessage("Bot Graf Editor Activated"); }
				else if (Mode == 2) { Mode = 0; vp.MakeMessage("Bot Graf Editor Deactivated"); }
				BEAct = 0;
			}
			if (KF(kKeyF5)) {
				KeyDown(kKeyF5); // F5 //
				if (Mode == 0) { Mode = 3; vp.MakeMessage("Item Editor Activated"); }
				else if (Mode == 3) { Mode = 0; vp.MakeMessage("Item Editor Deactivated"); }
			}
			if (Mode == 1) {
				if (KF(kKeyUp)) { if (cr.ter > 15) cr.ter -= 16; KeyDown(kKeyUp); }
				if (KF(kKeyDown)) { if (cr.ter < _MpImgLoaded - 16) cr.ter += 16; KeyDown(kKeyDown); }
				if (KF(kKeyLeft)) { if (cr.ter > 0) cr.ter--; KeyDown(kKeyLeft); }
				if (KF(kKeyRight)) { if (cr.ter < _MpImgLoaded - 1) cr.ter++; KeyDown(kKeyRight); }
				if (KF(kKeyEscape)) { SetColor(0); ClearScreen(); Mode = !Mode; KeyDown(kKeyEscape); continue; }
				ShowTools(cr.ter, 15);
			}
			else if (Mode == 0) {
				if (KF(kKeyEscape)) {
					KeyDown(kKeyEscape);
					exiting = true;
					break;
				}
				if (KF(kKeyUp)) { if (cr.y > 0) cr.y--; KeyDown(kKeyUp); }
				if (KF(kKeyDown)) { if (cr.y < Map.Height - 1) cr.y++; KeyDown(kKeyDown); }
				if (KF(kKeyLeft)) { if (cr.x > 0) cr.x--; KeyDown(kKeyLeft); }
				if (KF(kKeyRight)) { if (cr.x < Map.Width - 1) cr.x++; KeyDown(kKeyRight); }

				if (KF('P')) { if (cr.ter > 0)cr.ter--; KeyDown('P'); }
				if (KF('[')) { if (cr.ter < _MpImgLoaded - 1)cr.ter++; KeyDown('['); }
				if (KF(']')) { cr.ter = Map.Bits[Map.Width*cr.y + cr.x]; KeyDown(']'); }
				if (cr.Status) { // Draw Rectangle of Terrain or One Cell //
					if (!KF(kKeyEnter)) {
						cr.Status = 0;
						for (Si16 ix = std::min(cr.tx, cr.x); ix <= std::max(cr.tx, cr.x); ix++)
							for (Si16 iy = std::min(cr.ty, cr.y); iy <= std::max(cr.ty, cr.y); iy++)
								Map.Bits[Map.Width*iy + ix] = cr.ter;
					}
				}
				else { // Draw Rectangle of Terrain or One Cell //
					if (KF(kKeyEnter)) { cr.Status = 1; cr.tx = cr.x; cr.ty = cr.y; }
				}
				if (KF(kKeyF1)) { // F1 - Info //
					char tmpstr[64];
					KeyDown(kKeyF1);
					sprintf(tmpstr, "%hd %hd :: %hd %hd", cr.x, cr.y, cr.x * 20 + 10, cr.y * 20 + 10);
					vp.MakeMessage(tmpstr);
				}
				if (KF(kKeyF2)) { // F2 - Save Map //
					char tmpstr[64];
					KeyDown(kKeyF2);
					if (SaveMap(MapFile)) sprintf(tmpstr, "\"%s\" Saved", FileName);
					else sprintf(tmpstr, "Can't Save \"%s\"", FileName);
					vp.MakeMessage(tmpstr);
				}
				if (KF(kKeyF3)) { // F3 - Resize //
					KeyDown(kKeyF3);
					vp.Message[2][0] = '_'; vp.Message[2][1] = 0;
					Input = 1;
				}
				if (KF(kKeyF8)) { // F8 - Clear //
					KeyDown(kKeyF8);
					memset(Map.Bits, cr.ter, Map.Width*Map.Height);
					vp.MakeMessage("Map Cleared");
				}
				vp.StepTo(10 + cr.x * 20, 10 + cr.y * 20);
				vp.Show();
				vp.ShowItems();
				vp.ShowCursor(cr.x, cr.y, 15);
			}
			else if (Mode == 2) {
				if (KF(kKeyEscape)) {
					KeyDown(kKeyEscape);
					exiting = true;
					break;
				}

				if (KF(kKeyUp)) { if (cr.y > 0) cr.y--; KeyDown(kKeyUp); }
				if (KF(kKeyDown)) { if (cr.y < Map.Height - 1) cr.y++; KeyDown(kKeyDown); }
				if (KF(kKeyLeft)) { if (cr.x > 0) cr.x--; KeyDown(kKeyLeft); }
				if (KF(kKeyRight)) { if (cr.x < Map.Width - 1) cr.x++; KeyDown(kKeyRight); }
				if (KF(kKeyF8)) {
					KeyDown(kKeyF8); // F8 - Clear //
					PntAmount = 0;
					vp.MakeMessage("All Points and Links Removed");
				}
				if (KF(kKeySpace)) {
					KeyDown(kKeySpace); // Space - Select Point //
					Si16 a = 0;
					for (Si16 i = ActPnt + 1; i < PntAmount; i++)
						if (pnt[i].x >= cr.x * 20 && pnt[i].y >= cr.y * 20 &&
							pnt[i].x <= cr.x * 20 + 20 && pnt[i].y <= cr.y * 20 + 20) {
							ActPnt = i;
							a = 1;
						}
					if (!a) {
						for (Si16 i = 0; i < ActPnt; i++)
							if (pnt[i].x >= cr.x * 20 && pnt[i].y >= cr.y * 20 &&
								pnt[i].x <= cr.x * 20 + 20 && pnt[i].y <= cr.y * 20 + 20) {
								ActPnt = i;
								a = 1;
							}
					}
					if (a && BEAct == 2) {
						KeyUp(kKeyEnter);
					}
				}
				if (KF('W')) {
					KeyDown('W'); // W - Next Point //
					ActPnt = (ActPnt + 1) % PntAmount;
					cr.x = Si16((pnt[ActPnt].x - 10) / 20);
					cr.y = Si16((pnt[ActPnt].y - 10) / 20);
					vp.x = cr.x * 20 + 10 - (vp.scr_x2 - vp.scr_x1) / 2;
					vp.y = cr.y * 20 + 10 - (vp.scr_y2 - vp.scr_y1) / 2;
					ActLnk = -1;
				}
				if (KF('Q')) {
					KeyDown('Q'); // Q - Prev Point //
					ActPnt = (ActPnt + PntAmount - 1) % PntAmount;
					cr.x = Si16((pnt[ActPnt].x - 10) / 20);
					cr.y = Si16((pnt[ActPnt].y - 10) / 20);
					vp.x = cr.x * 20 + 10 - (vp.scr_x2 - vp.scr_x1) / 2;
					vp.y = cr.y * 20 + 10 - (vp.scr_y2 - vp.scr_y1) / 2;
					ActLnk = -1;
				}
				if (KF('R')) {
					KeyDown('R'); // R - Next Link //
					ActLnk = (ActLnk + 1) % pnt[ActPnt].NPnts;
				}
				if (KF('E')) {
					KeyDown('E'); // E - Prev Link //
					if (ActLnk == -1) ActLnk = pnt[ActPnt].NPnts - 1;
					else ActLnk = (ActLnk + pnt[ActPnt].NPnts - 1) % pnt[ActPnt].NPnts;
				}
				if (KF('A')) {
					KeyDown('A'); // A - Create New Point: Select Place //
					BEAct = 1;
					vp.MakeMessage("TAB - Cancel");
					vp.MakeMessage("Create New Point: Select Place");
				}
				if (KF('S')) {
					KeyDown('S'); // S - Create New Link: Select Place //
					if (pnt[ActPnt].NPnts == 8) {
						vp.MakeMessage("Too Many Links from Active Point");
					}
					else {
						BEAct = 2; BESave = ActPnt;
						vp.MakeMessage("TAB - Cancel");
						vp.MakeMessage("Create New Link: Select Place");
					}
				}
				if (KF('D')) {
					KeyDown('D'); // D - Smart Draw //
					if (pnt[ActPnt].NPnts == 8) {
						vp.MakeMessage("Too Many Links from Active Point");
					}
					else {
						// Search //
						Si16 prev = ActPnt;
						Si16 a = 0;
						for (Si16 i = ActPnt + 1; i < PntAmount; i++)
							if (pnt[i].x >= cr.x * 20 && pnt[i].y >= cr.y * 20 &&
								pnt[i].x <= cr.x * 20 + 20 && pnt[i].y <= cr.y * 20 + 20) {
								ActPnt = i;
								a = 1;
							}
						if (!a) {
							for (Si16 i = 0; i < ActPnt; i++)
								if (pnt[i].x >= cr.x * 20 && pnt[i].y >= cr.y * 20 &&
									pnt[i].x <= cr.x * 20 + 20 && pnt[i].y <= cr.y * 20 + 20) {
									ActPnt = i;
									a = 1;
								}
						}
						if (!a) { // Create Point //
							pnt[PntAmount].x = cr.x * 20 + 10;
							pnt[PntAmount].y = cr.y * 20 + 10;
							pnt[PntAmount].NPnts = 0;
							ActPnt = PntAmount;
							PntAmount++;
						}
						// Create Link //
						if (pnt[ActPnt].NPnts != 8) {
							Si16 t = prev;
							pnt[t].NPnt[pnt[t].NPnts] = ActPnt;
							pnt[t].NPnts++;
							pnt[ActPnt].NPnt[pnt[ActPnt].NPnts] = t;
							pnt[ActPnt].NPnts++;
							vp.MakeMessage("Ok");
						}
						else vp.MakeMessage("Too Many Links from Selected Point");
					}
				}
				if (ActPnt >= 0 && ActPnt < PntAmount && KF('Z')) {
					KeyDown('Z'); // Z - Delete Active Point //
					vp.MakeMessage("ENTER - Ok  TAB - Cancel");
					vp.MakeMessage("Delete Active Point?");
					vp.Show();
					vp.ShowPoints();
					ShowScreen();
					while (1) {
						if (KF(kKeyEnter)) {
							KeyDown(kKeyEnter);
							DeletePoint(ActPnt);
							ActLnk = -1;
							if (ActPnt >= PntAmount) ActPnt = PntAmount - 1;
							vp.ClearMessages();
							vp.MakeMessage("Ok");
							break;
						}
						if (KF(kKeyTab)) {
							KeyDown(kKeyTab);
							vp.ClearMessages();
							vp.MakeMessage("Canceled");
							break;
						}
					}
				}
				if (ActLnk >= 0 && ActLnk < pnt[ActPnt].NPnts && KF('X')) {
					KeyDown('X'); // X - Delete Active Link //
					vp.MakeMessage("ENTER - Ok  TAB - Cancel");
					vp.MakeMessage("Delete Active Link?");
					vp.Show();
					vp.ShowPoints();
					ShowScreen();
					while (1) {
						if (KF(kKeyEnter)) {
							KeyDown(kKeyEnter);
							DeleteLink(ActPnt, ActLnk);
							ActLnk = pnt[ActPnt].NPnts - 1;
							vp.ClearMessages();
							vp.MakeMessage("Ok");
							break;
						}
						if (KF(kKeyTab)) {
							KeyDown(kKeyTab);
							vp.ClearMessages();
							vp.MakeMessage("Canceled");
							break;
						}
					}
				}
				if (KF(kKeyF2)) { // F2 - Save Points //
					char tmpstr[64];
					KeyDown(kKeyF2);
					if (SavePoints(BotFileName)) sprintf(tmpstr, "\"%s\" Saved", BotFileName);
					else sprintf(tmpstr, "Can't Save \"%s\"", BotFileName);
					vp.MakeMessage(tmpstr);
				}
				if (BEAct != 0 && KF(kKeyTab)) {
					KeyDown(kKeyTab);
					vp.MakeMessage("Canceled");
					BEAct = 0;
				}
				if (BEAct == 1) { // Create Point //
					vp.MsgCounter[0] = 1;
					if (KF(kKeyEnter)) {
						KeyDown(kKeyEnter);
						Si16 t = PntAmount;
						pnt[t].x = cr.x * 20 + 10;
						pnt[t].y = cr.y * 20 + 10;
						pnt[t].NPnts = 0;
						PntAmount++;
						BEAct = 0;
						vp.MakeMessage("Ok");
					}
				}
				if (BEAct == 2) { // Create Link //
					vp.MsgCounter[0] = 1;
					if (BESave != ActPnt && pnt[ActPnt].NPnts != 8 && KF(kKeyEnter)) {
						KeyDown(kKeyEnter);
						Si16 t = BESave;
						pnt[t].NPnt[pnt[t].NPnts] = ActPnt;
						pnt[t].NPnts++;
						pnt[ActPnt].NPnt[pnt[ActPnt].NPnts] = t;
						pnt[ActPnt].NPnts++;
						BEAct = 0;
						vp.MakeMessage("Ok");
					}
				}
				vp.StepTo(10 + cr.x * 20, 10 + cr.y * 20);
				vp.Show();
				vp.ShowItems();
				vp.ShowPoints();
				vp.ShowCursor(cr.x, cr.y, 15);
			}
			else if (Mode == 3) {
				if (KF(kKeyEscape)) {
					KeyDown(kKeyEscape);
					exiting = true;
					break;
				}

				if (KF(kKeyUp)) { if (cr.y > 0) cr.y--; KeyDown(kKeyUp); }
				if (KF(kKeyDown)) { if (cr.y < Map.Height - 1) cr.y++; KeyDown(kKeyDown); }
				if (KF(kKeyLeft)) { if (cr.x > 0) cr.x--; KeyDown(kKeyLeft); }
				if (KF(kKeyRight)) { if (cr.x < Map.Width - 1) cr.x++; KeyDown(kKeyRight); }

				if (KF(kKeyEnter)) {
					KeyDown(kKeyEnter); // Create Item //
					itm[ItmAmount].x = cr.x * 20 + 10;
					itm[ItmAmount].y = cr.y * 20 + 10;
					itm[ItmAmount].type = TerItm;
					ItmAmount++;
				}
				if (KF(kKeyF8)) {
					KeyDown(kKeyF8); // F8 - Clear //
					ItmAmount = 0;
					vp.MakeMessage("All Items Removed");
				}

				if (KF(kKeyBackspace)) {
					KeyDown(kKeyBackspace); // Delete Item //
					for (Si16 i = 0; i < ItmAmount; i++)
						if (itm[i].x >= cr.x * 20 && itm[i].y >= cr.y * 20 &&
							itm[i].x <= cr.x * 20 + 20 && itm[i].y <= cr.y * 20 + 20) {
							break;
						}
					for (Si16 i = 0; i < ItmAmount - 1; i++) itm[i] = itm[i + 1];
					ItmAmount--;
				}
				if (KF(kKeyF2)) { // F2 - Save Items //
					char tmpstr[64];
					KeyDown(kKeyF2);
					if (SaveItems(ItemFileName)) sprintf(tmpstr, "\"%s\" Saved", ItemFileName);
					else sprintf(tmpstr, "Can't Save \"%s\"", ItemFileName);
					vp.MakeMessage(tmpstr);
				}
				vp.StepTo(10 + cr.x * 20, 10 + cr.y * 20);
				vp.Show();
				vp.ShowItems();
				vp.ShowCursor(cr.x, cr.y, 15);
			}
			else if (Mode == 4) {
				if (KF(kKeyUp)) { if (TerItm > 15) TerItm -= 16; KeyDown(kKeyUp); }
				if (KF(kKeyDown)) { if (TerItm < 8) TerItm += 16; KeyDown(kKeyDown); }
				if (KF(kKeyLeft)) { if (TerItm > 0) TerItm--; KeyDown(kKeyLeft); }
				if (KF(kKeyRight)) { if (TerItm < 23) TerItm++; KeyDown(kKeyRight); }
				if (KF(kKeyEscape)) { SetColor(0); ClearScreen(); Mode = 3; KeyDown(kKeyEscape); continue; }
				ShowItemTools(TerItm, 15);
			}

			ShowScreen();
		}

		ExitEditor(exitMessage);
	}
}
