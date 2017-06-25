/*
2D Quake v1.06
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/
#include "defs.h"

#include <math.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <random>

#include "defs.h"
#include "graph.h"
#include "2dqdefs.h"
#include "game.h"

Si16 Points;
POINT *pnt;
ITEMPOINT *itmpnt;
BOT *bot;
BYTE _BotInit;
BYTE _BotAmount;

/* Bot Actions */
void SelectPointABattle(BOT *bt);
void SNP1(BOT *bt);
void SNPRandom(BOT *bt);
void SelectWeapon(BOT *bt);
void Search(BOT *bt);
void RestartBot(BOT *bt);
void StartFarBattle(BOT *bt,BYTE Target);
BYTE NormalAvoid(BOT *bt,Si16 n);
BYTE GrenadeAvoid(BOT *bt,Si16 n);
void NormalizeAvoidEvents(BOT *bt,Si16 *BltIndex,Si16 MaxBlt);
BYTE MakeDistance(BOT *bt,Si16 x1,Si16 y1,Si16 Distance,Si16 Mode=BDM_EQ);
void GetDistance(BOT *bt,Si16 x1,Si16 y1);
void WalkTo(BOT *bt,Si16 x2,Si16 y2);
BYTE LookTo(BOT *bt,Si16 x2,Si16 y2,Si16 Fov=5);
void FindNextT(BOT *bt);
void ChaosWalk(BOT *bt);
/* Bot Brains */
void NormalWalk(BOT *bt);
void FarBattle(BOT *bt);
void SearchWalk(BOT *bt);
void MeatWalk(BOT *bt);
void MeatBattle(BOT *bt);
void MeatSearchWalk(BOT *bt);
/* Bot System Functions */
void InitBots(BYTE BotAmount,BYTE FirstBot,BYTE MapNumber);
void CloseBots(void);

void ShowBotPoints(VIEWPORT* vp, bool showIds)
{
	SetColor(120);
	for (Si16 i = 0; i < Points; i++) {
		FillRect(pnt[i].x - vp->x - 1, pnt[i].y - vp->y - 1, pnt[i].x - vp->x + 1, pnt[i].y - vp->y + 1);
		for (Si16 j = 0; j < pnt[i].NPnts; j++) {
			Line(pnt[i].x - vp->x, pnt[i].y - vp->y,
				pnt[pnt[i].NPnt[j]].x - vp->x, pnt[pnt[i].NPnt[j]].y - vp->y);
		}
	}
    if (showIds) {
		SetColor(125);
		for (Si16 i = 0; i < Points; i++) {
			Si16 xx = pnt[i].x - vp->x + vp->scr_x1;
			Si16 yy = pnt[i].y - vp->y + vp->scr_y1;
			if (xx >= vp->scr_x1 && yy >= vp->scr_y1 && xx <= vp->scr_x2 && yy + 4 <= vp->scr_y2) {
				char text[10];
				_itoa(i, text, 10);
				Si16 ss = (Si16)strlen(text) * 4;
				if (xx - ss - 1 < vp->scr_x1) xx = vp->scr_x1 + ss + 1;
				else if (xx + ss > vp->scr_x2) xx = vp->scr_x2 - ss;
				if (yy - 5 < vp->scr_y1) yy = vp->scr_y1 + 5;
				else if (yy + 4 > vp->scr_y2) yy = vp->scr_y2 - 4;

				SetTextJust(1, ((pnt[i].x - 10) / 20 & 1)? 2: 0);
				OutText(xx, yy, text, &g_font);
			}
		}
    }

	//if (ActPnt >= 0 && ActPnt < Points) {
	//	SetColor(47);
	//	FillRect(pnt[ActPnt].x - vp->x - 1, pnt[ActPnt].y - vp->y - 1,
	//		pnt[ActPnt].x - vp->x + 1, pnt[ActPnt].y - vp->y + 1);
	//	for (Si16 j = 0; j < pnt[ActPnt].NPnts; j++) {
	//		Line(pnt[ActPnt].x - vp->x, pnt[ActPnt].y - vp->y,
	//			pnt[pnt[ActPnt].NPnt[j]].x - vp->x, pnt[pnt[ActPnt].NPnt[j]].y - vp->y);
	//	}
	//	//if (ActLnk >= 0 && ActLnk < pnt[ActPnt].NPnts) {
	//	//	SetColor(79);
	//	//	Line(pnt[ActPnt].x - x, pnt[ActPnt].y - y,
	//	//		pnt[pnt[ActPnt].NPnt[ActLnk]].x - x,
	//	//		pnt[pnt[ActPnt].NPnt[ActLnk]].y - y);
	//	//}
	//}
}

void GraphAddPoint(Si16 idx, double xx, double yy, Si16 /*NPnts*/)
{
	//#define Graf(ii,xx,yy,pp)     pnt[ii].x=(xx)*20+10;pnt[ii].y=(yy)*20+10;pnt[ii].NPnts=pp
	POINT* p = &pnt[idx];
	p->x = xx * 20 + 10;
	p->y = yy * 20 + 10;
	pnt[idx].NPnts = 0;
}

void GraphAddNeighbour(Si16 idx, Si16 neighbourIdx, double weight)
{
	POINT* p = &pnt[idx];
	for (Si16 i = 0; i < p->NPnts; i++) {
		if (p->NPnt[i] == neighbourIdx) {
			return; // already added
		}
	}
	Si16 i = p->NPnts++;
	if (i >= sizeof(p->NPnt) / sizeof(*p->NPnt)) {
		abort(); // too many neighbours in graph
	}
	p->NPnt[i] = neighbourIdx;
	p->Weight[i] = weight;
}

void GraphAddEdge(Si16 srcIdx, Si16 /*edgeIdx*/, Si16 dstIdx)
{
	//#define Gpnt(ii,pp,tt)        pnt[ii].NPnt[pp]=tt; 
    // pnt[ii].Weight[pp]=sqrt((long)(pnt[ii].x-pnt[tt].x)*(pnt[ii].x-pnt[tt].x)+
	// (long)(pnt[ii].y-pnt[tt].y)*(pnt[ii].y-pnt[tt].y))
	double weight = sqrt(
		(long)(pnt[srcIdx].x - pnt[dstIdx].x)*(pnt[srcIdx].x - pnt[dstIdx].x) +
		(long)(pnt[srcIdx].y - pnt[dstIdx].y)*(pnt[srcIdx].y - pnt[dstIdx].y)
	);

	GraphAddNeighbour(srcIdx, dstIdx, weight);
	GraphAddNeighbour(dstIdx, srcIdx, weight);
}

