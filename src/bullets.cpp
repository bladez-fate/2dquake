/*
2D Quake v1.04
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/
#include "defs.h"

#include <math.h>

#include "2dqdefs.h"
#include "graph.h"
#include "game.h"

#define bl        ((OBJECT*)b)

void B_SmokeHandler(void *b)
{
    bl->age++;
    Si16 ix,iy;

    ix=bl->x+bl->sx;iy=bl->y+bl->sy;
    if(!FF_BltMov(ix,iy)) {bl->age=0;return;}

    if(bl->age>A_SMOKE) {bl->age=0;return;}
    bl->x+=bl->sx-1+Random(3);
    bl->y+=bl->sy-1+Random(3);
}

void B_Smoke2Handler(void *b)
{
    bl->age++;
    if(bl->age>A_SMOKE2) {bl->age=0;return;}
    bl->x+=bl->sx;
    bl->y+=bl->sy;
    bl->sx*=0.7;bl->sy*=0.7;
    bl->sx+=(-10+Random(21))/20.;bl->sy+=(-10+Random(21))/20.;
}

void B_GilzaHandler(void *b)
{
    bl->age+=6;
    if(bl->age>A_GILZA) {bl->age=0;return;}
    if(bl->sx!=0 || bl->sy!=0) {
        if(bl->age>A_GILZA/2) {bl->sx=0;bl->sy=0;return;}
        if(!FF_BltMov(bl->x+bl->sx,bl->y+bl->sy))
        {bl->sx=0;bl->sy=0;return;}
        bl->x+=bl->sx;
        bl->y+=bl->sy;
        bl->sx*=0.6;bl->sy*=0.6;
        bl->sx+=(-10+Random(21))/32.;bl->sy+=(-10+Random(21))/32.;
        if(Si16(bl->sx*50)==0 && Si16(bl->sy*50)==0)
        {bl->sx=0;bl->sy=0;}
    }
}

void B_BloodHandler(void *b)
{
    bl->age++;
    Si16 ix,iy;
    ix=bl->x+bl->sx;iy=bl->y+bl->sy;
    if(!FF_BltMov(ix,iy)) {
        bl->sx=-bl->sx/2+(double)(-10+Random(21))/15.;
        bl->sy=-bl->sy/2+(double)(-10+Random(21))/15.;
    }
    if(bl->age>A_BLOOD) {bl->age=0;return;}
    bl->x+=bl->sx;bl->y+=bl->sy;
    bl->sx=bl->sx*0.90;bl->sy=bl->sy*0.90;
}

void B_ShellHandler(void *b)
{
    if(bl->age==1) {
        Si16 j;NextFreeBlt();j=BltFst;
        blt[j].age=1+Random(6);
        blt[j].own=bl->own;
        blt[j].x=bl->x;
        blt[j].y=bl->y;
        blt[j].sx=bl->sy*2;
        blt[j].sy=-bl->sx*2;
        blt[j].Handler=B_GilzaHandler;
        blt[j].Draw=B_GilzaDraw;
    }
    for(Si16 count=0;count<C_SHELL;count++)
    {
        bl->age++;
        Si16 ax,ay;
        ax=bl->x+bl->sx;ay=bl->y+bl->sy;
        if(!FF_BltMov(ax,ay))
        {
            for(Si16 j=0;j<BltExt;j++) {
                if(blt[j].age &&
                        (blt[j].Handler==B_PhotoA0Handler ||
                         blt[j].Handler==B_PhotoA2Handler ||
                         blt[j].Handler==B_PhotoA4Handler ||
                         blt[j].Handler==B_PhotoA6Handler )
                        && bl->x>=blt[j].x-5 && bl->y>=blt[j].y-5 && bl->x<=blt[j].x+5 && bl->y<=blt[j].y+5) {
                    blt[j].Handler=B_PhotoBoomHandler;blt[j].age=CR_PHOTO;
                    break;
                }
            }
            bl->age=1;
            bl->Handler=B_Smoke2Handler;
            bl->Draw=B_Smoke2Draw;
            if(FF_BltMov(bl->x,ay)) {bl->sx=-bl->sx;}
            else if(FF_BltMov(ax,bl->y)) {bl->sy=-bl->sy;}
            else {bl->sx=-bl->sx;bl->sy=-bl->sy;}
            return;
        }
        if(bl->age>A_SHELL) {bl->age=0;return;}
        bl->x+=bl->sx;bl->y+=bl->sy;

        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(bl->x>=plr[i].x-P_SZ && bl->y>=plr[i].y-P_SZ && bl->x<=plr[i].x+P_SZ && bl->y<=plr[i].y+P_SZ) {
                CheckFrag=1;
                plr[i].sx+=bl->sx/5;
                plr[i].sy+=bl->sy/5;
                plr[i].damage+=D_SHELL;
                plr[i].damageown=bl->own;
                for(Si16 a=0;a<1;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6);
                    blt[j].own=i;
                    blt[j].x=bl->x-1+Random(3);
                    blt[j].y=bl->y-1+Random(3);
                    blt[j].sx=bl->sx+(double)(-10+Random(21))/20.;
                    blt[j].sy=bl->sy+(double)(-10+Random(21))/20.;
                    blt[j].Handler=B_BloodHandler;
                    blt[j].Draw=B_BloodDraw;
                }
                bl->age=1;
                bl->Handler=B_Smoke2Handler;
                bl->Draw=B_Smoke2Draw;
                bl->sx=-bl->sx;
                bl->sy=-bl->sy;
                return;
            }
        }
    }
}

void B_ArrowHandler(void *b)
{
    bl->age++;
    if(bl->age>A_ARROW) {bl->age=0;return;}

    Si16 ix,iy;
    for(Si16 count=0;count<C_ARROW;count++) {
        ix=bl->x+bl->sx;iy=bl->y+bl->sy;
        if(!FF_BltMov(ix,iy)) {bl->age=0;return;}
        bl->x+=bl->sx;bl->y+=bl->sy;
        bl->sx*=0.995;bl->sy*=0.995;

        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(bl->x>=plr[i].x-P_SZ && bl->y>=plr[i].y-P_SZ && bl->x<=plr[i].x+P_SZ && bl->y<=plr[i].y+P_SZ) {
                CheckFrag=1;
                plr[i].sx+=bl->sx;
                plr[i].sy+=bl->sy;
                plr[i].damage+=D_ARROW;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<5;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6);
                    blt[j].own=i;
                    blt[j].x=bl->x-3+Random(7);
                    blt[j].y=bl->y-3+Random(7);
                    blt[j].sx=bl->sx*2+(double)(-10+Random(21))/15.;
                    blt[j].sy=bl->sy*2+(double)(-10+Random(21))/15.;
                    blt[j].Handler=B_BloodHandler;
                    blt[j].Draw=B_BloodDraw;
                }
                bl->age=0;
                return;
            }
        }
    }
}

void B_NapalmHandler(void *b)
{
    bl->age++;
	if (bl->age > A_NAPALM) { bl->age = 0; return; }
	
	Si16 ix,iy;
	for (Si16 count = 0; count < C_NAPALM; count++) {
		ix = bl->x + bl->sx; iy = bl->y + bl->sy;
		if (!FF_BltMov(ix, iy)) { bl->age = 0; return; }
		bl->x += bl->sx; bl->y += bl->sy;

		for (Si16 i = 0; i < Players; i++) {
			if (plr[i].State <= 0) continue;
			if (bl->x >= plr[i].x - P_SZ && bl->y >= plr[i].y - P_SZ && bl->x <= plr[i].x + P_SZ && bl->y <= plr[i].y + P_SZ) {
				CheckFrag = 1;
				plr[i].sx += bl->sx / 3;
				plr[i].sy += bl->sy / 3;
				plr[i].damage += D_NAPALM;
				plr[i].damageown = bl->own;
				plr[i].damgfrm = D_LEN;
				for (Si16 a = 0; a < 2; a++) {
					Si16 j; NextFreeBlt(); j = BltFst;
					blt[j].age = 1 + Random(6);
					blt[j].own = i;
					blt[j].x = bl->x - 3 + Random(7);
					blt[j].y = bl->y - 3 + Random(7);
					blt[j].sx = bl->sx * 2 + (double)(-10 + Random(21)) / 15.;
					blt[j].sy = bl->sy * 2 + (double)(-10 + Random(21)) / 15.;
					blt[j].Handler = B_BloodHandler;
					blt[j].Draw = B_BloodDraw;
				}
				bl->age = 1;
				bl->x = i;
				bl->y = plr[i].hp;
				bl->sx = plr[i].armr;
				bl->Handler = B_Napalm2Handler;
				bl->Draw = EmptyDraw;
				plr[i].acc = P_ACC / BRK_NAPALM;
			}
		}
	}
}

void B_Napalm2Handler(void *b)
{
    bl->age++;
	Si16 pi = Si16(bl->x); // x is used as player index
    if(bl->age>A_NAPALM2 || plr[pi].hp>bl->y || plr[pi].armr>bl->sx)
    {bl->age=0;plr[pi].acc=P_ACC;return;}
    else {bl->y=plr[pi].hp;bl->sx=plr[pi].armr;}
    if(!(bl->age%FD_NAPALM2)) {
        CheckFrag=1;
        plr[pi].damage+=D_NAPALM2;
        plr[pi].damageown=bl->own;
        Si16 j;NextFreeBlt();j=BltFst;
        blt[j].age=1+Random(6);
        blt[j].own=pi;
        blt[j].x=plr[pi].x-3+Random(7);
        blt[j].y=plr[pi].y-3+Random(7);
        blt[j].sx=-plr[pi].sx*2+(double)(-10+Random(21))/15.;
        blt[j].sy=-plr[pi].sy*2+(double)(-10+Random(21))/15.;
        blt[j].Handler=B_BloodHandler;
        blt[j].Draw=B_BloodDraw;
    }
}

void B_RocketHandler(void *b)
{
    Si16 ix,iy,det,smoke;
    det=0;
    if(bl->age==-1) {det=1;}
    bl->x+=bl->sx;bl->y+=bl->sy;
    ix=bl->x+SZ_REDMER;iy=bl->y+SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x+SZ_REDMER;iy=bl->y-SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-SZ_REDMER;iy=bl->y+SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-SZ_REDMER;iy=bl->y-SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    bl->x-=bl->sx;bl->y-=bl->sy;
    if(bl->age>2) {
        bl->age=3+(bl->age-2)%10;
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(bl->x>=plr[i].x-P_SZ-SZ_REDMER && bl->y>=plr[i].y-P_SZ-SZ_REDMER &&
                    bl->x<=plr[i].x+P_SZ+SZ_REDMER && bl->y<=plr[i].y+P_SZ+SZ_REDMER) {
                det=1;
                break;
            }
        }
    }
    else {bl->age++;}

    smoke=1;
    for(Si16 i=0;i<BltExt;i++) {
        if(blt[i].age && blt[i].Handler==B_ShellHandler &&
                bl->x>=blt[i].x-SZB_REDMER && bl->y>=blt[i].y-P_SZ-SZB_REDMER &&
                bl->x<=blt[i].x+SZB_REDMER && bl->y<=blt[i].y+P_SZ+SZB_REDMER) {
            det=1;
            blt[i].age=0;
        }
        if(smoke && !blt[i].age && !(bl->age%3)) {
            blt[i].age=1;
            blt[i].own=bl->own;
            blt[i].x=bl->x-2.5*bl->sx;
            blt[i].y=bl->y-2.5*bl->sy;
            blt[i].sx=0;
            blt[i].sy=0;
            blt[i].Handler=B_SmokeHandler;
            blt[i].Draw=B_SmokeDraw;
            smoke=0;
        }
    }

    if(det) {
        for(Si16 i=0;i<BltExt;i++) {
            if(blt[i].age &&
                    (blt[i].Handler==B_PhotoA0Handler ||
                     blt[i].Handler==B_PhotoA2Handler ||
                     blt[i].Handler==B_PhotoA4Handler ||
                     blt[i].Handler==B_PhotoA6Handler )
                    && Blast(bl->x,bl->y,blt[i].x,blt[i].y,RNGF_ROCKET)) {
                blt[i].age=1; blt[i].Handler=B_PhotoBoomHandler;
            }
        }
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGN_ROCKET)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*4;
                plr[i].sy=(plr[i].y-bl->y)/z*4;
                plr[i].damage+=DN_ROCKET;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<10;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=bl->x-3+Random(7); blt[j].y=bl->y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
            else if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGF_ROCKET)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*2;
                plr[i].sy=(plr[i].y-bl->y)/z*2;
                plr[i].damage+=DF_ROCKET;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<5;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=plr[i].x-3+Random(7); blt[j].y=plr[i].y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
        }
        for(Si16 h=0;h<16;h++) {
            Si16 k;NextFreeBlt();k=BltFst;
            blt[k].age=1;
            blt[k].own=bl->own;
            blt[k].x=bl->x+2*cos((double)h*2*M_PI/16.);
            blt[k].y=bl->y-2*sin((double)h*2*M_PI/16.);
            blt[k].sx=2*cos((double)h*2*M_PI/16.);
            blt[k].sy=-2*sin((double)h*2*M_PI/16.);
            blt[k].Handler=B_SmokeHandler;
            blt[k].Draw=B_SmokeDraw;
        }
        bl->age=0;
        return;
    }
    bl->x+=bl->sx;bl->y+=bl->sy;
}

void B_PhotoBoomHandler(void *b)
{
    if(bl->age<CR_PHOTO) {bl->age++; return;}
    else {
        for(Si16 i=0;i<BltExt;i++) {
            if(blt[i].age &&
                    (blt[i].Handler==B_PhotoA0Handler ||
                     blt[i].Handler==B_PhotoA2Handler ||
                     blt[i].Handler==B_PhotoA4Handler ||
                     blt[i].Handler==B_PhotoA6Handler )
                    && Blast(bl->x,bl->y,blt[i].x,blt[i].y,RNGF_PHOTO)) {
                blt[i].age=1; blt[i].Handler=B_PhotoBoomHandler;
            }
        }
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGN_PHOTO)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*8;
                plr[i].sy=(plr[i].y-bl->y)/z*8;
                plr[i].damage+=DN_PHOTO;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<10;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=bl->x-3+Random(7); blt[j].y=bl->y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
            else if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGF_PHOTO)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*5;
                plr[i].sy=(plr[i].y-bl->y)/z*5;
                plr[i].damage+=DF_PHOTO;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<5;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=plr[i].x-3+Random(7); blt[j].y=plr[i].y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
        }
        for(Si16 h=0;h<16;h++) {
            Si16 k;NextFreeBlt();k=BltFst;
            blt[k].age=1;
            blt[k].own=bl->own;
            blt[k].x=bl->x+2*cos((double)h*2*M_PI/16.);
            blt[k].y=bl->y-2*sin((double)h*2*M_PI/16.);
            blt[k].sx=2*cos((double)h*2*M_PI/16.);
            blt[k].sy=-2*sin((double)h*2*M_PI/16.);
            blt[k].Handler=B_SmokeHandler;
            blt[k].Draw=B_SmokeDraw;
        }
        bl->age=0; return;
    }
}

void B_PhotoA0Handler(void *b)
{
    Si16 iy;
    if(!FF_PlrMov(Si16(bl->x),Si16(bl->y)) || FF_PlrMov(Si16(bl->x+1),Si16(bl->y))) {
        bl->age=CR_PHOTO; bl->Handler=B_PhotoBoomHandler;
        return;
    }
    if(bl->age<NA_PHOTO) {bl->age++; return;}
    iy=bl->y;
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        if(plr[i].x<bl->x && plr[i].y-P_SZ<bl->y && plr[i].y+P_SZ>bl->y) {
            for(Si16 j=bl->x-1;;j--) {
                if(j==(Si16)plr[i].x) {
                    bl->age=CR_PHOTO;
                    bl->Handler=B_PhotoBoomHandler;
                    return;
                }
                if(!FF_PlrSee(j,iy)) {
                    break;
                }
            }
        }
    }
}

void B_PhotoA2Handler(void *b)
{
    Si16 ix;
    if(!FF_PlrMov(Si16(bl->x),Si16(bl->y)) || FF_PlrMov(Si16(bl->x),Si16(bl->y-1))) {
        bl->age=CR_PHOTO; bl->Handler=B_PhotoBoomHandler;
        return;
    }
    if(bl->age<NA_PHOTO) {bl->age++; return;}
    ix=bl->x;
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        if(plr[i].y>bl->y && plr[i].x-P_SZ<bl->x && plr[i].x+P_SZ>bl->x) {
            for(Si16 j=bl->y+1;;j++) {
                if(j==(Si16)plr[i].y) {
                    bl->age=CR_PHOTO;
                    bl->Handler=B_PhotoBoomHandler;
                    return;
                }
                if(!FF_PlrSee(ix,j)) {
                    break;
                }
            }
        }
    }
}

void B_PhotoA4Handler(void *b)
{
    Si16 iy;
    if(!FF_PlrMov(Si16(bl->x),Si16(bl->y)) || FF_PlrMov(Si16(bl->x-1),Si16(bl->y))) {
        bl->age=CR_PHOTO; bl->Handler=B_PhotoBoomHandler;
        return;
    }
    if(bl->age<NA_PHOTO) {bl->age++; return;}
    iy=bl->y;
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        if(plr[i].x>bl->x && plr[i].y-P_SZ<bl->y && plr[i].y+P_SZ>bl->y) {
            for(Si16 j=bl->x+1;;j++) {
                if(j==(Si16)plr[i].x) {
                    bl->age=CR_PHOTO;
                    bl->Handler=B_PhotoBoomHandler;
                    return;
                }
                if(!FF_PlrSee(j,iy)) {
                    break;
                }
            }
        }
    }
}

void B_PhotoA6Handler(void *b)
{
    Si16 ix;
    if(!FF_PlrMov(Si16(bl->x),Si16(bl->y)) || FF_PlrMov(Si16(bl->x),Si16(bl->y+1))) {
        bl->age=CR_PHOTO; bl->Handler=B_PhotoBoomHandler;
        return;
    }
    if(bl->age<NA_PHOTO) {bl->age++; return;}
    ix=bl->x;
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        if(plr[i].y<bl->y && plr[i].x-P_SZ<bl->x && plr[i].x+P_SZ>bl->x) {
            for(Si16 j=bl->y-1;;j--) {
                if(j==(Si16)plr[i].y) {
                    bl->age=CR_PHOTO;
                    bl->Handler=B_PhotoBoomHandler;
                    return;
                }
                if(!FF_PlrSee(ix,j)) {
                    break;
                }
            }
        }
    }
}

void B_GrenadeHandler(void *b)
{
    bl->age++;
    Si16 ix,iy;
    ix=bl->x+bl->sx;iy=bl->y;
    if(!FF_BltMov(ix,iy)) {
        bl->sx=-bl->sx;
    }
    ix=bl->x;iy=bl->y+bl->sy;
    if(!FF_BltMov(ix,iy)) {
        bl->sy=-bl->sy;
    }
    if(bl->age>A_GRENADE)
    {
        for(Si16 i=0;i<BltExt;i++) {
            if(blt[i].age &&
                    (blt[i].Handler==B_PhotoA0Handler ||
                     blt[i].Handler==B_PhotoA2Handler ||
                     blt[i].Handler==B_PhotoA4Handler ||
                     blt[i].Handler==B_PhotoA6Handler )
                    && Blast(bl->x,bl->y,blt[i].x,blt[i].y,RNGF_GRENADE)) {
                blt[i].age=1; blt[i].Handler=B_PhotoBoomHandler;
            }
        }
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGN_GRENADE)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*4;
                plr[i].sy=(plr[i].y-bl->y)/z*4;
                plr[i].damage+=DN_GRENADE;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<10;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=bl->x-3+Random(7); blt[j].y=bl->y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
            else if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGF_GRENADE)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*2;
                plr[i].sy=(plr[i].y-bl->y)/z*2;
                plr[i].damage+=DF_GRENADE;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<5;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=plr[i].x-3+Random(7); blt[j].y=plr[i].y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
        }
        for(Si16 h=0;h<16;h++) {
            Si16 k;NextFreeBlt();k=BltFst;
            blt[k].age=1;
            blt[k].own=bl->own;
            blt[k].x=bl->x+2*cos((double)h*2*M_PI/16.);
            blt[k].y=bl->y-2*sin((double)h*2*M_PI/16.);
            blt[k].sx=2*cos((double)h*2*M_PI/16.);
            blt[k].sy=-2*sin((double)h*2*M_PI/16.);
            blt[k].Handler=B_SmokeHandler;
            blt[k].Draw=B_SmokeDraw;
        }
        bl->age=0;
        return;
    }
    bl->x+=bl->sx;bl->y+=bl->sy;
    bl->sx=bl->sx*0.96;bl->sy=bl->sy*0.96;
}

void B_RedmerHandler(void *b)
{
    Si16 ix,iy,det,smoke;
    plr[bl->own].reload++;
    det=0;
    if(plr[bl->own].wpn[B_REDMER]<=1) det=1;
    if(plr[bl->own].cmnd[C_A_P] || plr[bl->own].cmnd[C_A_M]) {
        bl->sx=S_REDMER*cos(plr[bl->own].a);
        bl->sy=-S_REDMER*sin(plr[bl->own].a);
    }
    bl->x+=bl->sx;bl->y+=bl->sy;
    ix=bl->x+SZ_REDMER;iy=bl->y+SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x+SZ_REDMER;iy=bl->y-SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-SZ_REDMER;iy=bl->y+SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    ix=bl->x-SZ_REDMER;iy=bl->y-SZ_REDMER;
    if(!FF_BltMov(ix,iy)) det=1;
    bl->x-=bl->sx;bl->y-=bl->sy;
    if(bl->age>=30) {
        bl->age=30+bl->age%30;
        if(plr[bl->own].cmnd[C_FIR]) det=1;
    }
    if(bl->age>10) {
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(bl->x>=plr[i].x-P_SZ-SZ_REDMER && bl->y>=plr[i].y-P_SZ-SZ_REDMER &&
                    bl->x<=plr[i].x+P_SZ+SZ_REDMER && bl->y<=plr[i].y+P_SZ+SZ_REDMER) {
                det=1;
                break;
            }
        }
    }
    smoke=1;
    bl->age++;
    for(Si16 i=0;i<BltExt;i++) {
        if(blt[i].age && blt[i].Handler==B_ShellHandler &&
                bl->x>=blt[i].x-SZB_REDMER && bl->y>=blt[i].y-P_SZ-SZB_REDMER &&
                bl->x<=blt[i].x+SZB_REDMER && bl->y<=blt[i].y+P_SZ+SZB_REDMER) {
            det=1;
            blt[i].age=0;
        }
        if(smoke==1 && !blt[i].age && !((bl->age)%3)) {
            blt[i].age=1;
            blt[i].own=bl->own;
            blt[i].x=bl->x-2.5*bl->sx;
            blt[i].y=bl->y-2.5*bl->sy;
            blt[i].sx=0;
            blt[i].sy=0;
            blt[i].Handler=B_SmokeHandler;
            blt[i].Draw=B_SmokeDraw;
            smoke--;
        }
    }
    if(det) {
        for(Si16 i=0;i<BltExt;i++) {
            if(blt[i].age &&
                    (blt[i].Handler==B_PhotoA0Handler ||
                     blt[i].Handler==B_PhotoA2Handler ||
                     blt[i].Handler==B_PhotoA4Handler ||
                     blt[i].Handler==B_PhotoA6Handler )
                    && Blast(bl->x,bl->y,blt[i].x,blt[i].y,RNGF_REDMER)) {
                blt[i].age=1; blt[i].Handler=B_PhotoBoomHandler;
            }
        }
        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGN_REDMER)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*8;
                plr[i].sy=(plr[i].y-bl->y)/z*8;
                plr[i].damage+=DN_REDMER;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<10;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=bl->x-3+Random(7); blt[j].y=bl->y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
            else if(Blast(bl->x,bl->y,plr[i].x,plr[i].y,RNGF_REDMER)) {
                CheckFrag=1;
                double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
                plr[i].sx=(plr[i].x-bl->x)/z*5;
                plr[i].sy=(plr[i].y-bl->y)/z*5;
                plr[i].damage+=DF_REDMER;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                for(Si16 a=0;a<5;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=plr[i].x-3+Random(7); blt[j].y=plr[i].y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
        }
        for(Si16 h=0;h<32;h++) {
            Si16 k;NextFreeBlt();k=BltFst;
            blt[k].age=1;
            blt[k].own=bl->own;
            blt[k].x=bl->x+2*cos((double)h*2*M_PI/32.);
            blt[k].y=bl->y-2*sin((double)h*2*M_PI/32.);
            blt[k].sx=3*cos((double)h*2*M_PI/32.);
            blt[k].sy=-3*sin((double)h*2*M_PI/32.);
            blt[k].Handler=B_SmokeHandler;
            blt[k].Draw=B_SmokeDraw;
        }
        plr[bl->own].wpn[B_REDMER]=0;
        bl->age=0;
        return;
    }
    bl->x+=bl->sx;bl->y+=bl->sy;
}

void B_LaserHandler(void *b)
{
    for(Si16 count=0;count<C_LASER;count++) {
        Si16 ix,iy;
        ix=bl->x+bl->sx;iy=bl->y;
        if(!FF_PlrSee(ix,iy)) {
            bl->sx=-bl->sx;
            if(ABS(bl->sx)>=ABS(bl->sy)) {bl->age=0;return;}
        }
        ix=bl->x;iy=bl->y+bl->sy;
        if(!FF_PlrSee(ix,iy)) {
            bl->sy=-bl->sy;
            if(ABS(bl->sx)<=ABS(bl->sy)) {bl->age=0;return;}
        }
        bl->x+=bl->sx;bl->y+=bl->sy;

        for(Si16 i=0;i<Players;i++) {
            if(plr[i].State<=0) continue;
            if(bl->x>=plr[i].x-P_SZ && bl->y>=plr[i].y-P_SZ && bl->x<=plr[i].x+P_SZ && bl->y<=plr[i].y+P_SZ) {
                CheckFrag=1;
                plr[i].sx+=bl->sx*bl->age*2/MD_LASER;
                plr[i].sy+=bl->sy*bl->age*2/MD_LASER;
                plr[i].damage+=bl->age;
                plr[i].damageown=bl->own;
                plr[i].damgfrm=D_LEN;
                Si16 bb=10*bl->age/MD_LASER;
                for(Si16 a=0;a<bb;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6);
                    blt[j].own=i;
                    blt[j].x=bl->x-3+Random(7);
                    blt[j].y=bl->y-3+Random(7);
                    blt[j].sx=plr[i].sx*2+(double)(-10+Random(21))/15.;
                    blt[j].sy=plr[i].sy*2+(double)(-10+Random(21))/15.;
                    blt[j].Handler=B_BloodHandler;
                    blt[j].Draw=B_BloodDraw;
                }
                bl->age=0;
            }
        }
    }
}

void B_AimHandler(void *b)
{
    bl->x=210+bl->age*sin(bl->sx);
    bl->y=210+bl->age*cos(bl->sx);
    bl->sx+=bl->sy;
    if(bl->sx>2*M_PI) bl->sx-=2*M_PI;
    for(Si16 i=0;i<BltExt;i++)
    {
        if(blt[i].age && blt[i].Handler==B_ArrowHandler &&
                bl->x>=blt[i].x-P_SZ && bl->y>=blt[i].y-P_SZ &&
                bl->x<=blt[i].x+P_SZ && bl->y<=blt[i].y+P_SZ) {
            plr[blt[i].own].frag++;
            plr[blt[i].own].ammo[B_ARROW]+=4;
            blt[i].age=0;
            for(Si16 a=0;a<5;a++) {
                Si16 j;NextFreeBlt();j=BltFst;
                blt[j].age=1+Random(6); blt[j].own=-1;
                blt[j].x=blt[i].x-3+Random(7); blt[j].y=blt[i].y-3+Random(7);
                blt[j].sx=blt[i].sx+(double)(-30+Random(61))/15.;
                blt[j].sy=blt[i].sy+(double)(-30+Random(61))/15.;
                blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
            }
        }
    }
}

void B_BallHandler(void *b)
{
    bl->age+=Si16(ABS(bl->sx)+ABS(bl->sy)+0.9);
    if(bl->age>10000) bl->age=1;
    Si16 ix,iy;
    ix=bl->x+bl->sx;iy=bl->y;
    if(!FF_BltMov(ix+BALL_SZ,iy+BALL_SZ) ||
            !FF_BltMov(ix-BALL_SZ,iy+BALL_SZ) ||
            !FF_BltMov(ix+BALL_SZ,iy-BALL_SZ) ||
            !FF_BltMov(ix-BALL_SZ,iy-BALL_SZ)) bl->sx=-bl->sx;
    ix=bl->x;iy=bl->y+bl->sy;
    if(!FF_BltMov(ix+BALL_SZ,iy+BALL_SZ) ||
            !FF_BltMov(ix-BALL_SZ,iy+BALL_SZ) ||
            !FF_BltMov(ix+BALL_SZ,iy-BALL_SZ) ||
            !FF_BltMov(ix-BALL_SZ,iy-BALL_SZ)) bl->sy=-bl->sy;
    bl->x+=bl->sx;bl->y+=bl->sy;
    bl->sx*=BALL_FRICTION;bl->sy*=BALL_FRICTION;
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        if(bl->x>=plr[i].x-BALL_SZ-P_SZ && bl->y>=plr[i].y-BALL_SZ-P_SZ &&
                bl->x<=plr[i].x+BALL_SZ+P_SZ && bl->y<=plr[i].y+BALL_SZ+P_SZ) {
            CheckFrag=1;
            double z=sqrt((bl->x-plr[i].x)*(bl->x-plr[i].x)+(bl->y-plr[i].y)*(bl->y-plr[i].y));
            bl->sx+=plr[i].sx/BALL_MASS;
            bl->sy+=plr[i].sy/BALL_MASS;
            if(Si16(bl->sx*20) && Si16(bl->sy*20) &&
                    bl->x+bl->sx*7>=plr[i].x-plr[i].sx/2-BALL_SZ-P_SZ && bl->y+bl->sy*7>=plr[i].y-plr[i].sy/2-BALL_SZ-P_SZ &&
                    bl->x+bl->sx*7<=plr[i].x-plr[i].sx/2+BALL_SZ+P_SZ && bl->y+bl->sy*7<=plr[i].y-plr[i].sy/2+BALL_SZ+P_SZ) {
                plr[i].damage+=plr[i].hp+plr[i].armr+1;
                plr[i].damageown=-1;
                plr[i].sx=0;
                plr[i].sy=0;
                for(Si16 a=0;a<5;a++) {
                    Si16 j;NextFreeBlt();j=BltFst;
                    blt[j].age=1+Random(6); blt[j].own=i;
                    blt[j].x=plr[i].x-3+Random(7); blt[j].y=plr[i].y-3+Random(7);
                    blt[j].sx=plr[i].sx+(double)(-30+Random(61))/15.;
                    blt[j].sy=plr[i].sy+(double)(-30+Random(61))/15.;
                    blt[j].Handler=B_BloodHandler; blt[j].Draw=B_BloodDraw;
                }
            }
            else {
                plr[i].sx+=(plr[i].x-bl->x)/z;
                plr[i].sy+=(plr[i].y-bl->y)/z;
            }
        }
    }
    for(Si16 i=0;i<BltExt;i++)
    {
        if(blt[i].age && blt[i].Handler==B_RocketHandler &&
                bl->x>=blt[i].x-BALL_SZ && bl->y>=blt[i].y-BALL_SZ &&
                bl->x<=blt[i].x+BALL_SZ && bl->y<=blt[i].y+BALL_SZ) {
            blt[i].age=-1;
            bl->sx+=blt[i].sx/BALL_MASS;
            bl->sy+=blt[i].sy/BALL_MASS;
        }
    }
}

/***********************/
/****   DRAW BLTs   ****/
/***********************/

