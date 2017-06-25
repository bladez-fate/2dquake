/*
2D Quake v1.04
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/
#include "defs.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "2dqdefs.h"
#include "graph.h"
#include "game.h"

Si16 SmokeGenerate;
Si16 _Turn;
BYTE door0;
BYTE door1;
BYTE door2;
BYTE door3;
BYTE door0w;
BYTE door1w;
BYTE door2w;
BYTE door3w;
Si16 FragSave[4];
Si16 Ball;
Si16 nn;

void M_Map0Handler(void)
{
    /* 0-close 1-opening 2-open 3-closing */
    for(Si16 j=0;j<Players;j++) {
        if(plr[j].State<=0) continue;
        if(plr[j].cmnd[C_USE] && plr[j].x>=200 && plr[j].y>=280 &&
                plr[j].x< 260 && plr[j].y< 320) {
            if(door0==0 || door0==3) {door0=1; door0w=0;}
            else if(door0==1 || door0==2) {door0=3; door0w=0;}
        }
        if(plr[j].cmnd[C_USE] && plr[j].x>=420 && plr[j].y>=360 &&
                plr[j].x< 480 && plr[j].y< 400) {
            if(door1==0 || door1==3) {door1=1; door1w=0;}
            else if(door1==1 || door1==2) {door1=3; door1w=0;}
        }
        if(plr[j].cmnd[C_USE] && plr[j].x>=280 && plr[j].y>=420 &&
                plr[j].x< 320 && plr[j].y< 480) {
            if(door2==0 || door2==3) {door2=1; door2w=0;}
            else if(door2==1 || door2==2) {door2=3; door2w=0;}
        }
        if(plr[j].cmnd[C_USE] && plr[j].x>=360 && plr[j].y>=200 &&
                plr[j].x< 400 && plr[j].y< 260) {
            if(door3==0 || door3==3) {door3=1; door3w=0;}
            else if(door3==1 || door3==2) {door3=3; door3w=0;}
        }
    }
    if(door0==1) {door0w++;
        if(door0w>5) {
            door0w=0;
            if(Map.Bits[487]==38) door0=2;
            else {Map.Bits[487]++;Map.Bits[521]++;}
        }
    }
    else if(door0==2) {door0w++;
        if(door0w>200) {door0=3; door0w=0;}
    }
    else if(door0==3) {door0w++;
        if(door0w>25) {
            door0w=0;
            if(Map.Bits[487]==34) door0=0;
            else {Map.Bits[487]--;Map.Bits[521]--;}
            for(Si16 j=0;j<Players;j++) {
                if(plr[j].State<=0) continue;
                Si16 ix,iy,zb=0;
                if((226>=plr[j].x-P_SZ+1 && 299-(Map.Bits[487]-34)*4>=plr[j].y-P_SZ+1  &&
                    226<=plr[j].x+P_SZ-1 && 299-(Map.Bits[487]-34)*4<=plr[j].y+P_SZ-1) ||
                        (233>=plr[j].x-P_SZ+1 && 299-(Map.Bits[487]-34)*4>=plr[j].y-P_SZ+1  &&
                         233<=plr[j].x+P_SZ-1 && 299-(Map.Bits[487]-34)*4<=plr[j].y+P_SZ-1)) {
                    plr[j].y+=4;
                }
                if((226>=plr[j].x-P_SZ+1 && 300+(Map.Bits[487]-34)*4>=plr[j].y-P_SZ+1  &&
                    226<=plr[j].x+P_SZ-1 && 300+(Map.Bits[487]-34)*4<=plr[j].y+P_SZ-1) ||
                        (233>=plr[j].x-P_SZ+1 && 300+(Map.Bits[487]-34)*4>=plr[j].y-P_SZ+1  &&
                         233<=plr[j].x+P_SZ-1 && 300+(Map.Bits[487]-34)*4<=plr[j].y+P_SZ-1)) {
                    plr[j].y-=4;
                }
                ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                for(Si16 i=0;i<Players;i++) {
                    if(plr[i].State<=0) continue;
                    if(j==i) continue;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                }
                if(zb) {
                    CheckFrag=1;
                    plr[j].damage=D_TELEFRAG;
                    plr[j].damageown=j;
                }
            }
        }
    }
    if(door1==1) {door1w++;
        if(door1w>5) {
            door1w=0;
            if(Map.Bits[634]==38) door1=2;
            else {Map.Bits[634]++;Map.Bits[668]++;}
        }
    }
    else if(door1==2) {door1w++;
        if(door1w>200) {door1=3; door1w=0;}
    }
    else if(door1==3) {door1w++;
        if(door1w>25) {
            door1w=0;
            if(Map.Bits[634]==34) door1=0;
            else {Map.Bits[634]--;Map.Bits[668]--;}
            for(Si16 j=0;j<Players;j++) {
                if(plr[j].State<=0) continue;
                Si16 ix,iy,zb=0;
                if((446>=plr[j].x-P_SZ+1 && 379-(Map.Bits[634]-34)*4>=plr[j].y-P_SZ+1  &&
                    446<=plr[j].x+P_SZ-1 && 379-(Map.Bits[634]-34)*4<=plr[j].y+P_SZ-1) ||
                        (453>=plr[j].x-P_SZ+1 && 379-(Map.Bits[634]-34)*4>=plr[j].y-P_SZ+1  &&
                         453<=plr[j].x+P_SZ-1 && 379-(Map.Bits[634]-34)*4<=plr[j].y+P_SZ-1)) {
                    plr[j].y+=4;
                }
                if((446>=plr[j].x-P_SZ+1 && 380+(Map.Bits[634]-34)*4>=plr[j].y-P_SZ+1  &&
                    446<=plr[j].x+P_SZ-1 && 380+(Map.Bits[634]-34)*4<=plr[j].y+P_SZ-1) ||
                        (453>=plr[j].x-P_SZ+1 && 380+(Map.Bits[634]-34)*4>=plr[j].y-P_SZ+1  &&
                         453<=plr[j].x+P_SZ-1 && 380+(Map.Bits[634]-34)*4<=plr[j].y+P_SZ-1)) {
                    plr[j].y-=4;
                }
                ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                for(Si16 i=0;i<Players;i++) {
                    if(plr[i].State<=0) continue;
                    if(j==i) continue;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                }
                if(zb) {
                    CheckFrag=1;
                    plr[j].damage=D_TELEFRAG;
                    plr[j].damageown=j;
                }
            }
        }
    }
    if(door2==1) {door2w++;
        if(door2w>5) {
            door2w=0;
            if(Map.Bits[762]==43) door2=2;
            else
            {Map.Bits[762]++;Map.Bits[763]++;}
        }
    }
    else if(door2==2) {door2w++;
        if(door2w>200) {door2=3; door2w=0;}
    }
    else if(door2==3) {door2w++;
        if(door2w>25) {
            door2w=0;
            if(Map.Bits[762]==39) door2=0;
            else {Map.Bits[762]--;Map.Bits[763]--;}
            for(Si16 j=0;j<Players;j++) {
                if(plr[j].State<=0) continue;
                Si16 ix,iy,zb=0;
                if((299-(Map.Bits[762]-39)*4>=plr[j].x-P_SZ+1 && 446>=plr[j].y-P_SZ+1  &&
                    299-(Map.Bits[762]-39)*4<=plr[j].x+P_SZ-1 && 446<=plr[j].y+P_SZ-1) ||
                        (299-(Map.Bits[762]-39)*4>=plr[j].x-P_SZ+1 && 453>=plr[j].y-P_SZ+1  &&
                         299-(Map.Bits[762]-39)*4<=plr[j].x+P_SZ-1 && 453<=plr[j].y+P_SZ-1)) {
                    plr[j].x+=4;
                }
                if((300+(Map.Bits[762]-39)*4>=plr[j].x-P_SZ+1 && 446>=plr[j].y-P_SZ+1  &&
                    300+(Map.Bits[762]-39)*4<=plr[j].x+P_SZ-1 && 446<=plr[j].y+P_SZ-1) ||
                        (300+(Map.Bits[762]-39)*4>=plr[j].x-P_SZ+1 && 453>=plr[j].y-P_SZ+1  &&
                         300+(Map.Bits[762]-39)*4<=plr[j].x+P_SZ-1 && 453<=plr[j].y+P_SZ-1)) {
                    plr[j].x-=4;
                }
                ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                for(Si16 i=0;i<Players;i++) {
                    if(plr[i].State<=0) continue;
                    if(j==i) continue;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                }
                if(zb) {
                    CheckFrag=1;
                    plr[j].damage=D_TELEFRAG;
                    plr[j].damageown=j;
                }
            }
        }
    }
    if(door3==1) {door3w++;
        if(door3w>5) {
            door3w=0;
            if(Map.Bits[392]==43) door3=2;
            else
            {Map.Bits[392]++;Map.Bits[393]++;}
        }
    }
    else if(door3==2) {door3w++;
        if(door3w>200) {door3=3; door3w=0;}
    }
    else if(door3==3) {door3w++;
        if(door3w>25) {
            door3w=0;
            if(Map.Bits[392]==39) door3=0;
            else {Map.Bits[392]--;Map.Bits[393]--;}
            for(Si16 j=0;j<Players;j++) {
                if(plr[j].State<=0) continue;
                Si16 ix,iy,zb=0;
                if((279-(Map.Bits[392]-39)*4>=plr[j].x-P_SZ+1+1 && 226>=plr[j].y-P_SZ+1+1  &&
                    279-(Map.Bits[392]-39)*4<=plr[j].x+P_SZ-1 && 226<=plr[j].y+P_SZ-1) ||
                        (279-(Map.Bits[392]-39)*4>=plr[j].x-P_SZ+1 && 233>=plr[j].y-P_SZ+1  &&
                         279-(Map.Bits[392]-39)*4<=plr[j].x+P_SZ-1 && 233<=plr[j].y+P_SZ-1)) {
                    plr[j].x+=4;
                }
                if((380+(Map.Bits[392]-39)*4>=plr[j].x-P_SZ+1 && 226>=plr[j].y-P_SZ+1  &&
                    380+(Map.Bits[392]-39)*4<=plr[j].x+P_SZ-1 && 226<=plr[j].y+P_SZ-1) ||
                        (380+(Map.Bits[392]-39)*4>=plr[j].x-P_SZ+1 && 233>=plr[j].y-P_SZ+1  &&
                         380+(Map.Bits[392]-39)*4<=plr[j].x+P_SZ-1 && 233<=plr[j].y+P_SZ-1)) {
                    plr[j].x-=4;
                }
                ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y-P_SZ+1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x     ;iy=plr[j].y+P_SZ-1; if(!FF_PlrMov(ix,iy)) zb=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y     ; if(!FF_PlrMov(ix,iy)) zb=1;
                for(Si16 i=0;i<Players;i++) {
                    if(plr[i].State<=0) continue;
                    if(j==i) continue;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                    ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1;
                    if(ix>=plr[i].x-P_SZ+1 && ix<=plr[i].x+P_SZ-1 && iy>=plr[i].y-P_SZ+1 && iy<=plr[i].y+P_SZ-1) zb=1;
                }
                if(zb) {
                    CheckFrag=1;
                    plr[j].damage=D_TELEFRAG;
                    plr[j].damageown=j;
                }
            }
        }
    }

    for(Si16 j=0;j<Players;j++) {
        if(plr[j].State<=0) continue;
        Si16 ix,iy,i;
        ix=plr[j].x/20;iy=plr[j].y/20;
        i=Map.Bits[iy*Map.Width+ix];
        if(i==24) {
            if(plr[j].x<340 && plr[j].y<340) {
                plr[j].x=630;plr[j].y=630;
                plr[j].sx=-2;plr[j].sy=-2;
                plr[j].a=A_3;
            }
            else if(plr[j].x>340 && plr[j].y>340) {
                plr[j].x=50;plr[j].y=50;
                plr[j].sx=2;plr[j].sy=2;
                plr[j].a=A_7;
            }
            else if(plr[j].x>340 && plr[j].y<340) {
                plr[j].x=50;plr[j].y=630;
                plr[j].sx=2;plr[j].sy=-2;
                plr[j].a=A_1;
            }
            else if(plr[j].x<340 && plr[j].y>340) {
                plr[j].x=630;plr[j].y=50;
                plr[j].sx=-2;plr[j].sy=2;
                plr[j].a=A_5;
            }
            for(Si16 a=16;a;a--) {
                Si16 k;
                NextFreeBlt();
                k=BltFst;
                blt[k].age=1;
                blt[k].own=j;
                blt[k].x=plr[j].x+2*cos((double)a*2*M_PI/16.);
                blt[k].y=plr[j].y-2*sin((double)a*2*M_PI/16.);
                blt[k].sx=2*cos((double)a*2*M_PI/16.);
                blt[k].sy=-2*sin((double)a*2*M_PI/16.);
                blt[k].Handler=B_SmokeHandler;
                blt[k].Draw=B_SmokeDraw;
            }
            Si16 telefrag=0;
            for(Si16 z=0;z<Players;z++) {
                if(plr[z].State<=0) continue;
                if(j==z) continue;
                telefrag=0;
                Si16 ix,iy;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                if(telefrag) {
                    CheckFrag=1;
                    plr[z].damage=D_TELEFRAG;
                    plr[z].damageown=j;
                }
            }
            MakePlayerEvent(j,PE_TELEPORT,Si16(plr[j].x),Si16(plr[j].y));
        }
    }
    SmokeGenerate--;
    if(!SmokeGenerate) {
        SmokeGenerate=2;
        Si16 j;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=650-3+Random(6); blt[j].y=650-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=30-3+Random(6); blt[j].y=650-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=650-3+Random(6); blt[j].y=30-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=30-3+Random(6); blt[j].y=30-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
    }
}
void M_Map1Handler(void)
{
    if(SmokeGenerate) SmokeGenerate--;
    else {
        SmokeGenerate=2;
        Si16 j;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=219; blt[j].y=290;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=276; blt[j].y=286;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=228; blt[j].y=258;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=267; blt[j].y=259;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=248; blt[j].y=269;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=200; blt[j].y=235;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=236; blt[j].y=234;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=257; blt[j].y=234;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=296; blt[j].y=234;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt(); j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=247; blt[j].y=206;
        blt[j].sx=(-10+Random(21))/10; blt[j].sy=(-10+Random(21))/10;
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
    }
}