void InitBots(BYTE BotAmount,BYTE FirstBot,BYTE MapNumber)
{
    _BotAmount=BotAmount;
    if((bot = (BOT *) malloc(BotAmount*sizeof(BOT))) == NULL)
        ExitGame("InitBots: No Memory for Create Bots");
    if((itmpnt = (ITEMPOINT *) malloc(_ItemAmount*sizeof(ITEMPOINT))) == NULL)
        ExitGame("InitBots: No Memory for Create ITEMPOINTS");
	memset(itmpnt, 0, _ItemAmount * sizeof(ITEMPOINT));

    FILE *src;
    Si16 LinkAmount;
    if((src=fopen(BotFileName,"rt"))==NULL)
        ExitGame("InitBots: Can't Open Bot File");
    fscanf(src," %hd",&Points);
    if((pnt = (POINT *) malloc(Points*sizeof(POINT))) == NULL)
        ExitGame("InitBots: No Memory for Create Points for Bots");
    for(Si16 i=0;i<Points;i++) {
		double ai,ax,ay,ap;
        fscanf(src," Graf( %lf , %lf , %lf , %lf ) ;",&ai,&ax,&ay,&ap);
		GraphAddPoint(ai, ax, ay, ap);
        //Graf(Si16(ai),ax,ay,ap);
    }
    fscanf(src," %hd",&LinkAmount);
    for(Si16 i=0;i<LinkAmount;i++) {
        double ai,ap,at;
        fscanf(src," Gpnt( %lf , %lf , %lf ) ;",&ai,&ap,&at);
		GraphAddEdge(ai, ap, at);
        //Gpnt(Si16(ai),Si16(ap),Si16(at));
    }
    fclose(src);
    // Make ITEMPOINTs //
    for(Si16 i=0;i<_ItemAmount;i++) {
        for(Si16 j=0;j<Points;j++) {
            if(ABS(blt[i].x-pnt[j].x)<3 && ABS(blt[i].y-pnt[j].y)<3) {
				itmpnt[i].Initialized=1;
                itmpnt[i].Pnt=j;
                itmpnt[i].ItmNum=i;
                itmpnt[i].Item=blt+i;

                if(blt[i].Handler==I_ArmourHandler) itmpnt[i].Type=IT_ARMOUR;
                else if(blt[i].Handler==I_HealthHandler) itmpnt[i].Type=IT_HEALTH;
                else if(blt[i].Handler==I_BowHandler) itmpnt[i].Type=IT_BOW;
                else if(blt[i].Handler==I_AkmHandler) itmpnt[i].Type=IT_AKM;
                else if(blt[i].Handler==I_NapalmHandler) itmpnt[i].Type=IT_NAPALM;
                else if(blt[i].Handler==I_LauncherHandler) itmpnt[i].Type=IT_LAUNCHER;
                else if(blt[i].Handler==I_LaserHandler) itmpnt[i].Type=IT_LASER;
                else if(blt[i].Handler==I_RedmerHandler) itmpnt[i].Type=IT_REDMER;
                else if(blt[i].Handler==I_PhotoHandler) itmpnt[i].Type=IT_PHOTO;
                else if(blt[i].Handler==I_GrenadeHandler) itmpnt[i].Type=IT_GRENADE;
                else if(blt[i].Handler==I_PistolAmmoHandler) itmpnt[i].Type=IT_PISTOLA;
                else if(blt[i].Handler==I_BowAmmoHandler) itmpnt[i].Type=IT_BOWA;
                else if(blt[i].Handler==I_AkmAmmoHandler) itmpnt[i].Type=IT_AKMA;
                else if(blt[i].Handler==I_NapalmAmmoHandler) itmpnt[i].Type=IT_NAPALMA;
                else if(blt[i].Handler==I_LauncherAmmoHandler) itmpnt[i].Type=IT_LAUNCHERA;
                else if(blt[i].Handler==I_LaserAmmoHandler) itmpnt[i].Type=IT_LASERA;
                else itmpnt[i].Type=IT_UNKNOWN;
            }
        }
    }
    // Make BOTs //
    if(cfg.BotChs == BC_BYNUM) {
        for(Si16 i=0;i<BotAmount;i++) {
            bot[i].Own=plr+(i+FirstBot);
            bot[i].OwnNum=i+FirstBot;
            plr[FirstBot+i].Control=PC_BOT;
            strcpy(plr[FirstBot+i].Name,cfg.BotName[FirstBot+i]);
            bot[i].Skill=BS_MEAT;//cfg.BotSkill[FirstBot+i];
            bot[i].LikeWpn=cfg.BotWpn[FirstBot+i];
            bot[i].Taktic=cfg.BotTaktic[FirstBot+i];
            if((bot[i].ItmMem = (Si16 *) malloc(_ItemAmount*sizeof(Si16))) == NULL)
                ExitGame("InitBots: No Memory for Create ITEMPOINTS");
            memset(bot[i].ItmMem,0,_ItemAmount * sizeof(Si16));
            RestartBot(bot+i);
        }
    }
    else {
        BYTE used[M_PLR];
        memset(used,0,M_PLR);
        for(Si16 i=0;i<BotAmount;i++) {
            Si16 cfgi;
            while(1) {
                cfgi=random(M_PLR);
                if(used[cfgi]==0) {used[cfgi]=1;break;}
            }
            bot[i].Own=plr+(i+FirstBot);
            bot[i].OwnNum=i+FirstBot;
            plr[FirstBot+i].Control=PC_BOT;
            strcpy(plr[FirstBot+i].Name,cfg.BotName[cfgi]);
            bot[i].Skill=BS_MEAT;//cfg.BotSkill[cfgi];
            bot[i].LikeWpn=cfg.BotWpn[cfgi];
            bot[i].Taktic=cfg.BotTaktic[cfgi];
            if((bot[i].ItmMem = (Si16 *) malloc(_ItemAmount*sizeof(Si16))) == NULL)
                ExitGame("InitBots: No Memory for Create ITEMPOINTS");
            memset(bot[i].ItmMem,0,_ItemAmount*sizeof(Si16));
            RestartBot(bot+i);
        }
    }
    _BotInit=1;
}

void CloseBots(void)
{
    if(!_BotInit) return;
    free(pnt);
    free(itmpnt);
    for(Si16 i=0;i<_BotAmount;i++) free(bot[i].ItmMem);
	free(bot);
	_BotInit=0;
}

#define pl        (bt->Own)
#define event     plrevn[bt->OwnNum]
#define Make(a)   bt->Own->cmnd[a]=1

/***************************************************************************/
/*******************************  ACTIONS  *********************************/
/***************************************************************************/

void NormalizeAvoidEvents(BOT *bt,Si16 *BltIndex,Si16 MaxBlt)
{
    Si16 AvCode=0;
    while(bt->AvoidCode[AvCode]!=AE_END)
    {
        if(bt->AvoidCode[AvCode]==AE_NEXT) {
            AvCode+=3;
            continue;
        }
        Si16 checkindex=bt->AvoidCode[AvCode];
		Si16 i;
        for(i=0;i<MaxBlt;i++)
        {
            if(BltIndex[i]==checkindex) break;
        }
        if(i==MaxBlt) bt->AvoidCode[AvCode]=AE_NEXT;
        AvCode+=3;
    }
    AvCode-=3;
    while(bt->AvoidCode[AvCode]==AE_NEXT)
    {
        bt->AvoidCode[AvCode]=AE_END;
        if(!AvCode) break;
        AvCode-=3;
    }
}