void B_SmokeDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
    if(bl->age<5)SetColor(112-bl->age);
    else SetColor(83-(bl->age)%16);
    WinBufPixel(x,y);
}

void B_Smoke2Draw(Si16 x,Si16 y,void *, VIEWPORT*)
{
    SetColor(10+random(4));
    WinBufPixel(x,y);
}

void B_BloodDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
    SetColor(72+bl->age%6); WinBufPixel(x,y);
    SetColor(72+bl->age%6); WinBufPixel(x-2+random(5),y-2+random(5));
    SetColor(72+bl->age%6); WinBufPixel(x-2+random(5),y-2+random(5));
}

void B_ShellDraw(Si16 x, Si16 y, void *b, VIEWPORT* v)
{
	int len = std::min<int>(C_SHELL, bl->age);
	int x0 = x - len*bl->sx;
	int y0 = y - len*bl->sy;
	v->linFilter->TraceLight(x0, y0, x, y, 2, Rgba(255, 255, 50, 0), Rgba(255, 255, 50, 100));
}

void B_GilzaDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
	SetColor(148+bl->age%6);
	WinBufPixel(x,y);
}

void B_ArrowDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
	SetColor(177); 
	Line(x-bl->sx,y-bl->sy,x+bl->sx,y+bl->sy);
}