void M_Map2Handler(void)
{
    for(Si16 i=0;i<Players;i++) {
        if(plr[i].State<=0) continue;
        plr[i].actwpn=B_ARROW;
    }
}

void M_Map3Handler(void)
{
	for (Si16 i = 0; i<Players; i++) {
		if (plr[i].State <= 0) continue;
		plr[i].actwpn = B_ROCKET;
		if (plr[i].ammo[B_ROCKET] < 20) {
			plr[i].ammo[B_ROCKET] = 20;
		}
	}
}

void M_Map6Handler(void)
{
    SmokeGenerate--;
    _Turn--;
    if(!SmokeGenerate) {
        SmokeGenerate=2;
        Si16 j;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=310-3+Random(6); blt[j].y=130-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=190-3+Random(6); blt[j].y=450-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=390-3+Random(6); blt[j].y=450-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=850-3+Random(6); blt[j].y=490-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
    }
    if(SmokeGenerate==1) {
        Si16 j;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=130-3+Random(6); blt[j].y=710-3+Random(6);
        blt[j].sx=-1+Random(3); blt[j].sy=-1+Random(3);
        blt[j].Handler=B_SmokeHandler; blt[j].Draw=B_SmokeDraw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=510+1; blt[j].y=830+1+Random(3);
        blt[j].sx=3+Random(3); blt[j].sy=0;
        blt[j].Handler=B_Smoke2Handler; blt[j].Draw=B_Smoke2Draw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=510+2; blt[j].y=830+Random(2);
        blt[j].sx=3+Random(3); blt[j].sy=0;
        blt[j].Handler=B_Smoke2Handler; blt[j].Draw=B_Smoke2Draw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=830+1; blt[j].y=130+1+Random(3);
        blt[j].sx=3+Random(3); blt[j].sy=0;
        blt[j].Handler=B_Smoke2Handler; blt[j].Draw=B_Smoke2Draw;
        NextFreeBlt();
        j=BltFst;
        blt[j].age=1; blt[j].own=-1;
        blt[j].x=830+2; blt[j].y=130+Random(2);
        blt[j].sx=3+Random(3); blt[j].sy=0;
        blt[j].Handler=B_Smoke2Handler; blt[j].Draw=B_Smoke2Draw;
    }
    // Vints Turning //
    if(!_Turn) {
        _Turn=14;
        BYTE *tmpimg=MpImg[37].Bits;
        MpImg[37].Bits=MpImg[38].Bits;
        MpImg[38].Bits=MpImg[39].Bits;
        MpImg[39].Bits=MpImg[40].Bits;
        MpImg[40].Bits=tmpimg;
    }
    // Teleport //
    for(Si16 j=0;j<Players;j++) {
        if(plr[j].State<=0) continue;
        Si16 ix,iy,i;
        ix=plr[j].x/20;iy=plr[j].y/20;
        i=Map.Bits[iy*Map.Width+ix];
        if(i==24) {
            if(ix==9 && iy==22) {
                switch(Random(4)) {
                case 0: plr[j].x=15;plr[j].y= 5;plr[j].sx= 0;plr[j].sy=-2; plr[j].a=A_2; break;
                case 1: plr[j].x=41;plr[j].y=23;plr[j].sx=-2;plr[j].sy=-2; plr[j].a=A_3; break;
                case 2: plr[j].x=20;plr[j].y=21;plr[j].sx= 2;plr[j].sy=-2; plr[j].a=A_1; break;
                case 3: plr[j].x= 7;plr[j].y=35;plr[j].sx= 2;plr[j].sy= 0; plr[j].a=A_0; break;
                }
                plr[j].x*=20;plr[j].y*=20;
                plr[j].x+=10;plr[j].y+=10;
            }
            else {
                plr[j].x=170;plr[j].y=450;
                plr[j].sx=-2;plr[j].sy=0;
                plr[j].a=A_4;
            }
            for(Si16 a=16;a;a--) {
                Si16 k;
                NextFreeBlt();
                k=BltFst;
                blt[k].age=1;
                blt[k].own=j;
                blt[k].x=plr[j].x+2*cos((double)a*2*M_PI/16.);
                blt[k].y=plr[j].y-2*sin((double)a*2*M_PI/16.);
                blt[k].sx=2*cos((double)a*2*M_PI/16.);
                blt[k].sy=-2*sin((double)a*2*M_PI/16.);
                blt[k].Handler=B_SmokeHandler;
                blt[k].Draw=B_SmokeDraw;
            }
            Si16 telefrag=0;
            for(Si16 z=0;z<Players;z++) {
                if(plr[z].State<=0) continue;
                if(j==z) continue;
                telefrag=0;
                Si16 ix,iy;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y+P_SZ-1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y-P_SZ+1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                ix=plr[j].x+P_SZ-1;iy=plr[j].y-P_SZ+1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                ix=plr[j].x-P_SZ+1;iy=plr[j].y+P_SZ-1;
                if(ix>=plr[z].x-P_SZ+1 && ix<=plr[z].x+P_SZ-1 && iy>=plr[z].y-P_SZ+1 && iy<=plr[z].y+P_SZ-1) telefrag=1;
                if(telefrag) {
                    CheckFrag=1;
                    plr[z].damage=D_TELEFRAG;
                    plr[z].damageown=j;
                }
            }
            MakePlayerEvent(j,PE_TELEPORT,Si16(plr[j].x),Si16(plr[j].y));
        }
    }
}