BYTE NormalAvoid(BOT *bt,Si16 n)
{
    Si16 x1,y1,x2,y2,sign,AvCode=0;
    BYTE NeedAvoid=0;
    double a1,angle,angle1,angle2,angle3,angle4;

    if(blt[n].own==bt->OwnNum)
        return 0;
    while(bt->AvoidCode[AvCode]!=AE_END)
    {
        if(bt->AvoidCode[AvCode]==AE_NEXT) {
            AvCode+=3;
            continue;
        }
        if(bt->AvoidCode[AvCode]==n) {
            if(pl->x>=bt->AvoidCode[AvCode+1]-P_SZ && pl->y>=bt->AvoidCode[AvCode+2]-P_SZ &&
                    pl->x<=bt->AvoidCode[AvCode+1]+P_SZ && pl->y<=bt->AvoidCode[AvCode+2]+P_SZ) {
                bt->AvoidCode[AvCode]=AE_NEXT;
                return 0;
            }
            WalkTo(bt,bt->AvoidCode[AvCode+1]
                    ,bt->AvoidCode[AvCode+2]);
            return 1;
        }
        AvCode+=3;
    }

    x1=blt[n].x;y1=blt[n].y;
    /* Calculate A1 */
    if(blt[n].sx==0) {
        if(blt[n].sy<0) a1=M_PI/2;
        else a1=M_PI/2+M_PI;
    }
    else {a1=atan2(-blt[n].sy,blt[n].sx);}
    if(a1<0) a1+=2*M_PI;
    /* Check 1 */
    x2=pl->x+P_SZ;y2=pl->y+P_SZ;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;
    angle=angle-a1;
    if(angle>M_PI) angle-=2*M_PI;
    if(angle<-M_PI) angle+=2*M_PI;
    sign=sgn(angle);
    angle1=angle;
    /* Check 2 */
    x2=pl->x+P_SZ;y2=pl->y-P_SZ;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;
    angle=angle-a1;
    if(angle>M_PI) angle-=2*M_PI;
    if(angle<-M_PI) angle+=2*M_PI;
    if(sign!=sgn(angle)) NeedAvoid=1;
    angle2=angle;
    /* Check 3 */
    x2=pl->x-P_SZ;y2=pl->y+P_SZ;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;
    angle=angle-a1;
    if(angle>M_PI) angle-=2*M_PI;
    if(angle<-M_PI) angle+=2*M_PI;
    if(sign!=sgn(angle)) NeedAvoid=1;
    angle3=angle;
    /* Check 4 */
    x2=pl->x-P_SZ;y2=pl->y-P_SZ;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;
    angle=angle-a1;
    if(angle>M_PI) angle-=2*M_PI;
    if(angle<-M_PI) angle+=2*M_PI;
    if(sign!=sgn(angle)) NeedAvoid=1;
    angle4=angle;

    if(NeedAvoid && ABS(angle1)<M_PI/2
            && ABS(angle2)<M_PI/2
            && ABS(angle3)<M_PI/2
            && ABS(angle4)<M_PI/2)
    {
        Si16 tx1,tx2,ty1,ty2,tx,ty,t;
        if(Si16(blt[n].sx*100)) {
            tx1=(pl->x-blt[n].x-P_SZ)/blt[n].sx;
            tx2=(pl->x-blt[n].x+P_SZ)/blt[n].sx;
            tx=std::min(tx1,tx2)+2;
        }
        else {tx=0;}
        if(Si16(blt[n].sy*100)) {
            ty1=(pl->y-blt[n].y-P_SZ)/blt[n].sy;
            ty2=(pl->y-blt[n].y+P_SZ)/blt[n].sy;
            ty=std::min(ty1,ty2)+2;
        }
        else {ty=0;}
        t=std::max(tx,ty);
        double bx=blt[n].x+blt[n].sx*t;
        double by=blt[n].y+blt[n].sy*t;
        t=random(2)*2-1;
        a1+=t*M_PI/2;
        if(Border(pl->x,pl->y,bx+cos(a1)*16,by-sin(a1)*16));
        else a1-=t*M_PI;

        Si16 tox=bx+cos(a1)*16,toy=by-sin(a1)*16;

        bt->AvoidCode[AvCode  ]=n;
        bt->AvoidCode[AvCode+1]=tox;
        bt->AvoidCode[AvCode+2]=toy;
        bt->AvoidCode[AvCode+3]=AE_END;
        WalkTo(bt,tox,toy);
        return 1;
    }

    return 0;
}
BYTE GrenadeAvoid(BOT *bt,Si16 n)
{
    Si16 AvCode=0;
    if(blt[n].own==bt->OwnNum)
        return 0;
    while(bt->AvoidCode[AvCode]!=AE_END)
    {
        if(bt->AvoidCode[AvCode]==AE_NEXT) {
            AvCode+=3;
            continue;
        }
        if(bt->AvoidCode[AvCode]==n) {
            if(ABS(pl->x-blt[n].x)+ABS(pl->y-blt[n].y)>RNGF_GRENADE+10) {
                //   Si16(blt[n].sx*50) || Si16(blt[n].sy*50)) {
                bt->AvoidCode[AvCode+1]++;
                return 0;
            }
            bt->AvoidCode[AvCode+1]++;
            return MakeDistance(bt,blt[n].x,blt[n].y,RNGF_GRENADE+10,BDM_HI);
        }
        AvCode+=3;
    }

    bt->AvoidCode[AvCode  ]=n;
    bt->AvoidCode[AvCode+1]=0;
    bt->AvoidCode[AvCode+2]=0;
    bt->AvoidCode[AvCode+3]=AE_END;
    return 0;
}
void StartFarBattle(BOT *bt,BYTE Target)
{
    bt->Target=Target;
    switch(bt->Skill) {
    case BS_MEAT: bt->Handler=MeatBattle; bt->AimSize=40; bt->AimFov=2;
        bt->Fire=0;
        break;
    default: bt->Handler=FarBattle;
    }
    bt->Distance=sqrt(double(plr[Target].x-pl->x)*(plr[Target].x-pl->x)
                      +double(plr[Target].y-pl->y)*(plr[Target].y-pl->y));
}

