/*
2D Quake v1.06
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/
#include "defs.h"

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdarg.h>
#include <time.h>

#include "graph.h"
#include "keyboard.h"
#include "keydefs2.h"
#include "menu.h"
#include "game.h"
#include "2dqdefs.h"

#include "engine/easy.h"

using namespace arctic;
using namespace arctic::easy;

namespace quake2d_editor {
	void main();
}

/* Menu Misc */
BYTE Demo;
char MapName[64];
BYTE BotEnable;
Si16 mnuMainAmount=5;
char mnuMain[5][15] = {
	"Новая Игра",
	"Разработчики",
	"Демо",
	"Редактор",
	"Выход"
};
Si16 mnuGameAmount = 10;
char mnuGame[10][30] = {
	"Кол-во Игроков     ",
	"Кол-во Ботов       ",
	"Предел Фрагов        ",
	"Предел Времени       ",
	"Карта                     ",
	"Имя Игрока 1            ",
	"Имя Игрока 2            ",
	"Сетевой Статус        ",
	"Старт",
	"Обратно"
};
Si16 mnuCreditsAmount=13;
char mnuCredits[13][50] = {
    "-= 2D QUAKE =-",
    "Version 1.07 (10.07.2002 - 14.06.2017)",
    "",
	"      Code   bladez-fate",
    "      Arts   bladez-fate",
    "Moral help   B. Alexander",
    "  Scenario   bladez-fate",
    "             B. Sergey",
    "  Testers    B. Andrey",
    "             bladez-fate",
	"",
	"  POWERED BY ARCTIC ENGINE",
    ""
};

/* Functions Prototypes */
void Start(void);
void Damage(void);
void Kill(BYTE,BYTE,Si16,BYTE);
Si16 InitializeMap(Si16);
void NextMap(Si16);
void CloseMap(void);
BYTE MainMenu(void);
BYTE GameMenu(void);
void HelpMenu(void);
void CreditsMenu(void);

VIEWPORT::VIEWPORT()
{
    MsgCounter[0]=0;MsgCounter[1]=0;
    MsgCounter[2]=0;MsgCounter[3]=0;
}

void VIEWPORT::Init(Si16 _scr_x1, Si16 _scr_y1, Si16 _scr_x2, Si16 _scr_y2)
{
	scr_x1 = _scr_x1;
	scr_y1 = _scr_y1;
	scr_x2 = _scr_x2;
	scr_y2 = _scr_y2;
	fogFilter.reset(new FogFilter(
		scr_x1, scr_y1, scr_x2, scr_y2, ScreenCenter().x, ScreenCenter().y));
	dmgFilter.reset(new DmgFilter(
		scr_x1, scr_y1, scr_x2, scr_y2));
	filters.fog_ = fogFilter.get();
	filters.dmg_ = dmgFilter.get();
}

void VIEWPORT::MakeMessage(char *MsgText,Si16 Important)
{
    if(NoMessages && !Important) return;
    strcpy(Message[3],Message[2]); MsgCounter[3]=MsgCounter[2];
    strcpy(Message[2],Message[1]); MsgCounter[2]=MsgCounter[1];
    strcpy(Message[1],Message[0]); MsgCounter[1]=MsgCounter[0];
    strcpy(Message[0],MsgText);
    if(Important) MsgCounter[0]=(Important+2)*MSG_READWAIT;
    else MsgCounter[0]=MSG_READWAIT;
}

void VIEWPORT::ClearMessages(void)
{
	MsgCounter[0] = 0;
}