void M_MapEmptyHandler(void)
{
}

#define FIRSTITEM nn=0
#define ITEM(xx,yy,ii) \
    blt[nn].x=(xx)*20+10;blt[nn].y=(yy)*20+10;blt[nn].own=-1;blt[nn].age=1; \
    blt[nn].Draw=I_ ## ii ## Draw;blt[nn].Handler=I_ ## ii ## Handler;nn++;

void MapInit(BYTE MapNum)
{
	FILE *src;
	if ((src = fopen(ItemFileName, "rt")) == NULL)
		ExitGame("MapInit: Can't Open Item File");
	Si16 mapPos;
	fscanf(src, " %hd", &mapPos);
	MapPos = (BYTE)mapPos;
	for (Si16 i = 0; i<MapPos; i++) {
		fscanf(src, " %hd %hd %lf", MapPosX + i, MapPosY + i, MapPosA + i);
		MapPosA[i] *= M_PI / 4;
	}
	fscanf(src, " %hd", &_ItemAmount);
	FIRSTITEM;
	for (Si16 i = 0; i<_ItemAmount; i++) {
		double x; double y; char tmpstr[32];
		fscanf(src, " %lf %lf %s", &x, &y, tmpstr);
		if (strcmp(tmpstr, "Armour") == 0) { ITEM(x, y, Armour) continue; }
		if (strcmp(tmpstr, "Health") == 0) { ITEM(x, y, Health) continue; }
		if (strcmp(tmpstr, "Bow") == 0) { ITEM(x, y, Bow) continue; }
		if (strcmp(tmpstr, "Akm") == 0) { ITEM(x, y, Akm) continue; }
		if (strcmp(tmpstr, "Napalm") == 0) { ITEM(x, y, Napalm) continue; }
		if (strcmp(tmpstr, "Launcher") == 0) { ITEM(x, y, Launcher) continue; }
		if (strcmp(tmpstr, "Laser") == 0) { ITEM(x, y, Laser) continue; }
		if (strcmp(tmpstr, "Redmer") == 0) { ITEM(x, y, Redmer) continue; }
		if (strcmp(tmpstr, "Photo") == 0) { ITEM(x, y, Photo) continue; }
		if (strcmp(tmpstr, "Grenade") == 0) { ITEM(x, y, Grenade) continue; }
		if (strcmp(tmpstr, "PistolAmmo") == 0) { ITEM(x, y, PistolAmmo) continue; }
		if (strcmp(tmpstr, "BowAmmo") == 0) { ITEM(x, y, BowAmmo) continue; }
		if (strcmp(tmpstr, "AkmAmmo") == 0) { ITEM(x, y, AkmAmmo) continue; }
		if (strcmp(tmpstr, "NapalmAmmo") == 0) { ITEM(x, y, NapalmAmmo) continue; }
		if (strcmp(tmpstr, "LauncherAmmo") == 0) { ITEM(x, y, LauncherAmmo) continue; }
		if (strcmp(tmpstr, "LaserAmmo") == 0) { ITEM(x, y, LaserAmmo) continue; }
		ExitGame("MapInit: Invalid Item Name");
	}
	fclose(src);
	MapHandler = M_MapEmptyHandler;
	if (MapNum == 0) {
		MapHandler = M_Map0Handler;
		SmokeGenerate = 10;
		door0 = 0; door1 = 0; door2 = 0; door3 = 0;
		door0w = 0; door1w = 0; door2w = 0; door3w = 0;
	}
	if (MapNum == 1) {
		SmokeGenerate = 10;
		MapHandler = M_Map1Handler;
	}
	if (MapNum == 2) {
		MapHandler = M_Map2Handler;
		for (Si16 i = 0; i<8; i++) {
			Si16 j; NextFreeBlt(); j = BltFst;
			blt[j].x = 0; blt[j].y = 0;
			blt[j].age = 140; blt[j].sx = i / 4.*M_PI; blt[j].sy = 2 * M_PI / 500;
			blt[j].Handler = B_AimHandler;
			blt[j].Draw = B_AimDraw;
			NextFreeBlt(); j = BltFst;
			blt[j].x = 0; blt[j].y = 0;
			blt[j].age = 160; blt[j].sx = i / 4.*M_PI + M_PI / 8; blt[j].sy = 2 * M_PI / 800;
			blt[j].Handler = B_AimHandler;
			blt[j].Draw = B_AimDraw;
			NextFreeBlt(); j = BltFst;
			blt[j].x = 0; blt[j].y = 0;
			blt[j].age = 180; blt[j].sx = i / 4.*M_PI; blt[j].sy = 2 * M_PI / 1000;
			blt[j].Handler = B_AimHandler;
			blt[j].Draw = B_AimDraw;
		}
	}
	if (MapNum == 3) {
		MapHandler = M_Map3Handler;
		Si16 j; NextFreeBlt(); j = BltFst;
		blt[j].x = 200; blt[j].y = 200;
		blt[j].age = 1; blt[j].sx = 0; blt[j].sy = 0;
		blt[j].Handler = B_BallHandler;
		blt[j].Draw = B_BallDraw;
	}
	if (MapNum == 6) {
		_Turn = 20;
		SmokeGenerate = 10;
		MapHandler = M_Map6Handler;
	}
}
#undef ITEM
