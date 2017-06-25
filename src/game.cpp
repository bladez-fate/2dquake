#include "game.h"

long GameTime;
tIMAGE Map;
VIEWPORT vp[2];
OBJECT *blt;
PLAYER *plr;
PLREVENT *plrevn;

BYTE Players;
BYTE PlayersIn;
BYTE BotsIn;

char Name1[80];
char Name2[80];
char BotFileName[80];
char ItemFileName[80];

Si16 MaxAmmo[M_WPN] = { 200,150,300,50,30,5,10,1,2000 };
Si16 AddAmmo[M_WPN] = { BA_SHELL,BA_ARROW,BA_AKM,BA_NAPALM,BA_ROCKET,BA_PHOTO,BA_GRENADE,BA_REDMER,BA_LASER };

//Si16 NetStatus;
Si16 SecondCounter;
BYTE Campaign;
BYTE TryExit;
BYTE EndGame;   // EG_NOEG || EG_TIMELIMIT || EG_FRAGLIMIT //
BYTE NoMessages;
char Control1;
char Control2;
char DC_Control1;
char DC_Control2;
Si16  NeedRead;
BYTE CheckFrag;
BYTE DrawStatistics;
Si16 DrawWeapon[M_PLR];
BYTE FragSort[M_PLR];
tIMAGE FragImg;
tIMAGE HealthImg;
tIMAGE ArmourImg;
tIMAGE ItemImg[M_ITEM];
tIMAGE WpnImg[M_WPN];
tIMAGE PlImg[M_PLRFRM];
tIMAGE PlfImg[M_PLRFRM];
tIMAGE PldImg[M_PLRFRM];
tIMAGE Pls1Img[M_PLRFRM];
tIMAGE Pls2Img[M_PLRFRM];
tIMAGE MpImg[M_MAPELM];
tIMAGE MaskImg[M_MASK];
tIMAGE AimImg;
tIMAGE BallImg[4];
tIMAGE GrImg[4];
tIMAGE RkImg[8];
tIMAGE RdImg[8];
tIMAGE CrpsImg[8];
BYTE MapNum;
BYTE MapPos;
Si16  MapPosX[M_POS];
Si16  MapPosY[M_POS];
double MapPosA[M_POS];
BYTE MF_PlrSee[M_MAPELM];
BYTE MF_BltMov[M_MAPELM];
BYTE MF_PlrMov[M_MAPELM];
BYTE F_MovFrm[4] = { 0,1,0,2 };
BYTE PlrWpnPrior[M_WPN] = { 4,5,7,6,8,1,3,2,9 };  // Choose Algoritm //
Si16 WpnPrior[M_WPN + 1] = { 10,30,40,50,60,   5,4,3,70,0 }; // For Bots
BYTE PEventSize[6] = { 2,4,2,3,1,5 };

char KillText[M_BLOWKILLTEXT][64] = {
	"%s Замочил %s",
	"%s Пристрелил %s",
	"%s Шлепнул %s",
	"%s Добил %s",
	"%s Убил %s",

	"%s Лопнул %s",
	"%s Разорвал %s",
};
char SelfKillText[M_SELFKILLTEXT][64] = {
	"%s-Самоубийца",
	"%s Совершил Харакири",
	"%s Подох",
	"%s Самоликвидировался",
	"%s Поджег Свой Арсенал",
	"%s Тихо Скончался",
	"R.I.P. %s"
};


BYTE CheckBltExt;
Si16 BltExt;  // non-free blts amount (with holes) //
Si16 BltFst;  // first free blt (there is no holes before) //

Si16 _ItemAmount;   // Especially for Bots //

void(*MapHandler)(void);
void(*Controller1)(PLAYER *);
void(*Controller2)(PLAYER *);

CONFIG cfg;

void NextFreeBlt(void)
{
	for (; BltFst<M_BLT; BltFst++)
		if (!blt[BltFst].age) {
			if (BltFst >= BltExt) BltExt = BltFst + 1;
			return;
		}
}

BYTE FF_PlrSee(Si16 ix, Si16 iy)
{
	BYTE masknum = MF_PlrSee[Map.Bits[Si16(iy / 20)*Map.Width + Si16(ix / 20)]];
	return MaskImg[masknum].Bits[(iy % 20) * 20 + ix % 20];
}

BYTE FF_BltMov(Si16 ix, Si16 iy)
{
	BYTE masknum = MF_BltMov[Map.Bits[Si16(iy / 20)*Map.Width + Si16(ix / 20)]];
	return MaskImg[masknum].Bits[(iy % 20) * 20 + ix % 20];
}

BYTE FF_PlrMov(Si16 ix, Si16 iy)
{
	BYTE masknum = MF_PlrMov[Map.Bits[Si16(iy / 20)*Map.Width + Si16(ix / 20)]];
	return MaskImg[masknum].Bits[(iy % 20) * 20 + ix % 20];
}

void MakePlayerEvent(Si16 n, Si16 EventID, ...)
{
	va_list ap;
	va_start(ap, EventID);
	plrevn[n].Code[plrevn[n].Length] = EventID;
	switch (EventID) {
	case PE_DEATH:
		plrevn[n].Code[plrevn[n].Length + 1] = va_arg(ap, BYTE);
		break;
	case PE_DAMAGE:
		*(Si16*)(plrevn[n].Code + plrevn[n].Length + 1) = va_arg(ap, Si16);
		plrevn[n].Code[plrevn[n].Length + 3] = va_arg(ap, BYTE);
		break;
	case PE_SEEPLR:
		plrevn[n].Code[plrevn[n].Length + 1] = va_arg(ap, BYTE);
		break;
	case PE_SEEBLT:
		*(Si16*)(plrevn[n].Code + plrevn[n].Length + 1) = va_arg(ap, Si16);
		break;
	case PE_NOAMMO:
		break;
	case PE_TELEPORT:
		*(Si16*)(plrevn[n].Code + plrevn[n].Length + 1) = va_arg(ap, Si16);
		*(Si16*)(plrevn[n].Code + plrevn[n].Length + 3) = va_arg(ap, Si16);
		break;
	}
	plrevn[n].Length += PEventSize[EventID];
	va_end(ap);
}

void EmptyDraw(Si16, Si16, void *)
{
	return;
}