void VIEWPORT::Show(void)
{
	MakeBufferWindow(scr_x1, scr_y1, scr_x2, scr_y2);
	if (NoFocusing) {
		fogFilter->Refresh();
	}
	else {
		fogFilter->Refresh((float)plr[ActPlayer].a, (float)V_FOV);
	}
	dmgFilter->Refresh(plr[ActPlayer].damageLast);

	EnableFilters(&filters);

	constexpr BYTE defaultTile = 1;

	// Prepare fog filter
	for (long iy = y / 20 - 1; iy < (long)(y + scr_y2 - scr_y1) / 20 + 1; iy++) {
		int yy = iy * 20 - y + scr_y1;
		for (long ix = x / 20 - 1; ix < (long)(x + scr_x2 - scr_x1) / 20 + 1; ix++) {
			int xx = ix * 20 - x + scr_x1;
			BYTE tile = defaultTile;
			if (ix >= 0 && iy >= 0 && ix < Map.Width && iy < Map.Height) {
				tile = Map.Bits[iy*Map.Width + ix];
			}

			BYTE masknum = MF_PlrSee[tile];
			tIMAGE& msk = MaskImg[masknum];
			BYTE* v = msk.Bits;
			int pos = screen::pixel(xx, yy);
			for (int my = 0, yyy = yy; my < msk.Height; my++, yyy++, pos += screen::w - msk.Width) {
				for (int mx = 0, xxx = xx; mx < msk.Width; mx++, xxx++, pos++, v++) {
					if (xxx >= scr_x1 && yyy >= scr_y1 &&
						xxx <= scr_x2 && yyy <= scr_y2)
					{
						if (!*v) {
							fogFilter->AddOpaquePixel(pos);
						}
					}
				}
			}
		}
	}

    BYTE da;
    long i,ix,iy;
	for (iy = y / 20 - 1; iy < (long)(y + scr_y2 - scr_y1) / 20 + 1; iy++) {
		for (ix = x / 20 - 1; ix < (long)(x + scr_x2 - scr_x1) / 20 + 1; ix++) {
			if (ix >= 0 && iy >= 0 && ix < Map.Width && iy < Map.Height) {
				i = Map.Bits[iy*Map.Width + ix];
			}
			else {
				i = defaultTile;
			}
			if (MapNum != 0 || i < 29 || i>48) {
				DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + i);
			}
			else {
				if (NoFocusing || Visible(ActPlayer, ix * 20, iy * 20) ||
					Visible(ActPlayer, ix * 20 + 19, iy * 20) ||
					Visible(ActPlayer, ix * 20, iy * 20 + 19) ||
					Visible(ActPlayer, ix * 20 + 19, iy * 20 + 19)) {
					DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + i);
				}
				else {
					if (i > 29 && i < 34) {
						DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + 29); 
						continue; 
					}
					if (i > 34 && i < 39) {
						DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + 34);
						continue; 
					}
					if (i > 39 && i < 44) {
						DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + 39);
						continue; 
					}
					if (i > 44 && i < 49) { 
						DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + 44);
						continue; 
					}
					DrawWImage(ix * 20 - x + scr_x1, iy * 20 - y + scr_y1, MpImg + i);
				}
			}
		}
	}
    Si16 xx,yy;
    BYTE plrvis[M_PLR];
	if (!NoFocusing) {
		Si16 CodeCur = 0;
		while (CodeCur != plrevn[ActPlayer].Length) {
			if (plrevn[ActPlayer].Code[CodeCur] == PE_SEEBLT) {
				i = *((WORD *)(plrevn[ActPlayer].Code + CodeCur + 1));
				if (blt[i].Handler == I_CorpseHandler) {
					blt[i].Draw(blt[i].x - x + scr_x1, blt[i].y - y + scr_y1, &(blt[i]));
				}
			}
			CodeCur += PEventSize[plrevn[ActPlayer].Code[CodeCur]];
		}
		CodeCur = 0;
		while (CodeCur != plrevn[ActPlayer].Length) {
			if (plrevn[ActPlayer].Code[CodeCur] == PE_SEEBLT) {
				i = *((WORD *)(plrevn[ActPlayer].Code + CodeCur + 1));
				if (blt[i].Handler != I_CorpseHandler) {
					blt[i].Draw(blt[i].x - x + scr_x1, blt[i].y - y + scr_y1, &(blt[i]));
				}
			}
			CodeCur += PEventSize[plrevn[ActPlayer].Code[CodeCur]];
		}
		memset(plrvis, 0, M_PLR);
		CodeCur = 0;
		while (CodeCur != plrevn[ActPlayer].Length) {
			if (plrevn[ActPlayer].Code[CodeCur] == PE_SEEPLR) {
				i = plrevn[ActPlayer].Code[CodeCur + 1];
				plrvis[i] = 1;
				da = ((BYTE)((plr[i].a + M_PI / 16) * 8 / M_PI)) % 16;
				xx = plr[i].x - x + scr_x1;
				yy = plr[i].y - y + scr_y1;
				if (plr[i].damgfrm) {
					DrawSWUImage(xx - 15, yy - 15, i, PldImg + da);
				}
				else if (plr[i].firefrm) {
					DrawSWUImage(xx - 15, yy - 15, i, PlfImg + da);
				}
				else {
					BYTE aa = F_MovFrm[plr[i].movefrm / M_STEPLEN];
					if (aa == 0) {
						DrawSWUImage(xx - 15, yy - 15, i, PlImg + da);
					}
					else if (aa == 1) {
						DrawSWUImage(xx - 15, yy - 15, i, Pls1Img + da);
					}
					else {
						DrawSWUImage(xx - 15, yy - 15, i, Pls2Img + da);
					}
				}
			}
			CodeCur += PEventSize[plrevn[ActPlayer].Code[CodeCur]];
		}
		i = ActPlayer;
		da = ((BYTE)((plr[i].a + M_PI / 16) * 8 / M_PI)) % 16;
		xx = plr[i].x - x + scr_x1; 
		yy = plr[i].y - y + scr_y1;
		if (plr[i].damgfrm) {
			DrawSWUImage(xx - 15, yy - 15, i, PldImg + da);
		}
		else if (plr[i].firefrm) {
			DrawSWUImage(xx - 15, yy - 15, i, PlfImg + da);
		}
		else {
			BYTE aa = F_MovFrm[plr[i].movefrm / M_STEPLEN];
			if (aa == 0) {
				DrawSWUImage(xx - 15, yy - 15, i, PlImg + da);
			}
			else if (aa == 1) {
				DrawSWUImage(xx - 15, yy - 15, i, Pls1Img + da);
			}
			else {
				DrawSWUImage(xx - 15, yy - 15, i, Pls2Img + da);
			}
		}
	}
    else {
        for(Si16 i=0;i<BltExt;i++)
            if(blt[i].Handler==I_CorpseHandler && blt[i].age)
                blt[i].Draw(blt[i].x-x+scr_x1,blt[i].y-y+scr_y1,&(blt[i]));
        for(Si16 i=0;i<BltExt;i++)
            if(blt[i].age && blt[i].Handler!=I_CorpseHandler)
                blt[i].Draw(blt[i].x-x+scr_x1,blt[i].y-y+scr_y1,&(blt[i]));
        memset(plrvis,0,M_PLR);
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            plrvis[i]=1;
            da=((BYTE)((plr[i].a+M_PI/16)*8/M_PI))%16;
            xx=plr[i].x-x+scr_x1;yy=plr[i].y-y+scr_y1;
            if(plr[i].damgfrm) DrawSWUImage(xx-15,yy-15,i,PldImg+da);
            else if(plr[i].firefrm) DrawSWUImage(xx-15,yy-15,i,PlfImg+da);
            else {
                BYTE aa=F_MovFrm[plr[i].movefrm/M_STEPLEN];
                if(aa==0) {DrawSWUImage(xx-15,yy-15,i,PlImg+da);continue;}
                if(aa==1) {DrawSWUImage(xx-15,yy-15,i,Pls1Img+da);continue;}
                DrawSWUImage(xx-15,yy-15,i,Pls2Img+da);
            }
        }
    }

	DisableFilters();

	// Bot Point for Debug
	if (showBotPoints) {
		void ShowBotPoints(VIEWPORT* vp, bool showIds);
		ShowBotPoints(this, showBotPoints == 2);
	}

    // Names //
    if(ShowNames) {
        ShowNames=0;
        SetTextJust(1,1);
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plrvis[i] && i!=ActPlayer) {
                SetColor(181+(i%4)*18);
                xx=plr[i].x-x+scr_x1;
                yy=plr[i].y-y+scr_y1;
                if(xx>=scr_x1 && yy>=scr_y1 && xx<=scr_x2 && yy+4<=scr_y2) {
                    Si16 ss=(Si16)strlen(plr[i].Name)*4;
                    if(xx-ss-1<scr_x1) xx=scr_x1+ss+1;
                    else if(xx+ss>scr_x2) xx=scr_x2-ss;
                    if( yy-5<scr_y1) yy=scr_y1+5;
                    else if( yy+4>scr_y2) yy=scr_y2-4;
                    OutText(xx,yy,plr[i].Name,&g_font);
                }
            }
        }
    }

    if(plr[ActPlayer].trn==P_TRN) SetColor(14);
    else SetColor(78);
    xx=plr[ActPlayer].x-x+scr_x1+CUR_RANGE*cos(plr[ActPlayer].a);
    yy=plr[ActPlayer].y-y+scr_y1-CUR_RANGE*sin(plr[ActPlayer].a);
    WinBufPixel(xx+1,yy);
    WinBufPixel(xx-1,yy);
    WinBufPixel(xx,yy+1);
    WinBufPixel(xx,yy-1);
    /* Show Messages */
    SetTextJust(1,0);
    {
        Si16 a=0;
        for(Si16 i=0;i<4 && MsgCounter[i];i++) {
            if(MsgCounter[i]<=MSG_READWAIT) SetColor(127);
            else SetColor((MsgCounter[i]-MSG_READWAIT-1)*16/MSG_READWAIT);
            if((Si16)strlen(Message[i]) > (scr_x2-scr_x1)/8-2) {
                char tmpstr[64];
                memcpy(tmpstr,Message[i],strlen(Message[i])/2);
                tmpstr[strlen(Message[i])/2]=0;
                OutText((scr_x2+scr_x1)/2,scr_y1+2+10*a,tmpstr,&g_font);
                a++;
                memcpy(tmpstr,Message[i]+strlen(Message[i])/2,strlen(Message[i])/2+strlen(Message[i])%2);
                tmpstr[strlen(Message[i])/2+strlen(Message[i])%2]=0;
                OutText((scr_x2+scr_x1)/2,scr_y1+2+10*a,tmpstr,&g_font);
                a++;
            }
            else {
                OutText((scr_x2+scr_x1)/2,scr_y1+2+10*a,Message[i],&g_font);
                a++;
            }
            /* ??? WHAT'S THE MAZA ??? */
            if(!NoFocusing) {
                MsgCounter[i]--;
                if(!(MsgCounter[i]%MSG_READWAIT)) MsgCounter[i]=0;
            }
        }
    }
    SetTextJust(2,0);
    NoBufferWindow();
    /* Draw Statistics */
	if (DrawStatistics) {
		SetColor(0);
		FrameRect(scr_x1 - 1, scr_y1 - 1, scr_x2 + 1, scr_y2 + 1);
		SetColor(47);
		VertLine(
			scr_y2 - plr[ActPlayer].hp*((double)(scr_y2 - scr_y1) / M_HEALTH),
			scr_y2,
			scr_x1 - 1);
		if (plr[ActPlayer].ammo[plr[ActPlayer].actwpn]) {
			SetColor(79);
			HorzLine(scr_x1,
				scr_x1 + plr[ActPlayer].ammo[plr[ActPlayer].actwpn] * ((double)(scr_x2 - scr_x1) / MaxAmmo[plr[ActPlayer].actwpn]),
				scr_y1 - 1);
		}
		if (plr[ActPlayer].armr) {
			SetColor(31);
			HorzLine(scr_x1,
				scr_x1 + plr[ActPlayer].armr*((double)(scr_x2 - scr_x1) / M_ARMOUR),
				scr_y2 + 1);
		}
	}
    DrawSImage(scr_x1+4,scr_y1+4,WpnImg+plr[ActPlayer].actwpn);
    if(DrawWeapon[ActPlayer]) {
        char tmpstr[16];
        DrawWeapon[ActPlayer]--;
        SetColor(127);
        _itoa(plr[ActPlayer].ammo[plr[ActPlayer].actwpn],tmpstr,10);
        OutText(scr_x1+49,scr_y1+10,tmpstr,&g_font);
        DrawSImage(scr_x1+4,scr_y2-23,&HealthImg);
        _itoa(plr[ActPlayer].hp,tmpstr,10);
        OutText(scr_x1+49,scr_y2-17,tmpstr,&g_font);
        if(plr[ActPlayer].armr) {
            DrawSImage(scr_x1+4,scr_y2-48,&ArmourImg);
            _itoa(plr[ActPlayer].armr,tmpstr,10);
            OutText(scr_x1+49,scr_y2-42,tmpstr,&g_font);
        }
        /* Names & Frags */
        if(DrawWeapon[ActPlayer]>=SHOW_WPN) {
            DrawWeapon[ActPlayer]++;
            for(Si16 i=0;i<Players && i<11;i++) if(plr[FragSort[i]].State) {
                _itoa(plr[FragSort[i]].frag,tmpstr,10);
                if(FragSort[i]==ActPlayer) SetColor(79);
                else SetColor(127);
                DrawSWUImage(scr_x2-45,18*i,FragSort[i],&FragImg);
                OutText(scr_x2,scr_y1+5+18*i,tmpstr,&g_font);
                OutText(scr_x2-50,scr_y1+5+18*i,plr[FragSort[i]].Name,&g_font);
            }
        }
        else {
            _itoa(plr[ActPlayer].frag,tmpstr,10);
            SetColor(127);
            DrawSWUImage(scr_x2-45,scr_y1+4,ActPlayer,&FragImg);
            OutText(scr_x2,scr_y1+10,tmpstr,&g_font);
        }
    }
}