void FindNextT(BOT *bt)
{
    BYTE *Field;
    BYTE *Field2;
    Si16 FSize=Map.Width*Map.Height;
    Si16 W=Map.Width;

    Si16 From=Si16(pl->x)/20+Si16(pl->y)/20*W;
    Si16 To=Si16(bt->rtx)/20+Si16(bt->rty)/20*W;
    Si16 CheckN=-1;

    if((Field=(BYTE *)malloc(FSize)) == NULL)
        ExitGame("FindNextT: Out of Memory");
    if((Field2=(BYTE *)malloc(FSize)) == NULL)
        ExitGame("FindNextT: Out of Memory");
    memset(Field,0xFF,FSize);

    Field[From]=0x00;Field[From+1]=0xFE;Field[From-1]=0xFE;
    Field[From+W]=0xFE;Field[From-W]=0xFE;

    memcpy(Field2,Field,FSize);

#ifdef _ATST
    FILE *stm;
    char tmpstr[30];
    ffblk ffb;
    Si16 done,filen=0;
    sprintf(tmpstr,"A\\Z%05d.CPP",filen);
    done = findfirst(tmpstr,&ffb,0);
    while (!done)
    {
        filen++;
        sprintf(tmpstr,"A\\Z%05d.CPP",filen);
        done = findfirst(tmpstr,&ffb,0);
    }

    stm = fopen(tmpstr, "wt");
    fprintf(stm,"To=%hd From=%hd",To,From);
#endif

    for(Si16 j=0;j<100;j++) {
        BYTE *p1=Field+1+W;
        BYTE *p2=Field2+1+W;
        for(Si16 i=1+W;i<FSize-W;i++) {
            if(*p2==0xFE && FF_PlrMov((i%W)*20+10,(i/W)*20+10)) {
                *p1=*(p2+1);
                if(*p1>*(p2-1)) *p1=*(p2-1);
                if(*p1>*(p2+W)) *p1=*(p2+W);
                if(*p1>*(p2-W)) *p1=*(p2-W);
                (*p1)++;

                if(*(p2+1)==0xFF) *(p1+1)=0xFE;
                if(*(p2-1)==0xFF) *(p1-1)=0xFE;
                if(*(p2+W)==0xFF) *(p1+W)=0xFE;
                if(*(p2-W)==0xFF) *(p1-W)=0xFE;
            }
            p1++;p2++;
#ifdef _ATST
            fprintf(stm,"j=%hd i=%hd\n",j,i);
            if(j==7 && i==1121) {
                fprintf(stm,"Fuck1\n");
            }
#endif
        }
        memcpy(Field2,Field,FSize);
        if(Field[To]<0xFE) {
            CheckN=To;break;
        }
    }
    if(CheckN==-1) {
        switch(bt->Skill) {
        case BS_MEAT: bt->Handler=MeatWalk; break;
        default: bt->Handler=NormalWalk;
        }
        free(Field);
        free(Field2);
        return;
    }

#ifdef _ATST
    fclose(stm);
#endif

    while(1) {
        Si16 Next=CheckN;
        if(Field[Next]>Field[CheckN+1] && Field[CheckN]>Field[CheckN+1])
            Next=CheckN+1;
        if(Field[Next]>Field[CheckN-1] && Field[CheckN]>Field[CheckN-1])
            Next=CheckN-1;
        if(Field[Next]>Field[CheckN+W] && Field[CheckN]>Field[CheckN+W])
            Next=CheckN+W;
        if(Field[Next]>Field[CheckN-W] && Field[CheckN]>Field[CheckN-W])
            Next=CheckN-W;

        CheckN=Next;
        if(Border(pl->x,pl->y,(CheckN%W)*20+10,(CheckN/W)*20+10)
                || (Next==CheckN && CheckN==From)) {
            bt->tx=(CheckN%W)*20+10;
            bt->ty=(CheckN/W)*20+10;
            break;
        }
    }

#ifdef _TEST
    FILE *stm;
    char tmpstr[30];
    ffblk ffb;
    Si16 done,filen=0;
    sprintf(tmpstr,"A\\F%05d.CPP",filen);
    done = findfirst(tmpstr,&ffb,0);
    while (!done)
    {
        filen++;
        sprintf(tmpstr,"A\\F%05d.CPP",filen);
        done = findfirst(tmpstr,&ffb,0);
    }

    stm = fopen(tmpstr, "wt");

    for(Si16 iy=0;iy<Map.Height;iy++) {
        for(Si16 ix=0;ix<W;ix++) {
            if(!FF_PlrMov((ix)*20+10,(iy)*20+10))
                fprintf(stm,"XX");
            else {
                if(Field[iy*W+ix]>0xFA)
                    fprintf(stm,"  ");
                else {
                    if(iy*W+ix==CheckN)
                        fprintf(stm,"++",Si16(Field[iy*W+ix]));
                    else if(iy*W+ix==To)
                        fprintf(stm,"**",Si16(Field[iy*W+ix]));
                    else if(iy*W+ix==From)
                        fprintf(stm,"[]",Si16(Field[iy*W+ix]));
                    else
                        fprintf(stm,"%2X",Si16(Field[iy*W+ix]));
                }
            }
        }
        fprintf(stm,"\n");
    }
    fprintf(stm,"tx=%hd(%hd)\nty=%hd(%hd)\nrtx=%hd(%hd)\nrty=%hd(%hd)\nx=%hd(%hd)\ny=%hd(%hd)",
            bt->tx,Si16(bt->tx)/20,
            bt->ty,Si16(bt->ty)/20,
            bt->rtx,Si16(bt->rtx)/20,
            bt->rty,Si16(bt->rty)/20,
            pl->x,Si16(pl->x)/20,
            pl->y,Si16(pl->y)/20);

    fclose(stm);
#endif

    free(Field);
    free(Field2);
}

void RestartBot(BOT *bt)
{
    bt->AvoidCode[0]=AE_END;
    bt->AvoidCode[1]=AE_END;
    switch(bt->Skill) {
    case BS_MEAT:
        bt->Handler=MeatWalk;
        bt->cnt1=bt->cnt2=bt->cnt3=bt->TurnAround=0;
        break;
    default: bt->Handler=NormalWalk;
    }
    bt->Target=0;
    bt->NoAmmos=0;
    bt->tx=0;
    bt->ty=0;
    // Signal to Select
    bt->NPntNum=-1;
	//   SelectPointABattle(bt);
    SelectWeapon(bt);
}
void GetDistance(BOT *bt,Si16 x1,Si16 y1)
{bt->Distance=sqrt((long)(pl->x-x1)*(pl->x-x1)+(long)(pl->y-y1)*(pl->y-y1));}

