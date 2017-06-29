#pragma once

#include "graph.h"
#include "2dqdefs.h"

#include <stdlib.h>
#include <stdarg.h>

extern Si64 GameStep;
extern Si16 SecondCounter;
extern long GameTime;
extern tIMAGE Map;
extern VIEWPORT vp[2];
extern OBJECT *blt;
extern PLAYER *plr;
extern PLREVENT *plrevn;
extern BYTE Players;
extern BYTE PlayersIn;
extern BYTE BotsIn;

extern char Name1[80];
extern char Name2[80];
extern char BotFileName[80];
extern char ItemFileName[80];

extern Si16 MaxAmmo[M_WPN];
extern Si16 AddAmmo[M_WPN];

extern BYTE Campaign;
extern BYTE TryExit;
extern BYTE EndGame;   // EG_NOEG || EG_TIMELIMIT || EG_FRAGLIMIT //
extern BYTE NoMessages;
extern char Control1;
extern char Control2;
extern char DC_Control1;
extern char DC_Control2;
extern Si16  NeedRead;
extern BYTE CheckFrag;
extern BYTE DrawStatistics;
extern Si16 DrawWeapon[M_PLR];
extern BYTE FragSort[M_PLR];
extern tIMAGE FragImg;
extern tIMAGE HealthImg;
extern tIMAGE ArmourImg;
extern tIMAGE ItemImg[M_ITEM];
extern tIMAGE WpnImg[M_WPN];
extern tIMAGE PlImg[M_PLRFRM];
extern tIMAGE PlfImg[M_PLRFRM];
extern tIMAGE PldImg[M_PLRFRM];
extern tIMAGE Pls1Img[M_PLRFRM];
extern tIMAGE Pls2Img[M_PLRFRM];
extern tIMAGE MpImg[M_MAPELM];
extern tIMAGE MaskImg[M_MASK];
extern tIMAGE AimImg;
extern tIMAGE BallImg[4];
extern tIMAGE GrImg[4];
extern tIMAGE RkImg[8];
extern tIMAGE RdImg[8];
extern tIMAGE CrpsImg[8];
extern BYTE MapNum;
extern BYTE MapPos;
extern Si16  MapPosX[M_POS];
extern Si16  MapPosY[M_POS];
extern double MapPosA[M_POS];
extern BYTE MF_PlrSee[M_MAPELM];
extern BYTE MF_BltMov[M_MAPELM];
extern BYTE MF_PlrMov[M_MAPELM];
extern BYTE F_MovFrm[4];
extern BYTE PlrWpnPrior[M_WPN];
extern Si16 WpnPrior[M_WPN + 1];
extern BYTE PEventSize[6];

extern char KillText[M_BLOWKILLTEXT][64];
extern char SelfKillText[M_SELFKILLTEXT][64];

extern BYTE CheckBltExt;
extern Si16 BltExt;  // non-free blts amount (with holes) //
extern Si16 BltFst;  // first free blt (there is no holes before) //

extern BOT *bot;
extern Si16 _ItemAmount;

extern void(*MapHandler)(void);
extern void(*Controller1)(PLAYER *);
extern void(*Controller2)(PLAYER *);

extern CONFIG cfg;

/* From SYSTEMFN.CPP */
//extern void SendCmnd(void);
Si16 Random(Si16);
Si16 InitRandom(void);

/* From GAME.CPP */
void NextFreeBlt(void);
BYTE FF_PlrSee(Si16, Si16);
BYTE FF_BltMov(Si16, Si16);
BYTE FF_PlrMov(Si16, Si16);
void MakePlayerEvent(Si16 n, Si16 EventID, ...);
void EmptyDraw(Si16, Si16, void *, VIEWPORT*);

/* From VIEWTOOL.CPP */
BYTE Blast(Si16, Si16, Si16, Si16, Si16);
BYTE Visible(BYTE a, double, double);
void CenterFocusing(void *);
void FarFocusing(void *);

BYTE Blast(Si16, Si16, Si16, Si16, Si16);
BYTE Border(Si16, Si16, Si16, Si16);
BYTE FireLine(Si16, Si16, Si16, Si16);
BYTE Visible(BYTE, double, double);