void PLAYER :: ResetCmnd(void)
{
    for(Si16 i=0;i<M_CMND;i++) cmnd[i]=0;
}

void Start(void)
{
    for(Si16 i=0;i<M_BLT;i++) {
        blt[i].own=-1;
        blt[i].age=0;
        blt[i].x=0;
        blt[i].y=0;
        blt[i].sx=0;
        blt[i].sy=0;
        blt[i].Draw=B_ShellDraw;
        blt[i].Handler=B_ShellHandler;
    }
    for(Si16 i=0;i<M_PLR;i++) FragSort[i]=i;
    MapInit(MapNum);
    DrawWeapon[0]=SHOW_WPN; DrawWeapon[1]=SHOW_WPN;
    DrawWeapon[2]=SHOW_WPN; DrawWeapon[3]=SHOW_WPN;
    GameTime=0; SecondCounter=0;
    TryExit=0; NoMessages=0;
    CheckBltExt=1; BltExt=M_BLT; BltFst=0;
    EndGame=EG_NOEG;
    CheckFrag=0;
    DrawStatistics=1;
    vp[0].Focusing=CenterFocusing;
    vp[1].Focusing=CenterFocusing;
    Controller1=C_KMBattle;Controller2=C_K2Battle;
    vp[0].ClearMessages();vp[1].ClearMessages();
    if(cfg.TimeLimit) {
        char tmpstr[32];
        if((cfg.TimeLimit%100>10 && cfg.TimeLimit%100<20)
                || cfg.TimeLimit%10>4 || cfg.TimeLimit%10==0)
            sprintf(tmpstr,"%hd Минут",cfg.TimeLimit);
        else if(cfg.TimeLimit%10>1) sprintf(tmpstr,"%hd Минуты",cfg.TimeLimit);
        else sprintf(tmpstr,"%hd Минута",cfg.TimeLimit);
        vp[0].MakeMessage(tmpstr,MSG_TIME);vp[1].MakeMessage(tmpstr,MSG_TIME);
        vp[0].MakeMessage(MapName,MSG_TIME);vp[1].MakeMessage(MapName,MSG_TIME);
    }
    if(PlayersIn==1) {
		vp[0].Init(1, 1, screen::w - 2, screen::h - 2);
        vp[0].x=0; vp[0].y=0;
		//vp[0].scr_x1=1; vp[0].scr_y1=1;
        //vp[0].scr_x2=318; vp[0].scr_y2=198;
		vp[1].Init(0, 0, 0, 0);
		vp[1].x=0; vp[1].y=0;
		//vp[1].scr_x1=0; vp[1].scr_y1=0;
        //vp[1].scr_x2=0; vp[1].scr_y2=0;
        //if(NetStatus==0)
		{
            Control1=0;plr[0].Control=PC_KEY1;
            Control2=-1;
            Players=1+BotsIn;
            if(Demo) {
                Players=BotsIn;
                InitBots(BotsIn,0,MapNum);
            }
			else {
				InitBots(BotsIn, 1, MapNum);
			} 
        }
    }
    else {
		vp[0].Init(screen::cx + 1, 1, screen::w - 2, screen::h - 2);
        vp[0].x=0; vp[0].y=0;
		//vp[0].scr_x1=161; vp[0].scr_y1=1;
        //vp[0].scr_x2=318; vp[0].scr_y2=198;
        vp[1].x=0; vp[1].y=0;
		vp[1].Init(1, 1, screen::cx - 2, screen::h - 2);
		//vp[1].scr_x1=1; vp[1].scr_y1=1;
        //vp[1].scr_x2=158; vp[1].scr_y2=198;
        //if(!NetStatus)
		{
            Control1=0;plr[0].Control=PC_KEY1;
            Control2=1;plr[1].Control=PC_KEY2;
            Players=2+BotsIn;
            if(BotsIn) InitBots(BotsIn,2,MapNum);
        }
    }

    //if(!NetStatus)
	{
        /* Not Net Naming */
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].Control==PC_KEY1) strcpy(plr[i].Name,Name1);
            if(plr[i].Control==PC_KEY2) strcpy(plr[i].Name,Name2);
            //		if(plr[i].Control==PC_BOT ) strcpy(plr[i].Name,cfg.BotName[i]);
            //      Bot Named in InitBots function
        }
    }

    /*if(NetStatus==1) {
        //// Connecting ... ////
        Si16 rd;
        ShowScreenImage("IMAGES\\MENU.RAW");
        SetTextJust(1,1);
        SetColor(MENU_COL1);
        OutText(160,100,"C O N N E C T I N G . . .",&g_font);
        ShowScreen();
        while(!SerialReaded) {if(KF(kKeyEscape)) {ExitGame("",0); TryExit=1; return;}}
        rd=SerialReadByte[0]; SerialReaded=0;
        SerialPortWrite(PlayersIn);
        Players=PlayersIn+rd;
        NeedRead=M_CMND*rd;
        if(PlayersIn==1) {
            Control1=0; Control2=-1;
            plr[0].Control=PC_KEY1;
            if(rd==1) {
                DC_Control1=1;DC_Control2=-1;
                plr[1].Control=PC_NET;
            }
            else {
                DC_Control1=1;DC_Control2=2;
                plr[1].Control=PC_NET;plr[2].Control=PC_NET;
            }
        }
        else {
            Control1=0; Control2=1;
            plr[0].Control=PC_KEY1;plr[1].Control=PC_KEY2;
            if(rd==1) {
                DC_Control1=2;DC_Control2=-1;
                plr[2].Control=PC_NET;
            }
            else {
                DC_Control1=2;DC_Control2=3;
                plr[2].Control=PC_NET;plr[3].Control=PC_NET;
            }
        }
        while(!SerialReaded) {if(KF(kKeyEscape)) {ExitGame("",0); TryExit=1; return;}}
        rd=SerialReadByte[0]; SerialReaded=0;
        SerialPortWrite(BotsIn);

        NeedRead+=M_CMND*rd;
        NeedRead=NeedRead/8+!!(NeedRead%8)+2;
        if(BotsIn) InitBots(BotsIn,Players,MapNum);
        Players+=BotsIn;
        for(Si16 i=Players;i<Players+rd;i++) {
            plr[i].Control=PC_NET;
        }
        Players+=rd;
        // Net Naming (Get)
        while(!SerialReaded || SerialReadByte[SerialReaded-1]!=0xFF); // Wait for Opponent // Get Names //
        char *p=(char *)SerialReadByte;
        for(Si16 i=0;i<Players;i++)
            if(plr[i].Control==PC_NET) {strcpy(plr[i].Name,p); p+=16;}
        SerialReaded=0;
        // Net Naming (Make & Send)
        Si16 WillSend;
        for(Si16 i=0;i<Players;i++)
        {
            WillSend=0;
            if(plr[i].Control==PC_KEY1) {strcpy(plr[i].Name,Name1);WillSend=1;}
            if(plr[i].Control==PC_KEY2) {strcpy(plr[i].Name,Name2);WillSend=1;}
            if(plr[i].Control==PC_BOT ) {
				//strcpy(plr[i].Name,cfg.BotName[i]);
				WillSend=1;
			}
            if(WillSend) {
                for(Si16 j=0;j<16;j++)
                    SerialPortWrite(plr[i].Name[j]);
            }
        }
        SerialPortWrite(0xFF);
        while(!SerialReaded);
        SerialReaded=0; // Wait for Opponent // Get Names //
    }
    if(NetStatus==2) {
        //// Connecting ... ////
        Si16 rd;
        ShowScreenImage("IMAGES\\MENU.RAW");
        SetTextJust(1,1);
        SetColor(MENU_COL1);
        OutText(160,100,"C O N N E C T I N G . . .",&g_font);
        ShowScreen();
        SerialPortWrite(PlayersIn);
        while(!SerialReaded) {if(KF(kKeyEscape)) {ExitGame("",0); TryExit=1; return;}}
        rd=SerialReadByte[0]; SerialReaded=0;
        NeedRead=M_CMND*rd;
        Players=PlayersIn+rd;
        if(rd==1) {
            DC_Control1=0; DC_Control2=-1;
            plr[0].Control=PC_NET;
            if(PlayersIn==1) {
                Control1=1;Control2=-1;
                plr[1].Control=PC_KEY1;
            }
            else {
                Control1=1;Control2=2;
                plr[1].Control=PC_KEY1;plr[2].Control=PC_KEY2;
            }
        }
        else {
            DC_Control1=0; DC_Control2=1;
            plr[0].Control=PC_NET;plr[1].Control=PC_NET;
            if(PlayersIn==1) {
                Control1=2;Control2=-1;
                plr[2].Control=PC_KEY1;
            }
            else {
                Control1=2;Control2=3;
                plr[2].Control=PC_KEY1;plr[3].Control=PC_KEY2;
            }
        }
        SerialPortWrite(BotsIn);
        while(!SerialReaded) {if(KF(kKeyEscape)) {ExitGame("",0); TryExit=1; return;}}
        rd=SerialReadByte[0]; SerialReaded=0;
        NeedRead+=M_CMND*rd;
        NeedRead=NeedRead/8+!!(NeedRead%8)+2;
        for(Si16 i=Players;i<Players+rd;i++) {
            plr[i].Control=PC_NET;
        }
        Players+=rd;
        if(BotsIn) InitBots(BotsIn,Players,MapNum);
        Players+=BotsIn;
        // Net Naming (Make & Send)
        Si16 WillSend;
        for(Si16 i=0;i<Players;i++)
        {
            WillSend=0;
            if(plr[i].Control==PC_KEY1) {strcpy(plr[i].Name,Name1);WillSend=1;}
            if(plr[i].Control==PC_KEY2) {strcpy(plr[i].Name,Name2);WillSend=1;}
            if(plr[i].Control==PC_BOT ) {
				//strcpy(plr[i].Name,cfg.BotName[i]);
				WillSend=1;
			}
            if(WillSend) {
                for(Si16 j=0;j<16;j++)
                    SerialPortWrite(plr[i].Name[j]);
            }
        }
        SerialPortWrite(0xFF);
        // Net Naming (Get) 
        while(!SerialReaded || SerialReadByte[SerialReaded-1]!=0xFF); // Wait for Opponent // Get Names //
        char *p=(char *)SerialReadByte;
        for(Si16 i=0;i<Players;i++)
            if(plr[i].Control==PC_NET) {strcpy(plr[i].Name,p); p+=16;}
        SerialReaded=0;
        SerialPortWrite(0x00);
    }*/

    vp[0].ActPlayer=Control1;
    vp[1].ActPlayer=Control2;

    if(Demo) {
        Controller1=C_Demo;
        //vp[0].Focusing=FarFocusing;
    }

    for(Si16 i=0;i<Players;i++) {plr[i].State=PS_OUT;}
    for(Si16 i=0;i<Players;i++)
    {
        plrevn[i].Length=0;
        plr[i].frag=0;
        plr[i].n=i;
        Kill(i,0,0,0);
    }
}