BYTE MakeDistance(BOT *bt,Si16 x1,Si16 y1,Si16 Distance,Si16 Mode)
/* Mode: BDM_EQ - Make == Distance
            BDM_HI - Make  > Distance
            BDM_LO - Make  < Distance */
{
    Si16 x2,y2;
    double angle,angleoff=0;

    /* Check 1 */
    x2=pl->x;y2=pl->y;
    Si16 RealDist=sqrt((long)(x2-x1)*(x2-x1)+(long)(y2-y1)*(y2-y1));
    if(Mode==BDM_EQ && ABS(RealDist-Distance)< P_SZ) return 0;
    if(Mode==BDM_HI &&     RealDist-Distance > P_SZ) return 0;
    if(Mode==BDM_LO &&     RealDist-Distance <-P_SZ) return 0;
    /* Check 2 */
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;

    while(angleoff<M_PI) {
        if(!Border(x2,y2,x1+Distance*cos(angle+angleoff),y1-Distance*sin(angle+angleoff)))
            angleoff=-(ABS(angleoff)+M_PI/30)*(angleoff>0?1:-1);
        else break;
    }
    if(angleoff>=M_PI) return 0;

    WalkTo(bt,x1+Distance*cos(angle+angleoff),y1-Distance*sin(angle+angleoff));
    return 1;
}
void WalkTo(BOT *bt,Si16 x2,Si16 y2)
{
    Si16 x1=pl->x,y1=pl->y;
    double a1=pl->a,angle;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;
    double z=angle-a1;
    if(z<0) z+=2*M_PI;

    if(z<=M_PI/8*1 || z>2*M_PI-M_PI/8) Make(C_ACC);
    else if(z<=M_PI/8* 3 && z>M_PI/8   ) {Make(C_ACC); Make(C_LFT);}
    else if(z<=M_PI/8* 5 && z>M_PI/8* 3) {             Make(C_LFT);}
    else if(z<=M_PI/8* 7 && z>M_PI/8* 5) {Make(C_BRK); Make(C_LFT);}
    else if(z<=M_PI+M_PI/8 && z>M_PI-M_PI/8) Make(C_BRK);
    else if(z<=M_PI/8*11 && z>M_PI/8* 9) {Make(C_BRK); Make(C_RHT);}
    else if(z<=M_PI/8*13 && z>M_PI/8*11) {             Make(C_RHT);}
    else if(z<=M_PI/8*15 && z>M_PI/8*13) {Make(C_ACC); Make(C_RHT);}
}
BYTE LookTo(BOT *bt,Si16 x2,Si16 y2,Si16 Fov)
{
    Si16 x1=pl->x,y1=pl->y;
    double a1=pl->a,angle;

    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;
    double z=angle-a1;
    if(z<0) z+=2*M_PI;
    if(z<P_TRN2*Fov || z>2*M_PI-P_TRN2*Fov)
        return 1;
    if(z>0 && z<=M_PI) Make(C_A_P);
    else if(z<2*M_PI && z>M_PI) Make(C_A_M);
    return 0;
}
void SelectPointABattle(BOT *bt)
{
    switch(bt->Skill) {
    case BS_MEAT: bt->Handler=MeatWalk; break;
    default: bt->Handler=NormalWalk;
    }
	Si16 MinDist = 30000;
	Si16 Next = 0;
	Si16 x = pl->x, y = pl->y;
    for(Si16 i=0;i<Points;i++) {
		Si16 Dist =
			sqrt((double)(x - pnt[i].x)*(x - pnt[i].x) +
			     (double)(y - pnt[i].y)*(y - pnt[i].y));
		if (Dist > MinDist) {
			continue;
		}
        if (Border(x, y, pnt[i].x, pnt[i].y)) {
			MinDist = Dist;
			Next = i;
        }
    }
    bt->PntNum=Next;
    bt->NPntNum=Next;
}
void SNP1(BOT *bt)  // Select Next Point 1 //
{
    Si16 start=bt->NPntNum;
	for (Si16 i = 0; i < _ItemAmount; i++) {
		if (!itmpnt[i].Initialized) {
			continue;
		}
		if (start == itmpnt[i].Pnt) {
			bt->ItmMem[i] = (T_RESPAWN / _ItemAmount) + 2;
		}
	}
    Si16 srch; // Item Type for Search //
    // Analize Needs of Bot //
    if(bt->LikeWpn==M_WPN) {
		do {
			bt->LikeWpn = random(M_WPN);
		} while (bt->LikeWpn == B_PHOTO);
	}
    if(pl->hp<100) srch=BN_HP;
    else { Si16 WpnAmount=0;
        if(pl->wpn[B_ARROW]) WpnAmount++;
        if(pl->wpn[B_AKM]) WpnAmount++;
        if(pl->wpn[B_NAPALM]) WpnAmount++;
        if(pl->wpn[B_ROCKET]) WpnAmount++;
        if(pl->wpn[B_LASER]) WpnAmount++;
        if(!WpnAmount) srch=BN_ANYWPN;
        else if(!pl->wpn[bt->LikeWpn]) srch=BN_FWPN;
        else { Si16 AmmoAmount=0;
            if(pl->wpn[B_ARROW] && pl->ammo[B_ARROW]>BA_ARROW/2) AmmoAmount++;
            if(pl->wpn[B_AKM] && pl->ammo[B_AKM]>BA_AKM/2) AmmoAmount++;
            if(pl->wpn[B_NAPALM] && pl->ammo[B_NAPALM]>BA_NAPALM/2) AmmoAmount++;
            if(pl->wpn[B_ROCKET] && pl->ammo[B_ROCKET]>>BA_ROCKET/2) AmmoAmount++;
            if(pl->wpn[B_LASER] && pl->ammo[B_LASER]>BA_LASER/2) AmmoAmount++;
            if(!AmmoAmount) srch=BN_ANYAMMO;
            else if(pl->hp<150) srch=BN_HP;
            else if(pl->ammo[bt->LikeWpn]!=MaxAmmo[bt->LikeWpn] &&
                    pl->ammo[bt->LikeWpn]<2*AddAmmo[bt->LikeWpn]) srch=BN_FAMMO;
            else if(pl->armr<30) srch=BN_ARMR;
            else if(pl->ammo[bt->LikeWpn]!=MaxAmmo[bt->LikeWpn] &&
                    pl->ammo[bt->LikeWpn]<3*AddAmmo[bt->LikeWpn]) srch=BN_FAMMO;
            else if(pl->hp<180) srch=BN_HP;
            else if(pl->armr<130) srch=BN_ARMR;
            else if(pl->hp<200) srch=BN_HP;
            else if(pl->ammo[bt->LikeWpn]!=MaxAmmo[bt->LikeWpn]) srch=BN_FAMMO;
            else srch=BN_FRAG;
        }
    }
    // Prepare Search Items in Graf //
    Si16 *fld;
    BYTE *sts;
    if((fld = (Si16 *) malloc(Points*sizeof(Si16))) == NULL)
        ExitGame("SNP1: No Memory for Create Points for Search");
    if((sts = (BYTE *) malloc(Points*sizeof(BYTE))) == NULL)
        ExitGame("SNP1: No Memory for Create Points for Search");
    memset(fld,0,Points* sizeof(Si16));
    memset(sts,0,Points);
    sts[start]=1; fld[start]=1;
    BYTE Needed[M_IT];
    memset(Needed,0,M_IT);
    switch(srch) {
    case BN_HP:
#ifdef _TEST
        vp[0].MakeMessage("HP");
#endif
        Needed[IT_HEALTH]=1; break;
    case BN_ARMR:
#ifdef _TEST
        vp[0].MakeMessage("ARMR");
#endif
        Needed[IT_ARMOUR]=1; break;
    case BN_FRAG: // WARNING //
#ifdef _TEST
        vp[0].MakeMessage("FRAG");
#endif
    case BN_ANYWPN:
#ifdef _TEST
        vp[0].MakeMessage("ANYWPN");
#endif
        if(!pl->wpn[B_ARROW]) Needed[IT_BOW]=1;
        if(!pl->wpn[B_AKM]) Needed[IT_AKM]=1;
        if(!pl->wpn[B_NAPALM]) Needed[IT_NAPALM]=1;
        if(!pl->wpn[B_ROCKET]) Needed[IT_LAUNCHER]=1;
        if(!pl->wpn[B_LASER]) Needed[IT_LASER]=1;
        break;
    case BN_ANYAMMO:
#ifdef _TEST
        vp[0].MakeMessage("ANYAMMO");
#endif
        if(pl->wpn[B_ARROW]) Needed[IT_BOWA]=1;
        if(pl->wpn[B_AKM]) Needed[IT_AKMA]=1;
        if(pl->wpn[B_NAPALM]) Needed[IT_NAPALMA]=1;
        if(pl->wpn[B_ROCKET]) Needed[IT_LAUNCHERA]=1;
        if(pl->wpn[B_LASER]) Needed[IT_LASERA]=1;
        break;
    case BN_FWPN:
#ifdef _TEST
        vp[0].MakeMessage("FWPN");
#endif
        switch(bt->LikeWpn) {
        case B_SHELL: Needed[IT_PISTOLA]=1; break;
        case B_ARROW: Needed[IT_BOW]=1; break;
        case B_AKM: Needed[IT_AKM]=1; break;
        case B_NAPALM: Needed[IT_NAPALM]=1; break;
        case B_ROCKET: Needed[IT_LAUNCHER]=1; break;
        case B_LASER: Needed[IT_LASER]=1; break;
            // WARNING //
        case B_PHOTO: Needed[IT_AKM]=1; break;
        case B_GRENADE: Needed[IT_LAUNCHER]=1; break;
        case B_REDMER: Needed[IT_LASER]=1; break;
        }
        break;
    case BN_FAMMO:
#ifdef _TEST
        vp[0].MakeMessage("FAMMO");
#endif
        switch(bt->LikeWpn) {
        case B_SHELL: Needed[IT_PISTOLA]=1; break;
        case B_ARROW: Needed[IT_BOWA]=1; break;
        case B_AKM: Needed[IT_AKMA]=1; break;
        case B_NAPALM: Needed[IT_NAPALMA]=1; break;
        case B_ROCKET: Needed[IT_LAUNCHERA]=1; break;
        case B_LASER: Needed[IT_LASERA]=1; break;
            // WARNING //
        case B_PHOTO: Needed[IT_AKMA]=1; break;
        case B_GRENADE: Needed[IT_LAUNCHERA]=1; break;
        case B_REDMER: Needed[IT_LASERA]=1; break;
        }
        break;
    }
    BYTE Exist=0;
    for(Si16 i=0;i<_ItemAmount;i++) {
		if (!itmpnt[i].Initialized) {
			continue;
		}
        if (Needed[itmpnt[i].Type] && itmpnt[i].Pnt!=start && !bt->ItmMem[i]) {
            Exist=1;sts[itmpnt[i].Pnt]=2;
        }
    }
    if(Exist==0) {
#ifdef _TEST
        vp[0].MakeMessage("Exist=0 - Random");
#endif
        SNPRandom(bt);
        free(fld);
        free(sts);
        return;
    }
    // Search Algoritm //
    long maxw=300000000l;
    Si16 fndnum=-1;
    //
	//  BREADTH-FIRST SEARCH ON GRAPH
	//
	// fld[] is used to:
	//  - hold sum of graph edge weights (length); iff >0
	//  - fld[] equals zero for all vertices we do not visit yet
	//
	// sts[] is used used to mark graph vertices:
	//  0=nomark
	//  1=starting or visited vertix
	//  2=vertix we want to walk to (with needed item)
	while (Exist) {
		Exist = 0;
		for (Si16 i = 0; i < Points; i++) { // iterate over all points
			if (sts[i] == 1) {
				sts[i] = 0;
				if (fld[i] > maxw) { // do not iterate vertices that are too far
					continue;
				}
				for (Si16 lnk = 0; lnk < pnt[i].NPnts; lnk++) { // iterate outgoing links
					if (fld[pnt[i].NPnt[lnk]] == 0 ||                           // we never been here
						fld[pnt[i].NPnt[lnk]] > fld[i] + pnt[i].Weight[lnk]) {  // we found shorter path
						if (sts[pnt[i].NPnt[lnk]] == 0) {
							sts[pnt[i].NPnt[lnk]] = 1;
							Exist = 1; // no stall
						}
						fld[pnt[i].NPnt[lnk]] = fld[i] + pnt[i].Weight[lnk];
						if (sts[pnt[i].NPnt[lnk]] == 2 && maxw > fld[pnt[i].NPnt[lnk]]) { // we found shorter path to some useful item
							maxw = fld[pnt[i].NPnt[lnk]];
							fndnum = pnt[i].NPnt[lnk]; // save resulting vertix
						}
					}
				}
			}
		}
	}
    // Decode Result -- Search Path //
    while(1) {
        for(Si16 i=0;i<Points;i++) {
            for(Si16 lnk=0;lnk<pnt[i].NPnts;lnk++) {
                if(pnt[i].NPnt[lnk]==fndnum && maxw-fld[i]==pnt[i].Weight[lnk]) {
                    if(i==start) {
                        bt->PntNum=bt->NPntNum;
                        bt->NPntNum=fndnum;
                        //   WARNING   //
                        //              if(rndlikewpn) bt->LikeWpn=M_WPN;
                        free(sts);
                        free(fld);
                        return;
                    }
                    maxw=fld[i];
					fndnum=i;
                }
            }
        }
    }
}
void SNPRandom(BOT *bt) // Select Next Point Random //
{
    Si16 Prev=bt->PntNum;
    Si16 Next=Prev;
    Si16 c=100;
	while (Next == Prev && c) {
		Next = pnt[bt->NPntNum].NPnt[random(pnt[bt->NPntNum].NPnts)];
		c--;
	}
    bt->PntNum=bt->NPntNum;
    bt->NPntNum=Next;
}
void SelectWeapon(BOT *bt)
{
    //---//    WpnPrior[M_WPN+1] = {10,30,40,50,60,   5,4,3,2,0};
    if(pl->wpn[bt->LikeWpn] && pl->ammo[bt->LikeWpn])
    {bt->Weapon=bt->LikeWpn;return;}
    bt->NoAmmos=0;
    bt->Weapon=M_WPN;
    for(Si16 i=0;i<M_WPN;i++)
    {
        if(pl->wpn[i] && pl->ammo[i] && WpnPrior[i]>WpnPrior[bt->Weapon])
            bt->Weapon=i;
    }
    if(bt->Weapon==M_WPN) {bt->NoAmmos=1;bt->Weapon=0;}
}
void Search(BOT *bt)
{
    if(plrevn[bt->Target].Length) {
        Si16 CodeCur=0;
        while(CodeCur!=plrevn[bt->Target].Length) {
            switch(plrevn[bt->Target].Code[CodeCur]) {
            case PE_DEATH:
                switch(bt->Skill) {
                case BS_MEAT: bt->Handler=MeatWalk; break;
                default: bt->Handler=NormalWalk;
                }
                return;
            }
            CodeCur+=PEventSize[plrevn[bt->Target].Code[CodeCur]];
        }
    }
    bt->rtx=bt->tx;
    bt->rty=bt->ty;
    if(!Border(pl->x,pl->y,bt->rtx,bt->rty))
        FindNextT(bt);
    switch(bt->Skill) {
    case BS_MEAT: bt->Handler=MeatSearchWalk; break;
    default: bt->Handler=SearchWalk;
    }
}
void ChaosWalk(BOT *bt)
{
    if(bt->rtx==80 && random(70)) return;
    bt->rtx=(bt->rtx+1)%80;
    switch(bt->rtx/10) {
    case 0: Make(C_RHT); break;
    case 1: Make(C_RHT); Make(C_BRK); break;
    case 2: Make(C_BRK); break;
    case 3: Make(C_BRK); Make(C_LFT); break;
    case 4: Make(C_LFT); break;
    case 5: Make(C_ACC); Make(C_LFT); break;
    case 6: Make(C_ACC); break;
    case 7: Make(C_ACC); Make(C_RHT); break;
    }
    if(!random(70)) bt->rtx=random(80);
    if(!random(160)) bt->rtx=80;
}