void B_NapalmDraw(Si16 x,Si16 y,void *b, VIEWPORT* v)
{
    SetColor(111); WinBufPixel(x,y);
    SetColor(79-(bl->age   )%16); WinBufPixel(x+1,y);
    SetColor(79-(bl->age+ 4)%16); WinBufPixel(x,y+1);
    SetColor(79-(bl->age+ 8)%16); WinBufPixel(x-1,y);
    SetColor(79-(bl->age+12)%16); WinBufPixel(x,y-1);
    SetColor(79-(bl->age+ 0)%16); WinBufPixel(x-bl->sx,y-bl->sy);
    SetColor(79-(bl->age+ 4)%16); WinBufPixel(x-bl->sx*2,y-bl->sy*2);
    SetColor(79-(bl->age+ 8)%16); WinBufPixel(x-bl->sx*3,y-bl->sy*3);
    SetColor(79-(bl->age+12)%16); WinBufPixel(x-bl->sx*4,y-bl->sy*4);
	v->linFilter->SpotLight(x, y, 10, Rgba(255, 0, 0, 100));
}
void B_RocketDraw(Si16 x,Si16 y,void *b, VIEWPORT* v)
{
    double angle;
    if((Si16)bl->sx==0) {
        if(bl->sy<0) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(-bl->sy,bl->sx);}
    if(angle<0) angle+=2*M_PI;
    BYTE da=((BYTE)((angle+M_PI/8)*4/M_PI))%8;
    DrawSWUImage(x-4,y-4,bl->own,RkImg+da);
	v->linFilter->SpotLight(x - bl->sx, y - bl->sy, 15, Rgba(255, 255, 200, 100));
}