void Kill(BYTE n,BYTE corpse,Si16 a,BYTE killer)
{
    if(a) {  // Battle Kill //
        Si16 txtnum;
        if(killer==n) {
            char tmpstr[64];
            sprintf(tmpstr,SelfKillText[random(M_SELFKILLTEXT)],plr[n].Name);
            if(n==vp[0].ActPlayer) vp[0].MakeMessage(tmpstr,MSG_DEATH);
            else vp[0].MakeMessage(tmpstr);
            if(n==vp[1].ActPlayer) vp[1].MakeMessage(tmpstr,MSG_DEATH);
            else vp[1].MakeMessage(tmpstr);
        }
        else {
            if(corpse) txtnum=random(M_KILLTEXT);
            else txtnum=M_KILLTEXT+random(M_BLOWKILLTEXT-M_KILLTEXT);
            char tmpstr[64];
            sprintf(tmpstr,KillText[txtnum],plr[killer].Name,plr[n].Name);
            if(n==vp[0].ActPlayer) vp[0].MakeMessage(tmpstr,MSG_DEATH);
            else if(killer==vp[0].ActPlayer) vp[0].MakeMessage(tmpstr,MSG_KILL);
            else vp[0].MakeMessage(tmpstr);
            if(n==vp[1].ActPlayer) vp[1].MakeMessage(tmpstr,MSG_DEATH);
            else if(killer==vp[1].ActPlayer) vp[1].MakeMessage(tmpstr,MSG_KILL);
            else vp[1].MakeMessage(tmpstr);
        }

        if(vp[0].ActPlayer==n) vp[0].NoFocusing=1;
        else if(vp[1].ActPlayer==n) vp[1].NoFocusing=1;
        plr[n].reload=DEAD_TIME;
        plr[n].State=PS_DEAD;
        plr[n].Move=M_NoMove;
        plr[n].MakeCmnd=MC_Dead;

        if(plr[n].wpn[B_REDMER]>1) {
            plr[n].wpn[B_REDMER]=0;
            if(vp[0].ActPlayer==n)
                CenterFocusing(vp);
            else if(vp[1].ActPlayer==n)
                CenterFocusing(vp+1);
        }
        if(corpse) {
            Si16 j;
            NextFreeBlt();
            j=BltFst;
            blt[j].age=((BYTE)((plr[n].a+M_PI/8)*4/M_PI))%8+1;
            blt[j].own=n%4;blt[j].x=plr[n].x;blt[j].y=plr[n].y;
            blt[j].sx=plr[n].sx;
            blt[j].sy=plr[n].sy;
            blt[j].Handler=I_CorpseHandler;
            blt[j].Draw=I_CorpseDraw;
        }
        {for(Si16 j;a>0;a--) {
                NextFreeBlt();
                j=BltFst;
                blt[j].age=1+Random(6);
                blt[j].own=n;
                blt[j].x=plr[n].x-4+Random(9);
                blt[j].y=plr[n].y-4+Random(9);
                blt[j].sx=plr[n].sx+(double)(-30+Random(61))/15.;
                blt[j].sy=plr[n].sy+(double)(-30+Random(61))/15.;
                blt[j].Handler=B_BloodHandler;
                blt[j].Draw=B_BloodDraw;
            }
        }

        {Si16 i;
            NextFreeBlt();
            i=BltFst;
            blt[i].age=1;
            blt[i].own=n;
            blt[i].x=plr[n].x;
            blt[i].y=plr[n].y;
			PACK **pk = &blt[i].pack; // ((PACK **)(blt + i) + 4);
			if((*pk = (PACK *)malloc(sizeof(PACK)))==NULL)
                ExitGame("Kill: No Memory for Create Pack");
            (*pk)->sx=plr[n].sx-1+Random(61)/30;
            blt[i].sy=plr[n].sy-1+Random(61)/30;
            blt[i].Handler=I_PackHandler;
            blt[i].Draw=I_PackDraw;
            (*pk)->Shell=plr[n].ammo[B_SHELL];
            (*pk)->Arrow=plr[n].ammo[B_ARROW];
            (*pk)->AkmAmmo=plr[n].ammo[B_AKM];
            (*pk)->NapalmAmmo=plr[n].ammo[B_NAPALM];
            (*pk)->Rocket=plr[n].ammo[B_ROCKET];
            (*pk)->LaserAmmo=plr[n].ammo[B_LASER];
            if(plr[n].actwpn==B_GRENADE && !plr[n].ammo[B_GRENADE]) (*pk)->Weapon=M_WPN;
            else (*pk)->Weapon=plr[n].actwpn;
        }
        if(plr[n].actwpn==B_LASER && plr[n].wpn[B_LASER]>1) {
            Si16 i;
            NextFreeBlt();
            i=BltFst;
            blt[i].age=plr[n].wpn[B_LASER]-1;
            blt[i].own=n;
            blt[i].x=plr[n].x+WPN_RANGE*cos(plr[n].a);
            blt[i].y=plr[n].y-WPN_RANGE*sin(plr[n].a);
            blt[i].sx=S_LASER*cos(plr[n].a);
            blt[i].sy=-S_LASER*sin(plr[n].a);
            blt[i].Handler=B_LaserHandler;
            blt[i].Draw=B_LaserDraw;
        }
        else if(plr[n].actwpn==B_GRENADE && plr[n].wpn[B_GRENADE]>1) {
            Si16 i;
            NextFreeBlt();
            i=BltFst;
            blt[i].age=plr[n].wpn[B_GRENADE]-1;
            blt[i].own=n;
            blt[i].x=plr[n].x+WPN_RANGE*cos(plr[n].a);
            blt[i].y=plr[n].y-WPN_RANGE*sin(plr[n].a);
			blt[i].sx = (SP_GRENADE * plr[n].sx + S_GRENADE*cos(plr[n].a))/2;
			blt[i].sy = (SP_GRENADE * plr[n].sy - S_GRENADE*sin(plr[n].a))/2;
            blt[i].Handler=B_GrenadeHandler;
            blt[i].Draw=B_GrenadeDraw;
        }
    }
    else { // First Kill && Such as First Kills --- Fast Start //
        plr[n].reload=0;
        plr[n].Move=M_Battle;
        plr[n].MakeCmnd=MC_Battle;
        plr[n].State=PS_BATTLE;
    }
    // All Kill Modes //
    DrawStatistics=1;
    plr[n].damgfrm=0;
    plr[n].firefrm=0;
    plr[n].movefrm=0;
    plr[n].damage=0;
    plr[n].damageown=0;
    plr[n].hp=M_HEALTH;
    plr[n].armr=0;
    {for(Si16 i=0;i<M_CMND;i++) {plr[n].cmnd[i]=0;}}
    BYTE maxprior=0;
    {for(Si16 i=0;i<M_WPN;i++) {
            if(cfg.StartWeapon[i]!=0) {
                plr[n].wpn[i]=1;
                if(PlrWpnPrior[i]>maxprior) {plr[n].actwpn=i;maxprior=PlrWpnPrior[i];}
            }
            else plr[n].wpn[i]=0;
            plr[n].ammo[i]=cfg.StartWeapon[i];
        }
    }
    plr[n].acc=P_ACC;
    plr[n].trn=P_TRN;
    plr[n].sx=0; plr[n].sy=0;

    Si16 pos;
    Si16 X;
    Si16 Y;
    BYTE PosFlag[22],NoPos=0;
    memset(PosFlag,0,22);
    //if(NetStatus!=2)
	for(;;) {
        Si16 checkpos=1;
		pos = random(MapPos);
        X=MapPosX[pos];
        Y=MapPosY[pos];
		Si16 i;
		for(i=0;i<MapPos;i++) if(!PosFlag[i]) break;
        if(i==MapPos) {
            X+=cos(MapPosA[pos])*15;
            Y-=sin(MapPosA[pos])*15;
            NoPos=128;
        }
        for(Si16 i=0;i<Players;i++) {
            if(n==i || plr[i].State<=0) continue;
            Si16 ix,iy;
            ix=X+P_SZ;iy=Y+P_SZ;
            if(ix>=plr[i].x-P_SZ && ix<=plr[i].x+P_SZ && iy>=plr[i].y-P_SZ && iy<=plr[i].y+P_SZ) {checkpos=0;break;}
            ix=X-P_SZ;iy=Y-P_SZ;
            if(ix>=plr[i].x-P_SZ && ix<=plr[i].x+P_SZ && iy>=plr[i].y-P_SZ && iy<=plr[i].y+P_SZ) {checkpos=0;break;}
            ix=X+P_SZ;iy=Y-P_SZ;
            if(ix>=plr[i].x-P_SZ && ix<=plr[i].x+P_SZ && iy>=plr[i].y-P_SZ && iy<=plr[i].y+P_SZ) {checkpos=0;break;}
            ix=X-P_SZ;iy=Y+P_SZ;
            if(ix>=plr[i].x-P_SZ && ix<=plr[i].x+P_SZ && iy>=plr[i].y-P_SZ && iy<=plr[i].y+P_SZ) {checkpos=0;break;}
        }
        if(checkpos) break;
        else PosFlag[pos]=1;
    }

    /*if(NetStatus==1) {
        while(!SerialReaded); SerialReaded=0;
        SerialPortWrite(pos+NoPos);
    }
    if(NetStatus==2) {
        SerialPortWrite(0x00);
        while(!SerialReaded);
        pos=SerialReadByte[0]; SerialReaded=0;
        if(pos&0x80) {
            pos-=128;
            X=MapPosX[pos]+cos(MapPosA[pos])*15;
            Y=MapPosY[pos]-sin(MapPosA[pos])*15;
        }
        else {
            X=MapPosX[pos];
            Y=MapPosY[pos];
        }
    }*/
    plr[n].x=X;
    plr[n].y=Y;
    plr[n].a=MapPosA[pos];
    MakePlayerEvent(n,PE_DEATH,(BYTE)pos);
}