void AntiStallInTheWall(BOT* bt)
{
	if (!(GameTime % 6)) {
		if (ABS(bt->prev_x - pl->x) + ABS(bt->prev_y - pl->y) < 5) {
			bt->prev_cnt++;
			if (bt->prev_cnt > 15) {
				bt->prev_cnt = 0;
				SelectPointABattle(bt);
			}
		}
		else {
			bt->prev_cnt = 0;
		}
		if (!bt->prev_cnt) {
			bt->prev_cnt = 0;
			bt->prev_x = pl->x;
			bt->prev_y = pl->y;
		}
	}
}

/***************************************************************************/
/********************************  BRAINS  *********************************/
/***************************************************************************/

//-------------------------------  NORMAL  --------------------------------//

void FarBattle(BOT *)
{
}

void NormalWalk(BOT *)
{
}

void SearchWalk(BOT *)
{
}

//----------------------------------  MEAT  -------------------------------//

void MeatWalk(BOT *bt)
{
    /* Inside Event Flags */
    BYTE Reach=0,ChooseWeapon=0;
    /* OutSide Events Realize & Inside Events Generate */
    /* Izvrat */
    if(bt->NPntNum==-1) {
		SelectPointABattle(bt);
	}
    /* End Izvrat */
    if(event.Length)
    {
        Si16 CodeCur=0;
        while(CodeCur!=event.Length)
        {
            switch(event.Code[CodeCur])
            {
            case PE_DEATH:
                bt->PntNum=event.Code[CodeCur+1];
                bt->NPntNum=event.Code[CodeCur+1];
                RestartBot(bt);
                return;
            case PE_DAMAGE:
                if(!bt->TurnAround) bt->TurnAround=60*(-1+2*random(2));
                break;
            case PE_SEEPLR:
                if(!bt->NoAmmos
                        && FireLine(pl->x,pl->y,
                                    plr[event.Code[CodeCur+1]].x,
                                    plr[event.Code[CodeCur+1]].y)) {
                    SelectWeapon(bt);
                    StartFarBattle(bt,event.Code[CodeCur+1]);
                }
                break;
			case PE_SEEBLT: {
				Si16 i = *((WORD *)(event.Code + CodeCur + 1));
				if (blt[i].Handler == I_HealthHandler ||
					blt[i].Handler == I_ArmourHandler ||
					blt[i].Handler == I_BowHandler ||
					blt[i].Handler == I_AkmHandler ||
					blt[i].Handler == I_NapalmHandler ||
					blt[i].Handler == I_LauncherHandler ||
					blt[i].Handler == I_LaserHandler ||
					blt[i].Handler == I_PhotoHandler ||
					blt[i].Handler == I_GrenadeHandler ||
					blt[i].Handler == I_RedmerHandler ||
					blt[i].Handler == I_PistolAmmoHandler ||
					blt[i].Handler == I_BowAmmoHandler ||
					blt[i].Handler == I_NapalmAmmoHandler ||
					blt[i].Handler == I_AkmAmmoHandler ||
					blt[i].Handler == I_LauncherAmmoHandler ||
					blt[i].Handler == I_LaserAmmoHandler)
				{
					if (blt[i].age > 1) { // if item is not present
						if (bt->ItmMem[i] == 0) {
							bt->ItmMem[i] = (T_RESPAWN / _ItemAmount) + 2;
						}
					}
					else if (bt->ItmMem[i] > 0) { // item is in place
						bt->ItmMem[i] = 0;
					}
				}
				break;
			}
            case PE_NOAMMO:
                ChooseWeapon=1;
                break;
            case PE_TELEPORT:
                SelectPointABattle(bt);
                return;
            }
            CodeCur+=PEventSize[event.Code[CodeCur]];
        }
    }
    if(pl->x>=pnt[bt->NPntNum].x-P_SZ && pl->y>=pnt[bt->NPntNum].y-P_SZ &&
            pl->x<=pnt[bt->NPntNum].x+P_SZ && pl->y<=pnt[bt->NPntNum].y+P_SZ) {
		Reach=1;
	}
    /* Inside Events Realize */
	if (bt->ItmMem[bt->cnt1] > 0) {
		bt->ItmMem[bt->cnt1]--;
	}
    bt->cnt1++;
	if (bt->cnt1 == _ItemAmount) {
		bt->cnt1 = 0;
	}
	if (ChooseWeapon || bt->NoAmmos || !random(300)) {
		SelectWeapon(bt);
	}
	if (Reach) {
		SNP1(bt);
	}
    /* Commands Generation */
	if (pl->actwpn != bt->Weapon) {
		Make(C_NXT);
	}
	if (pl->trn == P_TRN2) {
		Make(C_TRN);
	}
    WalkTo(bt,pnt[bt->NPntNum].x,pnt[bt->NPntNum].y);
	if (!bt->TurnAround) {
		LookTo(bt, pnt[bt->NPntNum].x, pnt[bt->NPntNum].y);
	}
    else if(bt->TurnAround>0) {
		bt->TurnAround--;
		Make(C_A_P);
	}
    else if(bt->TurnAround<0) {
		bt->TurnAround++;
		Make(C_A_M);
	}
	AntiStallInTheWall(bt);
}