void B_PhotoHorzDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
    SetColor(10); WinBufPixel(x,y-1);
    if(bl->age<NA_PHOTO) {SetColor(56); WinBufPixel(x,y);}
    else {SetColor(76); WinBufPixel(x,y);}
    SetColor(10); WinBufPixel(x,y+1);
}

void B_PhotoVertDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
    SetColor(10); WinBufPixel(x-1,y);
    if(bl->age<NA_PHOTO) {SetColor(56); WinBufPixel(x,y);}
    else {SetColor(76); WinBufPixel(x,y);}
    SetColor(10); WinBufPixel(x+1,y);
}

void B_GrenadeDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
    if(Si16(bl->sx*100) || Si16(bl->sy*100))
        DrawSWImage(x-2,y-2,GrImg+(bl->age/7)%4);
    else
        DrawSWImage(x-2,y-2,GrImg);
}

void B_RedmerDraw(Si16 x,Si16 y,void *b, VIEWPORT* v)
{
    double angle;
    if(Si16(bl->sx*100)==0) {
        if(bl->sy<0) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(-bl->sy,bl->sx);}
    if(angle<0) angle+=2*M_PI;
    BYTE da=((BYTE)((angle+M_PI/8)*4/M_PI))%8;
    DrawSWUImage(x-6,y-6,bl->own,RdImg+da);
	v->linFilter->SpotLight(x - bl->sx, y - bl->sy, 20, Rgba(255, 255, 220, 120));
}

void B_LaserDraw(Si16 x,Si16 y,void *b, VIEWPORT* v)
{
    SetColor(105);
    for(Si16 i=0;i<10;i++) {
        Si16 a=random(bl->age*8/MD_LASER);
        WinBufPixel(x+bl->sx*a/S_LASER,y+bl->sy*a/S_LASER);
    }
    SetColor(104+(bl->age*7)/(MD_LASER));
    WinBufPixel(x,y);
	v->linFilter->SpotLight(x, y, 12, Rgba(255, 255, 0, std::min(200, 20 + bl->age / 2)));
}

void B_AimDraw(Si16 x,Si16 y,void *, VIEWPORT*)
{
	DrawSWImage(x-6,y-6,&AimImg);
}

void B_BallDraw(Si16 x,Si16 y,void *b, VIEWPORT*)
{
    DrawSWImage(x-10,y-10,BallImg+((bl->age/BALL_FRM_CHANGE)%4));
}