void Damage(void)
{
    for(Si16 i=0;i<Players;i++) {
        if(!plr[i].State) continue;
		if(plr[i].damage) {
            Si16 DamageSave=plr[i].damage;
            BYTE DamageOwn=plr[i].damageown;
            if(plr[i].armr>=plr[i].damage/2+1) {
                if(plr[i].damage%2) {plr[i].damage=plr[i].damage/2; plr[i].armr-=plr[i].damage+1;}
                else {plr[i].damage=plr[i].damage/2; plr[i].armr-=plr[i].damage;}
            }
            else
            {plr[i].damage-=plr[i].armr;plr[i].armr=0;}
            plr[i].hp-=plr[i].damage;
            plr[i].damage=0;
            if(plr[i].hp<=0)
            {
                Si16 plrido=plr[i].damageown;
                DrawWeapon[i]=SHOW_WPN;
                DrawWeapon[plrido]=SHOW_WPN;
                if(plrido<0 && plrido>=Players)
                {plr[i].damageown=i;plrido=i;}
                if(plrido==i) plr[i].frag--;
                else plr[plrido].frag++;
				Si16 k;
                for(k=0;k<Players;k++) if(plr[FragSort[k]].frag<plr[plrido].frag) break;
				Si16 l;
				for(l=0;l<Players;l++) if(FragSort[l]==plrido) break;
                if(k>l+1) {
                    BYTE save=FragSort[l];
                    for(Si16 j=l+1;j<k;j++) FragSort[j-1]=FragSort[j];
                    FragSort[k-1]=save;
                }
                if(k<l) {
                    BYTE save=FragSort[l];
                    for(Si16 j=l-1;j>=k;j--) FragSort[j+1]=FragSort[j];
                    FragSort[k]=save;
                }
				if (cfg.FragLimit && plr[plr[i].damageown].frag >= cfg.FragLimit) {
					EndGame = EG_FRAGLIMIT;
				}
				if (plr[i].hp < -50) {
					Kill(i, 0, BLOWN_BLOOD, plr[i].damageown);
				} else {
					Kill(i, 1, KILL_BLOOD, plr[i].damageown);
				}
            } else {
                MakePlayerEvent(i,PE_DAMAGE,DamageSave,DamageOwn);
            }
        }
    }
}

void ExitGame(char *ExitText, Si16 Exit)
{
    FreeImage(&FragImg);
    FreeImage(&HealthImg);
    FreeImage(&ArmourImg);
    FreeImage(&AimImg);
    for(Si16 i=0;i<M_WPN;i++) FreeImage(WpnImg+i);
    for(Si16 i=0;i<8;i++) FreeImage(CrpsImg+i);
    for(Si16 i=0;i<4;i++) FreeImage(BallImg+i);
    for(Si16 i=0;i<M_PLRFRM;i++) {
        FreeImage(PlImg+i);
        FreeImage(PlfImg+i);
        FreeImage(Pls1Img+i);
        FreeImage(Pls2Img+i);
        FreeImage(PldImg+i);
    }
    for(Si16 i=0;i<M_MASK;i++) FreeImage(MaskImg+i);
    for(Si16 i=0;i<M_ITEM;i++) FreeImage(ItemImg+i);
    for(Si16 i=0;i<4;i++) FreeImage(GrImg+i);
    for(Si16 i=0;i<8;i++) {FreeImage(RdImg+i);FreeImage(RkImg+i);}
    CloseMap();
	for (Si16 i = 0; i<BltExt; i++) {
		if (blt[i].age && blt[i].Handler == I_PackHandler)
			free(blt[i].pack);
	}
	free(plr);
    free(blt);
    free(plrevn);
    CloseBots();
    //if(NetStatus) SerialPortClose();
    if (Exit) {
        FreeFont(&g_font);
        printf(ExitText);
        exit(0);
    }
}

Si16 _MpImgLoaded=0;
void CloseMap(void)
{
    for(Si16 i=0;i<_MpImgLoaded;i++)
        FreeImage(MpImg+i);
    FreeImage(&Map);
}
void NextMap(Si16 NewMapNum)
{
    FILE *src;
    if((src=fopen("MAPS\\MAPS.CFG","rt"))==NULL) {return;}
    char tmpstr[64];
    Si16 tmpi;
    Si16 MapAmount;
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"mapamount")!=0) {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return;}
    fscanf(src," %hd",&MapAmount);
    MapNum=(NewMapNum+MapAmount)%MapAmount;
    for(Si16 i=0;i<MapAmount;i++)
    {
        fscanf(src," %[a-zA-Z]",tmpstr);
        _strlwr(tmpstr);
        if(strcmp(tmpstr,"mapfilename")!=0) {fclose(src);return;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='[') {fclose(src);return;}
        fscanf(src," %hd",&tmpi);
        if(i!=tmpi) {fclose(src);return;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!=']') {fclose(src);return;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='=') {fclose(src);return;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='"') {fclose(src);return;}
        fscanf(src,"%[^\"]",tmpstr);
        if(i==MapNum) break;
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='"') {fclose(src);return;}
    }
    fclose(src);
    if((src=fopen(tmpstr,"rt"))==NULL) {return;}
    /* Name */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"name")!=0) {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return;}
    fscanf(src,"%[^\"]",MapName);
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return;}
    /* MapFile */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"mapfile")!=0) {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return;}
    fscanf(src,"%[^\"]",tmpstr);
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return;}
    /* BotFile */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"botfile")!=0) {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return;}
    fscanf(src,"%[^\"]",tmpstr);
    fclose(src);
    if((src=fopen(tmpstr,"rt"))==NULL) {return;}
    fscanf(src,"%hd",&tmpi);
    if(tmpi==0)
        BotEnable=0;
    else
        BotEnable=1;
    fclose(src);
}