void MeatBattle(BOT *bt)
{
    Si16 BltIndex[50];
    /* Inside Event Flags */
    BYTE ChooseWeapon=0,SeeTarget=0,TargetDestroyed=0,SeeBlt=0,AlreadyMoved=0;
    /* OutSide Events Realize & Inside Events Generate */
    if(event.Length) {
        Si16 CodeCur=0;
        while(CodeCur!=event.Length) {
            switch(event.Code[CodeCur]) {
            case PE_DEATH:
                bt->PntNum=event.Code[CodeCur+1];
                bt->NPntNum=event.Code[CodeCur+1];
                RestartBot(bt);
                return;
            case PE_SEEBLT:
				if (SeeBlt < sizeof(BltIndex) / sizeof(*BltIndex)) {
					BltIndex[SeeBlt] = *((WORD *)(event.Code + CodeCur + 1));
					if (blt[BltIndex[SeeBlt]].Handler == B_PhotoA0Handler ||
						blt[BltIndex[SeeBlt]].Handler == B_PhotoA2Handler ||
						blt[BltIndex[SeeBlt]].Handler == B_PhotoA4Handler ||
						blt[BltIndex[SeeBlt]].Handler == B_PhotoA6Handler ||
						blt[BltIndex[SeeBlt]].Handler == B_GrenadeHandler) {
						SeeBlt++;
					}
				}
                break;
            case PE_SEEPLR:
				if (event.Code[CodeCur + 1] == bt->Target) {
					SeeTarget = 1;
				}
                break;
            case PE_NOAMMO:
				ChooseWeapon = 1;
                break;
            }
			CodeCur += PEventSize[event.Code[CodeCur]];
        }
    }
	if (plrevn[bt->Target].Length) {
		Si16 CodeCur = 0;
		while (CodeCur != plrevn[bt->Target].Length) {
			switch (plrevn[bt->Target].Code[CodeCur]) {
			case PE_DEATH: TargetDestroyed = 1; break;
			}
			if (TargetDestroyed) break;
			CodeCur += PEventSize[plrevn[bt->Target].Code[CodeCur]];
		}
	}
    /* Inside Events Realize */
	if (ChooseWeapon) {
		SelectWeapon(bt);
	}
	if (bt->NoAmmos || TargetDestroyed) {
		SelectPointABattle(bt);
	}
	if (!SeeTarget) {
		Search(bt);
	}
	if (bt->AvoidCode[0] != AE_END) {
		NormalizeAvoidEvents(bt, BltIndex, SeeBlt);
	}
    /* Commands Generation */
    if(pl->actwpn!=bt->Weapon) {
		Make(C_NXT);
	}
	if (SeeBlt) {
		for (Si16 i = 0; i < SeeBlt; i++) {
			if (blt[BltIndex[i]].Handler == B_PhotoA0Handler ||
				blt[BltIndex[i]].Handler == B_PhotoA2Handler ||
				blt[BltIndex[i]].Handler == B_PhotoA4Handler ||
				blt[BltIndex[i]].Handler == B_PhotoA6Handler) {
				continue;
			}
			if (blt[BltIndex[i]].Handler == B_GrenadeHandler &&
				GrenadeAvoid(bt, BltIndex[i]))
			{
				AlreadyMoved = 1; break;
			}
		}
	}
	if (SeeTarget) {
		if (!AlreadyMoved) {
			Si16 Tak, Sys = 1;
			GetDistance(bt, plr[bt->Target].x, plr[bt->Target].y);
			if (bt->Taktic == BT_NORMAL) {
				Tak = 90;
			}
			if (bt->Taktic == BT_SAFETY) {
				Tak = 110;
			}
			if (bt->Taktic == BT_AGRESSIVE) {
				Tak = 65;
			}
			if (pl->actwpn == B_AKM) {
				Tak -= 45;
				Sys = 2;
			}
			if (plr[bt->Target].actwpn == B_ROCKET) {
				Tak += 45;
			}
			switch (Sys) {
			case 1:
				if (bt->Distance > Tak + 20 || bt->Distance < Tak - 20) {
					AlreadyMoved = MakeDistance(bt, plr[bt->Target].x, plr[bt->Target].y, Tak);
				}
				if (!AlreadyMoved) {
					ChaosWalk(bt);
				}
				break;
			case 2:
				AlreadyMoved = MakeDistance(bt, plr[bt->Target].x, plr[bt->Target].y, Tak);
				break;
			}
		}
		bt->tx = plr[bt->Target].x;
		bt->ty = plr[bt->Target].y;
		if (ABS(plr[bt->Target].sx) < 0.05 && ABS(plr[bt->Target].sy) < 0.05) {
			if (bt->AimSize > 0) {
				bt->AimSize--;
			}
		}
		else {
			bt->AimSize = 10;
		}
		BYTE l = 0;
		l = LookTo(bt, plr[bt->Target].x, plr[bt->Target].y, 20);
		if (!l || !random(6)) {
			l = LookTo(bt, plr[bt->Target].x - bt->AimSize / 2 + random(bt->AimSize + 1),
				plr[bt->Target].y - bt->AimSize / 2 + random(bt->AimSize + 1),
				bt->AimFov);
		}
		if (pl->trn == P_TRN2) {
			Make(C_TRN);
		}
		if (!bt->Fire && l && pl->actwpn == bt->Weapon && pl->reload == 0) {
			//		&& FireLine(pl->x,pl->y,plr[bt->Target].x,plr[bt->Target].y))
			if (pl->actwpn == B_LASER) {
				if (!random(6)) bt->Fire = 50 + random(100);
				else bt->Fire = 5 + random(20);
			}
			else if (pl->actwpn == B_AKM) {
				bt->Fire = 25 + random(25);
			}
			else {
				bt->Fire = 5 + random(20);
			}
		}
	}
    if(bt->Fire>1) {
		bt->Fire--;
		Make(C_FIR);
	}
    else if(bt->Fire==1) {
		bt->Fire--;
	}
}