/* From BULLETS.CPP */
void B_BloodHandler(void *);
void B_SmokeHandler(void *);
void B_Smoke2Handler(void *);
void B_GilzaHandler(void *);
void B_ShellHandler(void *);
void B_ArrowHandler(void *);
void B_NapalmHandler(void *);
void B_Napalm2Handler(void *);
void B_RocketHandler(void *);
void B_PhotoA0Handler(void *);
void B_PhotoA2Handler(void *);
void B_PhotoA4Handler(void *);
void B_PhotoA6Handler(void *);
void B_PhotoBoomHandler(void *);
void B_GrenadeHandler(void *);
void B_RedmerHandler(void *);
void B_LaserHandler(void *);
void B_AimHandler(void *);
void B_BallHandler(void *);
void B_BloodDraw(Si16, Si16, void *, VIEWPORT*);
void B_SmokeDraw(Si16, Si16, void *, VIEWPORT*);
void B_Smoke2Draw(Si16, Si16, void *, VIEWPORT*);
void B_GilzaDraw(Si16, Si16, void *, VIEWPORT*);
void B_ShellDraw(Si16, Si16, void *, VIEWPORT*);
void B_ArrowDraw(Si16, Si16, void *, VIEWPORT*);
void B_NapalmDraw(Si16, Si16, void *, VIEWPORT*);
void B_RocketDraw(Si16, Si16, void *, VIEWPORT*);
void B_PhotoHorzDraw(Si16, Si16, void *, VIEWPORT*);
void B_PhotoVertDraw(Si16, Si16, void *, VIEWPORT*);
void B_GrenadeDraw(Si16, Si16, void *, VIEWPORT*);
void B_RedmerDraw(Si16, Si16, void *, VIEWPORT*);
void B_LaserDraw(Si16, Si16, void *, VIEWPORT*);
void B_AimDraw(Si16, Si16, void *, VIEWPORT*);
void B_BallDraw(Si16, Si16, void *, VIEWPORT*);

/* From ITEMS.CPP */
void I_HealthHandler(void *);
void I_ArmourHandler(void *);
void I_PistolAmmoHandler(void *);
void I_BowHandler(void *);
void I_BowAmmoHandler(void *);
void I_AkmHandler(void *);
void I_AkmAmmoHandler(void *);
void I_NapalmHandler(void *);
void I_NapalmAmmoHandler(void *);
void I_LauncherHandler(void *);
void I_LauncherAmmoHandler(void *);
void I_PhotoHandler(void *);
void I_GrenadeHandler(void *);
void I_RedmerHandler(void *);
void I_LaserHandler(void *);
void I_LaserAmmoHandler(void *);
void I_CorpseHandler(void *);
void I_PackHandler(void *);

void I_HealthDraw(Si16, Si16, void *, VIEWPORT*);
void I_ArmourDraw(Si16, Si16, void *, VIEWPORT*);
void I_PistolAmmoDraw(Si16, Si16, void *, VIEWPORT*);
void I_BowDraw(Si16, Si16, void *, VIEWPORT*);
void I_BowAmmoDraw(Si16, Si16, void *, VIEWPORT*);
void I_AkmDraw(Si16, Si16, void *, VIEWPORT*);
void I_AkmAmmoDraw(Si16, Si16, void *, VIEWPORT*);
void I_NapalmDraw(Si16, Si16, void *, VIEWPORT*);
void I_NapalmAmmoDraw(Si16, Si16, void *, VIEWPORT*);
void I_LauncherDraw(Si16, Si16, void *, VIEWPORT*);
void I_LauncherAmmoDraw(Si16, Si16, void *, VIEWPORT*);
void I_PhotoDraw(Si16, Si16, void *, VIEWPORT*);
void I_GrenadeDraw(Si16, Si16, void *, VIEWPORT*);
void I_RedmerDraw(Si16, Si16, void *, VIEWPORT*);
void I_LaserDraw(Si16, Si16, void *, VIEWPORT*);
void I_LaserAmmoDraw(Si16, Si16, void *, VIEWPORT*);
void I_CorpseDraw(Si16, Si16, void *, VIEWPORT*);
void I_PackDraw(Si16, Si16, void *, VIEWPORT*);

/* From MAPS.CPP */
void MapInit(BYTE);
void M_Map0Handler();

/* From BOTS.CPP */
void InitBots(BYTE BotAmount, BYTE FirstBot, BYTE MapNumber);
void CloseBots(void);

/* From PLAYERS.CPP */
void C_Demo(PLAYER *);
void C_KMBattle(PLAYER *);
void C_K1Battle(PLAYER *);
void C_K2Battle(PLAYER *);
void M_Battle(PLAYER *);
void M_NoMove(PLAYER *);
void MC_Battle(PLAYER *);
void MC_Dead(PLAYER *);

void ExitGame(char *, Si16 Exit = 1);