Si16 InitializeMap(Si16 N)
{
    FILE *src;
    if((src=fopen("MAPS\\MAPS.CFG","rt"))==NULL) {return 0;}
    char tmpstr[64];
    Si16 tmpi;
    Si16 MapAmount;
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"mapamount")!=0) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return 0;}
    fscanf(src," %hd",&MapAmount);
    if(N>=MapAmount) {fclose(src);return 0;}
    for(Si16 i=0;i<MapAmount;i++)
    {
        fscanf(src," %[a-zA-Z]",tmpstr);
        _strlwr(tmpstr);
        if(strcmp(tmpstr,"mapfilename")!=0) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='[') {fclose(src);return 0;}
        fscanf(src," %hd",&tmpi);
        if(i!=tmpi) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!=']') {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='=') {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='"') {fclose(src);return 0;}
        fscanf(src,"%[^\"]",tmpstr);
        if(i==N) break;
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='"') {fclose(src);return 0;}
    }
    fclose(src);
    if((src=fopen(tmpstr,"rt"))==NULL) {return 0;}
    Si16 TerAmount;
    /* Name */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"name")!=0) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    fscanf(src,"%[^\"]",tmpstr);
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    /* MapFile */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"mapfile")!=0) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    fscanf(src,"%[^\"]",tmpstr);
    if(!LoadImage(tmpstr,&Map)) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    /* BotFile */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"botfile")!=0) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    fscanf(src,"%[^\"]",BotFileName);
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    /* ItemFile */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"itemfile")!=0) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    fscanf(src,"%[^\"]",ItemFileName);
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='"') {fclose(src);return 0;}
    /* Terrain Images */
    fscanf(src," %[a-zA-Z]",tmpstr);
    _strlwr(tmpstr);
    if(strcmp(tmpstr,"terrainimageamount")!=0) {fclose(src);return 0;}
    fscanf(src," %c",tmpstr);
    if(tmpstr[0]!='=') {fclose(src);return 0;}
    fscanf(src," %hd",&TerAmount);

    for(Si16 i=0;i<TerAmount;i++)
    {
        fscanf(src," %[a-zA-Z]",tmpstr);
        _strlwr(tmpstr);
        if(strcmp(tmpstr,"mapimage")!=0) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='[') {fclose(src);return 0;}
        fscanf(src," %hd",&tmpi);
        if(i!=tmpi) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!=']') {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='=') {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='"') {fclose(src);return 0;}
        fscanf(src,"%[^\"]",tmpstr);
        if(!LoadImage(tmpstr,MpImg+i)) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='"') {fclose(src);return 0;}

        fscanf(src," %[a-zA-Z]",tmpstr);
        _strlwr(tmpstr);
        if(strcmp(tmpstr,"mapmasks")!=0) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='[') {fclose(src);return 0;}
        fscanf(src," %hd",&tmpi);
        if(i!=tmpi) {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!=']') {fclose(src);return 0;}
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='=') {fclose(src);return 0;}

        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='{') {fclose(src);return 0;}
        fscanf(src,"%hd",&tmpi); MF_PlrSee[i]=tmpi;
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!=',') {fclose(src);return 0;}
        fscanf(src,"%hd",&tmpi); MF_BltMov[i]=tmpi;
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!=',') {fclose(src);return 0;}
        fscanf(src,"%hd",&tmpi); MF_PlrMov[i]=tmpi;
        fscanf(src," %c",tmpstr);
        if(tmpstr[0]!='}') {fclose(src);return 0;}
    }

    fclose(src);
    _MpImgLoaded=TerAmount;
    return 1;
}

BYTE MainMenu(void)
{
    SetColor(0);ClearScreen();
    ShowScreen();
    g_palette.Load("MENU.PAL");
    g_palette.Set();
    SetTextJust(1,1);
    Si16 cur=0;
    while(1)
    {
        ShowScreenImage("IMAGES\\MENU.RAW");
		int txtx = screen::cx; // 160
		int txty = screen::h * 3 / 4; // 152

        for(Si16 i=0;i<mnuMainAmount;i++) {
            SetColor(MENU_COL3);OutText(txtx + 2,txty+2-mnuMainAmount*10+i*20,mnuMain[i],&g_font);
            SetColor(i==cur?MENU_COL2:MENU_COL1);OutText(txtx,txty-mnuMainAmount*10+i*20,mnuMain[i],&g_font);
        }
        ShowScreen();
        if(KF(kKeyEscape)) {KeyDown(kKeyEscape);cur=-1;break;}
        if(KF(kKeyEnter)) {KeyDown(kKeyEnter);
            Si16 a=0;
            switch(cur) {
            case 0: if(GameMenu()) {a=1;} break;
            case 1: CreditsMenu(); break;
            case 2: HelpMenu(); a=1; break;
			case 3: 
				quake2d_editor::main();
				g_palette.Load("MENU.PAL");
				g_palette.Set();
				SetTextJust(1, 1);
				break;
			case 4: cur=-1;a=1;break;
            }
            if(a) break;
        }
        if(KF(kKeyUp)) {KeyDown(kKeyUp);cur=(cur+mnuMainAmount-1)%mnuMainAmount;}
        if(KF(kKeyDown)) {KeyDown(kKeyDown);cur=(cur+1)%mnuMainAmount;}
    }
    SetColor(0);
	ClearScreen();
    ShowScreen();
    g_palette.Load("2DQUAKE.PAL");
    g_palette.Set();
    return cur;
}
BYTE GameMenu(void)
{
    Si16 cur=0;
    Demo=0;
    NextMap(MapNum);
    while(1)
    {
        ShowScreenImage("IMAGES\\MENU.RAW");
		for (Si16 i = 0; i < mnuGameAmount; i++) {
			Si16 y = screen::cy - mnuGameAmount * 8 + i * 16;
			Si16 x;
			char tmpstr[30];
			switch (i) {
			case 0: sprintf(tmpstr, "%hd", PlayersIn); x = screen::cx + 80; break;
			case 1: sprintf(tmpstr, "%hd", BotsIn); x = screen::cx + 80; break;
			case 2: sprintf(tmpstr, "%hd", cfg.FragLimit); x = screen::cx + 88; break;
			case 3: sprintf(tmpstr, "%hd", cfg.TimeLimit); x = screen::cx + 88; break;
			case 4: strcpy(tmpstr, MapName); x = screen::cx + 60; break;
			case 5: strcpy(tmpstr, Name1); x = screen::cx + 60; break;
			case 6: strcpy(tmpstr, Name2); x = screen::cx + 60; break;
				//case 7: strcpy(tmpstr, NetStatus ? ((NetStatus - 1) ? "Second" : "ПFirst") : "НNo"); x = 220; break;
			case 7: strcpy(tmpstr, "Local"); x = screen::cx + 60; break;
			default: tmpstr[0] = 0;
			}
			SetColor(MENU_COL3);
			OutText(screen::cx + 2, y + 2, mnuGame[i], &g_font);
			OutText(x + 2, y + 2, tmpstr, &g_font);
			SetColor(i == cur ? MENU_COL2 : MENU_COL1);
			if (i == 1 && !BotEnable) {
				SetColor(MENU_COL4);
			}
			OutText(screen::cx, y, mnuGame[i], &g_font);
			OutText(x, y, tmpstr, &g_font);
		}
        ShowScreen();
        if(KF(kKeyEscape)) {KeyDown(kKeyEscape);cur=0;break;}
        if(KF(kKeyEnter)) {KeyDown(kKeyEnter);
            switch(cur) {
            case 5: InputMenu(Name1,"Enter 1st Player Name"); break;
            case 6: InputMenu(Name2,"Enter 2nd Player Name"); break;
            case 8: if(!BotEnable) BotsIn=0; return 1;
            case 9: return 0;
            }
        }
        if(KF(kKeyLeft)) {KeyDown(kKeyLeft);
            switch(cur) {
            case 0: PlayersIn=1+(PlayersIn)%2; break;
            case 1: BotsIn=(BotsIn+9)%10; break;
            case 2: if(cfg.FragLimit>0)cfg.FragLimit--; break;
            case 3: if(cfg.TimeLimit>0)cfg.TimeLimit--; break;
            case 4: NextMap(MapNum-1); break;
            //case 7: NetStatus=(NetStatus+2)%3; break;
            }
        }
        if(KF(kKeyRight)) {KeyDown(kKeyRight);
            switch(cur) {
            case 0: PlayersIn=1+(PlayersIn)%2; break;
            case 1: BotsIn=(BotsIn+1)%10; break;
            case 2: if(cfg.FragLimit<1000) cfg.FragLimit++; break;
            case 3: if(cfg.TimeLimit<1000) cfg.TimeLimit++; break;
            case 4: NextMap(MapNum+1); break;
            //case 7: NetStatus=(NetStatus+1)%3; break;
            }
        }
        if(KF(kKeyUp)) {
            KeyDown(kKeyUp);
            cur=(cur+mnuGameAmount-1)%mnuGameAmount;
            if(cur==1 && !BotEnable) cur=(cur+mnuGameAmount-1)%mnuGameAmount;
        }
        if(KF(kKeyDown)) {
            KeyDown(kKeyDown);
            cur=(cur+1)%mnuGameAmount;
            if(cur==1 && !BotEnable) cur=(cur+1)%mnuGameAmount;
        }
    }
    return cur;
}

