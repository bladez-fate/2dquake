/*
2D Quake v1.04
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/

#include "defs.h"

//#include <conio.h>
#include <stdio.h>
#include "defs.h"
#include "2dqdefs.h"
#include "graph.h"
#include "game.h"

#define bl        ((OBJECT*)b)

char WpnName[M_WPN][16] = {
    "Пистолет",
    "Лук",
    "АКМ",
    "Напалм",
    "Базука",
    "Растяжка",
    "Граната",
    "УР Земля-Земля",
    "Лазер",
};

void I_PistolAmmoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_PistolAmmoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_SHELL]!=MaxAmmo[B_SHELL] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].ammo[B_SHELL]+=BA_SHELL;
                if(plr[i].ammo[B_SHELL]>MaxAmmo[B_SHELL])
                    plr[i].ammo[B_SHELL]=MaxAmmo[B_SHELL];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Пуль",BA_ARROW);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Пуль",BA_ARROW);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_BowAmmoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_BowAmmoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_ARROW]!=MaxAmmo[B_ARROW] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].ammo[B_ARROW]+=BA_ARROW;
                if(plr[i].ammo[B_ARROW]>MaxAmmo[B_ARROW])
                    plr[i].ammo[B_ARROW]=MaxAmmo[B_ARROW];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Стрел",BA_ARROW);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Стрел",BA_ARROW);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_AkmAmmoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_AkmAmmoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_AKM]!=MaxAmmo[B_AKM] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].ammo[B_AKM]+=BA_AKM;
                if(plr[i].ammo[B_AKM]>MaxAmmo[B_AKM])
                    plr[i].ammo[B_AKM]=MaxAmmo[B_AKM];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Патрон для АКМ",BA_AKM);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Патрон для АКМ",BA_AKM);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_NapalmAmmoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_NapalmAmmoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_NAPALM]!=MaxAmmo[B_NAPALM] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].ammo[B_NAPALM]+=BA_NAPALM;
                if(plr[i].ammo[B_NAPALM]>MaxAmmo[B_NAPALM])
                    plr[i].ammo[B_NAPALM]=MaxAmmo[B_NAPALM];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Капсул",BA_NAPALM);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Капсул",BA_NAPALM);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_LauncherAmmoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_LauncherAmmoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_ROCKET]!=MaxAmmo[B_ROCKET] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].ammo[B_ROCKET]+=BA_ROCKET;
                if(plr[i].ammo[B_ROCKET]>MaxAmmo[B_ROCKET])
                    plr[i].ammo[B_ROCKET]=MaxAmmo[B_ROCKET];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Рокеты",BA_ROCKET);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Рокеты",BA_ROCKET);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_LaserAmmoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_LaserAmmoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_LASER]!=MaxAmmo[B_LASER] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].ammo[B_LASER]+=BA_LASER;
                if(plr[i].ammo[B_LASER]>MaxAmmo[B_LASER])
                    plr[i].ammo[B_LASER]=MaxAmmo[B_LASER];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Энергии",BA_LASER);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {
                    char tmpstr[30]; sprintf(tmpstr,"%hd Энергии",BA_LASER);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_HealthHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_HealthDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].hp!=M_HEALTH &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].hp+=H_MEDIKIT;
                if(plr[i].hp>M_HEALTH)
                    plr[i].hp=M_HEALTH;
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) vp[0].MakeMessage("Аптечка");
                else if(vp[1].ActPlayer==i) vp[1].MakeMessage("Аптечка");
            }
        }
    }
}

void I_ArmourHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_ArmourDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].armr!=M_ARMOUR &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].armr+=H_BODYARMR;
                if(plr[i].armr>M_ARMOUR)
                    plr[i].armr=M_ARMOUR;
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) vp[0].MakeMessage("Бронежелет");
                else if(vp[1].ActPlayer==i) vp[1].MakeMessage("Бронежелет");
            }
        }
    }
}

void I_BowHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_BowDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(!plr[i].wpn[B_ARROW] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_ARROW]=1;
                plr[i].ammo[B_ARROW]+=BA_ARROW;
                if(plr[i].ammo[B_ARROW]>MaxAmmo[B_ARROW])
                    plr[i].ammo[B_ARROW]=MaxAmmo[B_ARROW];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {vp[0].MakeMessage("Лук");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Стрел",BA_ARROW);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {vp[1].MakeMessage("Лук");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Стрел",BA_ARROW);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_AkmHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_AkmDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(!plr[i].wpn[B_AKM] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_AKM]=1;
                plr[i].ammo[B_AKM]+=BA_AKM;
                if(plr[i].ammo[B_AKM]>MaxAmmo[B_AKM])
                    plr[i].ammo[B_AKM]=MaxAmmo[B_AKM];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {vp[0].MakeMessage("АКМ");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Патрон для АКМ",BA_AKM);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {vp[1].MakeMessage("АКМ");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Патрон для АКМ",BA_AKM);
                    vp[1].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_NapalmHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_NapalmDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(!plr[i].wpn[B_NAPALM] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_NAPALM]=1;
                plr[i].ammo[B_NAPALM]+=BA_NAPALM;
                if(plr[i].ammo[B_NAPALM]>MaxAmmo[B_NAPALM])
                    plr[i].ammo[B_NAPALM]=MaxAmmo[B_NAPALM];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {vp[0].MakeMessage("Напалм");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Капсул",BA_NAPALM);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {vp[1].MakeMessage("Напалм");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Капсул",BA_NAPALM);
                    vp[0].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_LauncherHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_LauncherDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(!plr[i].wpn[B_ROCKET] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_ROCKET]=1;
                plr[i].ammo[B_ROCKET]+=BA_ROCKET;
                if(plr[i].ammo[B_ROCKET]>MaxAmmo[B_ROCKET])
                    plr[i].ammo[B_ROCKET]=MaxAmmo[B_ROCKET];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {vp[0].MakeMessage("Базука");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Рокеты",BA_ROCKET);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {vp[1].MakeMessage("Базука");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Рокеты",BA_ROCKET);
                    vp[0].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_PhotoHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_PhotoDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_PHOTO] != MaxAmmo[B_PHOTO] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_PHOTO]=1;
                plr[i].ammo[B_PHOTO]+=BA_PHOTO;
                if(plr[i].ammo[B_PHOTO]>MaxAmmo[B_PHOTO])
                    plr[i].ammo[B_PHOTO]=MaxAmmo[B_PHOTO];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) vp[0].MakeMessage("Растяжка");
                else if(vp[1].ActPlayer==i) vp[1].MakeMessage("Растяжка");
            }
        }
    }
}

void I_GrenadeHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_GrenadeDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].ammo[B_GRENADE] != MaxAmmo[B_GRENADE] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_GRENADE]=1;
                plr[i].ammo[B_GRENADE]+=BA_GRENADE;
                if(plr[i].ammo[B_GRENADE]>MaxAmmo[B_GRENADE])
                    plr[i].ammo[B_GRENADE]=MaxAmmo[B_GRENADE];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) vp[0].MakeMessage("Граната");
                else if(vp[1].ActPlayer==i) vp[1].MakeMessage("Граната");
            }
        }
    }
}

void I_RedmerHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_RedmerDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(plr[i].wpn[B_REDMER]==0 && plr[i].ammo[B_REDMER] != MaxAmmo[B_REDMER] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_REDMER]=1;
                plr[i].ammo[B_REDMER]+=BA_REDMER;
                if(plr[i].ammo[B_REDMER]>MaxAmmo[B_REDMER])
                    plr[i].ammo[B_REDMER]=MaxAmmo[B_REDMER];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) vp[0].MakeMessage("УР Земля-Земля");
                else if(vp[1].ActPlayer==i) vp[1].MakeMessage("УР Земля-Земля");
            }
        }
    }
}

void I_LaserHandler(void *b)
{
    if(bl->age>1) {
        bl->age++;
        if(bl->age>=T_RESPAWN) {bl->age=1;bl->Draw=I_LaserDraw;}
        return;
    }
    else {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(!plr[i].wpn[B_LASER] &&
                    bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                    bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
                DrawStatistics=1;
                plr[i].wpn[B_LASER]=1;
                plr[i].ammo[B_LASER]+=BA_LASER;
                if(plr[i].ammo[B_LASER]>MaxAmmo[B_LASER])
                    plr[i].ammo[B_LASER]=MaxAmmo[B_LASER];
                bl->age++;
                bl->Draw=EmptyDraw;
                if(vp[0].ActPlayer==i) {vp[0].MakeMessage("Лазер");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Энергии",BA_LASER);
                    vp[0].MakeMessage(tmpstr);
                }
                else if(vp[1].ActPlayer==i) {vp[1].MakeMessage("Лазер");
                    char tmpstr[30]; sprintf(tmpstr,"%hd Энергии",BA_LASER);
                    vp[0].MakeMessage(tmpstr);
                }
            }
        }
    }
}

void I_CorpseHandler(void *b)
{
    Si16 ix,iy,det=0;
    bl->age+=8;
    bl->sx=bl->sx*0.85;bl->sy=bl->sy*0.85;
    bl->x+=bl->sx;bl->y+=bl->sy;
    ix=bl->x+P_SZ;iy=bl->y+P_SZ;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x+P_SZ;iy=bl->y-P_SZ;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-P_SZ;iy=bl->y+P_SZ;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-P_SZ;iy=bl->y-P_SZ;
    if(!FF_BltMov(ix,iy)) det=1;
    bl->x-=bl->sx;bl->y-=bl->sy;
    if(det) {
        bl->sx=0;
        bl->sy=0;
    }
    bl->x+=bl->sx;bl->y+=bl->sy;
    if(bl->age>T_CORPSE) bl->age=0;
}

void I_PackHandler(void *b)
{
    Si16 ix,iy,det=0;
    PACK *pk=bl->pack;  // sx --> pk //
    pk->sx=pk->sx*0.85;bl->sy=bl->sy*0.85;
    bl->x+=pk->sx;bl->y+=bl->sy;
    ix=bl->x+P_SZ;iy=bl->y+P_SZ; if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x+P_SZ;iy=bl->y-P_SZ; if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-P_SZ;iy=bl->y+P_SZ; if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-P_SZ;iy=bl->y-P_SZ; if(!FF_BltMov(ix,iy)) det=1;
    bl->x-=pk->sx;bl->y-=bl->sy;
    if(det) {pk->sx=0;bl->sy=0;}
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        if(bl->x>=plr[i].x-P_SZ*2 && bl->y>=plr[i].y-P_SZ*2 &&
                bl->x<=plr[i].x+P_SZ*2 && bl->y<=plr[i].y+P_SZ*2) {
            DrawStatistics=1;
            plr[i].ammo[B_SHELL]+=pk->Shell;
            plr[i].ammo[B_ARROW]+=pk->Arrow;
            plr[i].ammo[B_AKM]+=pk->AkmAmmo;
            plr[i].ammo[B_NAPALM]+=pk->NapalmAmmo;
            plr[i].ammo[B_ROCKET]+=pk->Rocket;
            plr[i].ammo[B_LASER]+=pk->LaserAmmo;
            if(!plr[i].wpn[pk->Weapon]) plr[i].wpn[pk->Weapon]=1;
            if(pk->Weapon==B_PHOTO ||
                    pk->Weapon==B_GRENADE ||
                    pk->Weapon==B_REDMER){
                plr[i].ammo[pk->Weapon]++;
            }
            if(plr[i].ammo[B_SHELL]>MaxAmmo[B_SHELL]) plr[i].ammo[B_SHELL]=MaxAmmo[B_SHELL];
            if(plr[i].ammo[B_ARROW]>MaxAmmo[B_ARROW]) plr[i].ammo[B_ARROW]=MaxAmmo[B_ARROW];
            if(plr[i].ammo[B_AKM]>MaxAmmo[B_AKM]) plr[i].ammo[B_AKM]=MaxAmmo[B_AKM];
            if(plr[i].ammo[B_NAPALM]>MaxAmmo[B_NAPALM]) plr[i].ammo[B_NAPALM]=MaxAmmo[B_NAPALM];
            if(plr[i].ammo[B_ROCKET]>MaxAmmo[B_ROCKET]) plr[i].ammo[B_ROCKET]=MaxAmmo[B_ROCKET];
            if(plr[i].ammo[B_PHOTO]>MaxAmmo[B_PHOTO]) plr[i].ammo[B_PHOTO]=MaxAmmo[B_PHOTO];
            if(plr[i].ammo[B_GRENADE]>MaxAmmo[B_GRENADE]) plr[i].ammo[B_GRENADE]=MaxAmmo[B_GRENADE];
            if(plr[i].ammo[B_REDMER]>MaxAmmo[B_REDMER]) plr[i].ammo[B_REDMER]=MaxAmmo[B_REDMER];
            if(plr[i].ammo[B_LASER]>MaxAmmo[B_LASER]) plr[i].ammo[B_LASER]=MaxAmmo[B_LASER];
            bl->age=0;
            bl->Draw=EmptyDraw;
            if(vp[0].ActPlayer==i) {
                char tmpstr[40]; sprintf(tmpstr,"%3dП %3dС %3dПА %3dК %3dР %3dЭ",
                                         pk->Shell,pk->Arrow,pk->AkmAmmo,pk->NapalmAmmo,pk->Rocket,pk->LaserAmmo);
                vp[0].MakeMessage(tmpstr);
                if(pk->Weapon!=M_WPN) vp[0].MakeMessage(WpnName[pk->Weapon]);
            }
            else if(vp[1].ActPlayer==i) {
                char tmpstr[40]; sprintf(tmpstr,"%3dП %3dС %3dПА %3dК %3dР %3dЭ",
                                         pk->Shell,pk->Arrow,pk->AkmAmmo,pk->NapalmAmmo,pk->Rocket,pk->LaserAmmo);
                vp[1].MakeMessage(tmpstr);
                if(pk->Weapon!=M_WPN) vp[1].MakeMessage(WpnName[pk->Weapon]);
            }
            free(pk);
        }
    }
    bl->x+=pk->sx;bl->y+=bl->sy;
}

void I_HealthDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[0].Width/2,y-ItemImg[0].Height/2,ItemImg);
}

void I_ArmourDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[1].Width/2,y-ItemImg[1].Height/2,ItemImg+1);
}

void I_PistolAmmoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[2].Width/2,y-ItemImg[2].Height/2,ItemImg+2);
}

void I_BowDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[3].Width/2,y-ItemImg[3].Height/2,ItemImg+3);
}

void I_BowAmmoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[4].Width/2,y-ItemImg[4].Height/2,ItemImg+4);
}

void I_AkmDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[5].Width/2,y-ItemImg[5].Height/2,ItemImg+5);
}

void I_AkmAmmoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[6].Width/2,y-ItemImg[6].Height/2,ItemImg+6);
}

void I_NapalmDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[7].Width/2,y-ItemImg[7].Height/2,ItemImg+7);
}

void I_NapalmAmmoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[8].Width/2,y-ItemImg[8].Height/2,ItemImg+8);
}

void I_LauncherDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[9].Width/2,y-ItemImg[9].Height/2,ItemImg+9);
}

void I_LauncherAmmoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[10].Width/2,y-ItemImg[10].Height/2,ItemImg+10);
}

void I_PhotoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[11].Width/2,y-ItemImg[11].Height/2,ItemImg+11);
}

void I_GrenadeDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[12].Width/2,y-ItemImg[12].Height/2,ItemImg+12);
}

void I_RedmerDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[13].Width/2,y-ItemImg[13].Height/2,ItemImg+13);
}

void I_LaserDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[14].Width/2,y-ItemImg[14].Height/2,ItemImg+14);
}

void I_LaserAmmoDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-ItemImg[15].Width/2,y-ItemImg[15].Height/2,ItemImg+15);
}

void I_CorpseDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
	DrawSWUImage(x-CrpsImg[(bl->age-1)%8].Width/2,y-CrpsImg[(bl->age-1)%8].Height/2,bl->own,CrpsImg+(bl->age-1)%8);
}

void I_PackDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
	DrawSWUImage(x-ItemImg[16].Width/2,y-ItemImg[16].Height/2,bl->own,ItemImg+16);
}