void MeatSearchWalk(BOT *bt)
{
    /* Inside Event Flags */
    BYTE Reach=0,ChooseWeapon=0;
    /* OutSide Events Realize & Inside Events Generate */
    if(event.Length)
    {
        Si16 CodeCur=0;
        while(CodeCur!=event.Length) {
            switch(event.Code[CodeCur]) {
            case PE_DEATH:
                bt->PntNum=event.Code[CodeCur+1];
                bt->NPntNum=event.Code[CodeCur+1];
                RestartBot(bt);
                return;
            case PE_DAMAGE:
                if(!bt->TurnAround) bt->TurnAround=60*(-1+2*random(2));
                break;
            case PE_SEEBLT:
                break;
            case PE_SEEPLR:
                StartFarBattle(bt,bt->Target);
                break;
            case PE_NOAMMO:
                ChooseWeapon=1;
                break;
            case PE_TELEPORT:
                break;
            }
            CodeCur+=PEventSize[event.Code[CodeCur]];
        }
    }
    if(pl->x>=bt->tx-P_SZ && pl->y>=bt->ty-P_SZ &&
            pl->x<=bt->tx+P_SZ && pl->y<=bt->ty+P_SZ)
    {Reach=1;}
    /* Inside Events Realize */
    if(ChooseWeapon || bt->NoAmmos) SelectWeapon(bt);
    if(Reach) {
        if(pl->x>=bt->rtx-P_SZ && pl->y>=bt->rty-P_SZ &&
                pl->x<=bt->rtx+P_SZ && pl->y<=bt->rty+P_SZ) {
            SelectPointABattle(bt);
        }
        else {
            if(Border(pl->x,pl->y,bt->rtx,bt->rty))
            {bt->tx=bt->rtx;bt->ty=bt->rty;}
            else {
                FindNextT(bt);
            }
        }
    }
    /* Commands Generation */
    if(pl->actwpn!=bt->Weapon) Make(C_NXT);
    if(pl->trn==P_TRN2) Make(C_TRN);
    WalkTo(bt,bt->tx,bt->ty);
    if(!bt->TurnAround) LookTo(bt,bt->tx,bt->ty);
    else if(bt->TurnAround>0) {bt->TurnAround--;Make(C_A_P);}
    else if(bt->TurnAround<0) {bt->TurnAround++;Make(C_A_M);}
	AntiStallInTheWall(bt);
}


#undef Make
#undef event
#undef pl