void CreditsMenu(void)
{
    SetColor(0);ClearScreen();
    ShowScreen();
    g_palette.Load("CREDITS.PAL");
    g_palette.Set();
    SetTextJust(1,1);
    ShowScreenImage("IMAGES\\CREDITS.RAW");
    ShowScreen();
	for (int i = 120; i > 0; i--) {
		ShowScreen();
		Sleep(1.0 / 60.0);
	}
	for (Si16 i = 0; i < mnuCreditsAmount; i++) {
		SetColor(0);
		OutText(screen::cx + 2, screen::cy + 4 - mnuCreditsAmount * 5 + i * 10, mnuCredits[i], &g_font);
		SetColor(254);
		OutText(screen::cx, screen::cy + 2 - mnuCreditsAmount * 5 + i * 10, mnuCredits[i], &g_font);
	}
    ShowScreen();
    while(1) {
		if(KF(kKeyEnter)) {
			KeyDown(kKeyEnter);
			break;
		}
		ShowScreen();
	}
    SetColor(0);ClearScreen();
    ShowScreen();
    g_palette.Load("MENU.PAL");
    g_palette.Set();
}

void HelpMenu(void) // DEMO //
{
	do {
		MapNum = random(7);
	} while (MapNum == 2 || MapNum == 3);
    //MapNum=6;
    NextMap(MapNum);
    BotsIn=7;PlayersIn=1;Demo=1;
    //NetStatus=0;
    cfg.FragLimit=0;
    cfg.TimeLimit=0;
}

/************/
/*** MAIN ***/
/************/
void QuakeMain(void)
{
    /* Config Loading */
    cfg.Load("2DQUAKE.CFG");

    /* Default Startup Settings */
	//NetStatus = 0;
	Campaign = MapNum = 6; PlayersIn = 1; BotsIn = 9;
    strcpy(Name1,cfg.Name1); strcpy(Name2,cfg.Name2);

    /* Loading & Connecting */
    Si16 error=0;
    if(!InitGraph()) {printf("Can't Initialize Graphics Mode 13H"); exit(1);}
    error+=!LoadFont("STAND.SFN",&g_font);
    //InitKeyHandler();

    while(1) {
        if(MainMenu()==0xFF) {
            FreeFont(&g_font);
            break;
        }
        //if(NetStatus) {SerialPortInit(cfg.PortNum,cfg.PortSpeed);}

        if ((plr = (PLAYER *) malloc(M_PLR*sizeof(PLAYER))) == NULL) error++;
        if ((blt = (OBJECT *) malloc(M_BLT*sizeof(OBJECT))) == NULL) error++;
        if ((plrevn = (PLREVENT *) malloc(M_PLR*sizeof(PLREVENT))) == NULL) error++;

		memset(plr, 0, M_PLR * sizeof(PLAYER));
		memset(blt, 0, M_BLT * sizeof(OBJECT));
		memset(plrevn, 0, M_PLR * sizeof(PLREVENT));

        error+=!g_palette.Load("2DQUAKE.PAL");
        error+=!g_palette.Load("MENU.PAL");
        if(!error) {
            g_palette.Set();
            ShowScreenImage("IMAGES\\MENU.RAW");
            SetTextJust(1,1);
            SetColor(MENU_COL3);
			OutText(screen::cx + 2, screen::cy + 2, "L O A D I N G . . .", &g_font);
            SetColor(MENU_COL1);
			OutText(screen::cx, screen::cy, "L O A D I N G . . .", &g_font);
            ShowScreen();
        }
        error+=!LoadImage("IMAGES\\FRAG.RAW",&FragImg);
        error+=!LoadImage("IMAGES\\HEALTH.RAW",&HealthImg);
        error+=!LoadImage("IMAGES\\ARMOUR.RAW",&ArmourImg);
        error+=!LoadImage("IMAGES\\AIM.RAW",&AimImg);
        error+=!LoadImage("IMAGES\\BALL1.RAW",BallImg);
        error+=!LoadImage("IMAGES\\BALL2.RAW",BallImg+1);
        error+=!LoadImage("IMAGES\\BALL3.RAW",BallImg+2);
        error+=!LoadImage("IMAGES\\BALL4.RAW",BallImg+3);
        for(Si16 i=0;i<M_MASK;i++) {
            char tmpstr[32];
            sprintf(tmpstr,"IMAGES\\MASK%hd.RAW",i+1);
            error+=!LoadImage(tmpstr,MaskImg+i);
        }
        for(Si16 i=0;i<M_WPN;i++) {
            char tmpstr[32];
            sprintf(tmpstr,"IMAGES\\WEAPON%hd.RAW",i);
            error+=!LoadImage(tmpstr,WpnImg+i);
        }
        for(Si16 i=0;i<M_PLRFRM;i++) {
            char tmpstr[32];
            sprintf(tmpstr,"IMAGES\\PLAYER%hd.RAW",i);
            error+=!LoadImage(tmpstr,PlImg+i);
            sprintf(tmpstr,"IMAGES\\PLAY1S%hd.RAW",i);
            error+=!LoadImage(tmpstr,Pls1Img+i);
            sprintf(tmpstr,"IMAGES\\PLAY2S%hd.RAW",i);
            error+=!LoadImage(tmpstr,Pls2Img+i);
            sprintf(tmpstr,"IMAGES\\PLAYF%hd.RAW",i);
            error+=!LoadImage(tmpstr,PlfImg+i);
            sprintf(tmpstr,"IMAGES\\PLAYD%hd.RAW",i);
            error+=!LoadImage(tmpstr,PldImg+i);
        }

        error+=!InitializeMap(MapNum);

        error+=!LoadImage("IMAGES\\HLTH.RAW",ItemImg);
        error+=!LoadImage("IMAGES\\ARMR.RAW",ItemImg+1);
        error+=!LoadImage("IMAGES\\PISTOLA.RAW",ItemImg+2);
        error+=!LoadImage("IMAGES\\BOW.RAW",ItemImg+3);
        error+=!LoadImage("IMAGES\\BOWA.RAW",ItemImg+4);
        error+=!LoadImage("IMAGES\\AKM.RAW",ItemImg+5);
        error+=!LoadImage("IMAGES\\AKMA.RAW",ItemImg+6);
        error+=!LoadImage("IMAGES\\NAPALM.RAW",ItemImg+7);
        error+=!LoadImage("IMAGES\\NAPALMA.RAW",ItemImg+8);
        error+=!LoadImage("IMAGES\\LNCHER.RAW",ItemImg+9);
        error+=!LoadImage("IMAGES\\LNCHERA.RAW",ItemImg+10);
        error+=!LoadImage("IMAGES\\PHOTO.RAW",ItemImg+11);
        error+=!LoadImage("IMAGES\\GRENADE.RAW",ItemImg+12);
        error+=!LoadImage("IMAGES\\REDMER.RAW",ItemImg+13);
        error+=!LoadImage("IMAGES\\LASER.RAW",ItemImg+14);
        error+=!LoadImage("IMAGES\\LASERA.RAW",ItemImg+15);
        error+=!LoadImage("IMAGES\\PACK.RAW",ItemImg+16);

        for(Si16 i=0;i<4;i++) {
            char tmpstr[32];
            sprintf(tmpstr,"IMAGES\\GRFRM%hd.RAW",i);
            error+=!LoadImage(tmpstr,GrImg+i);
        }
        for(Si16 i=0;i<8;i++) {
            char tmpstr[32];
            sprintf(tmpstr,"IMAGES\\RDFRM%hd.RAW",i);
            error+=!LoadImage(tmpstr,RdImg+i);
            sprintf(tmpstr,"IMAGES\\RKFRM%hd.RAW",i);
            error+=!LoadImage(tmpstr,RkImg+i);
            sprintf(tmpstr,"IMAGES\\CORPSE%hd.RAW",i);
            error+=!LoadImage(tmpstr,CrpsImg+i);
        }

        error+=!InitRandom(); srand(time(0));

        if(error) ExitGame("Disk Loading or Memory Error(s)");
        Start();
        if(TryExit) {KeyDown(kKeyEscape);continue;}

        SetColor(0);ClearScreen();
        ShowScreen();
        g_palette.Load("2DQUAKE.PAL");
        g_palette.Set();
        SetTextJust(2,0);

        /* Nuclon */
        while(1)
        {
            /*** Controller ***/
            /* Key Controller */
            if(KF(kKeyEscape)) {
				TryExit=1;
				KeyDown(kKeyEscape);
			}
			if(KF(kKeyF8)) {
				KeyDown(kKeyF8);
				NoMessages = !NoMessages;
			}
			if (KF(kKeyF9)) {
				KeyDown(kKeyF9);
				vp[0].showBotPoints = (vp[0].showBotPoints + 1) % 3;
				vp[1].showBotPoints = (vp[1].showBotPoints + 1) % 3;
			}

            Controller1(plr+Control1);
            if(Control2!=-1) Controller2(plr+Control2);

            /* Bot Controller */
			for(Si16 i=0;i<BotsIn;i++) {
				bot[i].Handler(bot+i);
			}

            /* Communication & Exit & Win & Lose */
            //if(NetStatus) SendCmnd();
            //else
			if(TryExit) {
                char tmpstr[32];
				int x1 = screen::cx - 100;
				int x2 = screen::cx + 100;
				int y1 = screen::cy - 70;
				int y2 = screen::cy + 70;
				for (Si16 z = 69; z >= 0; z -= 3) {
					SetColor(6);
					FillRect(x1 + z, y1 + z, x2 - z, y2 - z);
					for (Si16 s = 0; s < 9; s++) {
						SetColor(15 - s);
						FrameRect(x1 + s + z, y1 + s + z, x2 - s - z, y2 - s - z);
					}
					ShowScreen();
				}
                SetTextJust(1,1);
                SetColor(120);
                sprintf(tmpstr,"Время Игры %02hd:%02hd:%02hd"
                        ,Si16(GameTime/3600),Si16((GameTime/60)%60),Si16(GameTime%60));
				if (cfg.FragLimit) {
					OutText(screen::cx, screen::cy - 50, tmpstr, &g_font);
					sprintf(tmpstr, "Предел Фрагов %6hd", cfg.FragLimit);
					OutText(screen::cx, screen::cy - 30, tmpstr, &g_font);
				}
				else {
					OutText(screen::cx, screen::cy - 40, tmpstr, &g_font);
				}

                sprintf(tmpstr, "FPS Limit  %9hd",cfg.MaxFps);
                OutText(screen::cx, screen::cy - 10,tmpstr,&g_font);
                SetColor(127);
                OutText(screen::cx, screen::cy + 10, "Выйти в Главное Меню",&g_font);
                SetColor(127);
                OutText(screen::cx, screen::cy + 30, "Выйти из 2D Quake",&g_font);
                OutText(screen::cx, screen::cy + 50, "Продолжить",&g_font);
                ShowScreen();
                Si16 cur=0;
                while(1) {
                    SetColor(6); FillRect(screen::cx, screen::cy - 20, screen::cx + 85, screen::cy);
                    if(cur==0) SetColor(79);
                    else SetColor(127);
                    sprintf(tmpstr, "FPS Limit  %9hd",cfg.MaxFps);
                    OutText(screen::cx, screen::cy - 10,tmpstr,&g_font);
                    if(cur==1) SetColor(79);
                    else SetColor(127);
                    OutText(screen::cx, screen::cy+10, "Выйти в Главное Меню",&g_font);
                    if(cur==2) SetColor(79);
                    else SetColor(127);
                    OutText(screen::cx, screen::cy+30, "Выйти из 2D Quake",&g_font);
                    if(cur==3) SetColor(79);
                    else SetColor(127);
                    OutText(screen::cx, screen::cy+50, "Продолжить",&g_font);
                    ShowScreen();

                    if(KF(kKeyEscape)) {KeyDown(kKeyEscape);break;}
                    if(KF(kKeyEnter)) {KeyDown(kKeyEnter);
                        Si16 a=0;
                        switch(cur) {
                        case 1: a=1; cur=-1; break;
                        case 2: ExitGame("",1); break;
                        case 3: a=1; break;
                        }
                        if(a) break;
                    }
                    if(KF(kKeyUp)) {KeyDown(kKeyUp);cur=(cur+3)%4;}
                    if(KF(kKeyDown)) {KeyDown(kKeyDown);cur=(cur+1)%4;}
                    if(cur==0 && KF(kKeyLeft)) {if(cfg.MaxFps>0) cfg.MaxFps--; KeyDown(kKeyLeft);}
                    if(cur==0 && KF(kKeyRight)) {cfg.MaxFps++; KeyDown(kKeyRight);}
                }
                if(cur==-1) {ExitGame("",0); break;}
                TryExit=0;DrawStatistics=1;
            }

            if(EndGame)
            {
                switch(EndGame) {
                case EG_TIMELIMIT:
                    vp[0].MakeMessage("Время Вышло");
                    if(Control2!=-1) vp[1].MakeMessage("Время Вышло");
                    break;
                case EG_FRAGLIMIT:
                    vp[0].MakeMessage("Предел Фрагов");
                    if(Control2!=-1) vp[1].MakeMessage("Предел Фрагов");
                    break;
                case EG_SYSPAUSE:
					while (!(KF(kKeyEscape)) && !(KF(kKeyEnter))) {
						ShowScreen();
					}
                    KeyDown(kKeyEscape);KeyDown(kKeyEnter);
                    ExitGame("",0);
                    break;
                }
                if(EndGame==EG_SYSPAUSE) break;
                vp[0].ClearMessages();
                DrawWeapon[vp[0].ActPlayer]=SHOW_WPN+1;
                if(Control2!=-1) {
                    vp[0].ClearMessages();
                    DrawWeapon[vp[1].ActPlayer]=SHOW_WPN+1;
                }
                EndGame=EG_SYSPAUSE;
            }

            /* Event Handler Part 1*/
            BltFst=0;
            {for(Si16 i=0;i<Players;i++) {plrevn[i].Length=0; if(plr[i].State) plr[i].MakeCmnd(plr+i);}}
            {for(Si16 i=0;i<Players;i++) if(plr[i].State>0) plr[i].Move(plr+i);}
            if(CheckBltExt) {
                Si16 newbltext=0;
                for(Si16 i=0;i<BltExt;i++)
                    if(blt[i].age) {
                        blt[i].Handler(blt+i);
                        newbltext=i+1;
                    }
                BltExt=newbltext;
                CheckBltExt=0;
            }
            else {
                for(Si16 i=0;i<BltExt;i++)
                    if(blt[i].age)
                        blt[i].Handler(blt+i);
            }
            MapHandler();

            /* Event Handler Part 2*/
			for (Si16 i = 0; i < Players; i++) {
				plr[i].damageLast = plr[i].damage; // for dmg graphical filter
			}
            if(CheckFrag) {Damage();CheckFrag=0;DrawStatistics=1;}
            {for(Si16 i=0;i<Players;i++) if(plr[i].State) plr[i].ResetCmnd();}

            /* Output */
            if(!vp[0].NoFocusing) vp[0].Focusing(vp);
            vp[0].Show();
            if(Control2!=-1) {
                if(!vp[1].NoFocusing) vp[1].Focusing(vp+1);
                vp[1].Show();
            }

            if(DrawStatistics) {DrawStatistics=0;}

            ShowScreen();

            /* Timing */
			static double currentTime = 0.0;
			double frameTime = 1.0 / cfg.MaxFps;
			double now = arctic::easy::Time();
			double nextFrameTime = currentTime + frameTime;
			arctic::easy::Sleep(nextFrameTime - now);
			currentTime = std::max(nextFrameTime, now);

            SecondCounter++;
            if(SecondCounter>=N_NLPS) {
                CheckBltExt=1;
                SecondCounter=0;
                GameTime++;
                if(cfg.TimeLimit)
                {
                    char tmpstr[32];
                    tmpstr[0]=0;
                    if(GameTime==cfg.TimeLimit*60) EndGame=EG_TIMELIMIT;
                    else if(GameTime>cfg.TimeLimit*60-10)
                        sprintf(tmpstr,"%hd",(Si16)(cfg.TimeLimit*60-GameTime));
                    else if(GameTime>(cfg.TimeLimit-1)*60 && !((cfg.TimeLimit*60-GameTime)%10))
                        sprintf(tmpstr,"%hd Seconds",(Si16)(cfg.TimeLimit*60-GameTime));
                    else if(GameTime==(cfg.TimeLimit-1)*60)
                        sprintf(tmpstr,"1 Minute");
                    else if(!(GameTime%60)) {
                        if(GameTime/60==cfg.TimeLimit-5)
                            sprintf(tmpstr,"5 Minutes");
                        else if(GameTime/60>=cfg.TimeLimit-4)
                            sprintf(tmpstr,"%hd Minutes",(Si16)(cfg.TimeLimit-GameTime/60));
                        else if(!((cfg.TimeLimit-GameTime/60)%10))
                            sprintf(tmpstr,"%hd Minutes",(Si16)(cfg.TimeLimit-GameTime/60));
                    }
                    if(tmpstr[0]) {
                        vp[0].MakeMessage(tmpstr,MSG_TIME);
                        if(Control2!=-1) vp[1].MakeMessage(tmpstr,MSG_TIME);
                    }
                }
            }
        }
    }
}
