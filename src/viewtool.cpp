/*
2D Quake v1.04
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/

#include "defs.h"

#include <math.h>

#include "defs.h"
#include "2dqdefs.h"
#include "game.h"

#define vprt      ((VIEWPORT*)v)

BYTE OneBorder(Si16 x1,Si16 y1,Si16 x2,Si16 y2)
{
    Si16 x = x2-x1;
    Si16 y = y2-y1;
    Si16 sx = sgn(x);
    Si16 sy = sgn(y);
    Si16 dx = ABS(x);
    Si16 dy = ABS(y);

	x = x1;
	y = y1;

    Si16 Bound, Count=0;
    if (dx >= dy) {
		Bound = (dx + 1) >> 1;
        for (Si16 i = 0; i <= dx; i++) {
            if (!FF_PlrMov(x,y)) {
				return 0;
			}
			Count += dy;
            if (Count >= Bound) {
				y += sy;
				Count -= dx;
			}
            x+=sx;
        }
    } else {
		Bound = (dy + 1) >> 1;
        for (Si16 i=0; i <= dy; i++) {
            if (!FF_PlrMov(x,y)) {
				return 0;
			}
			Count += dx;
            if (Count >= Bound) {
				x += sx;
				Count -= dy;
			}
            y += sy;
        }
    }
    return 1;
}
BYTE Border(Si16 x1,Si16 y1,Si16 x2,Si16 y2)
{
    BYTE a=0;
	a += OneBorder(x1 - P_SZ, y1 - P_SZ, x2 - P_SZ, y2 - P_SZ);
	a += OneBorder(x1 + P_SZ, y1 - P_SZ, x2 + P_SZ, y2 - P_SZ);
	a += OneBorder(x1 - P_SZ, y1 + P_SZ, x2 - P_SZ, y2 + P_SZ);
	a += OneBorder(x1 + P_SZ, y1 + P_SZ, x2 + P_SZ, y2 + P_SZ);
	if (a == 4) {
		return 1;
	} else {
		return 0;
	}
}

BYTE FireLine(Si16 x1,Si16 y1,Si16 x2,Si16 y2)
{
    Si16 x,y;
    x = x2-x1;
    y = y2-y1;
    Si16 sx = sgn(x);
    Si16 sy = sgn(y);
    Si16 dx = ABS(x);
    Si16 dy = ABS(y);

    x=x1; y=y1;

    Si16 Bound, Count=0;
    if(dx>=dy)
    {
        Bound=(dx+1)>>1;
        for(Si16 i=0;i<=dx;i++)
        {
            if(!FF_BltMov(x,y)) {return 0;}
            Count+=dy;
            if(Count>=Bound) {y+=sy; Count-=dx;}
            x+=sx;
        }
    }
    else
    {
        Bound=(dy+1)>>1;
        for(Si16 i=0;i<=dy;i++)
        {
            if(!FF_BltMov(x,y)) {return 0;}
            Count+=dx;
            if(Count>=Bound) { x+=sx; Count-=dy; }
            y+=sy;
        }
    }
    return 1;
}

BYTE Blast(Si16 x1,Si16 y1,Si16 x2,Si16 y2,Si16 range)
{
    if(ABS(x2-x1)+ABS(y2-y1)>range)
        return 0;

    return FireLine(x1,y1,x2,y2);
}

BYTE RedmerVisible(double x1,double y1,double a1,double x2,double y2)
{
    if(x2>x1-V_SZ && x2<x1+V_SZ &&
            y2>y1-V_SZ && y2<y1+V_SZ)
        return 1;
    if(x2<x1-V_SZ2 || x2>x1+V_SZ2 ||
            y2<y1-V_SZ2 || y2>y1+V_SZ2)
        return 0;

    double angle;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;

    if((angle>a1+V_FOV        || angle<a1-V_FOV       ) &&
            (angle>a1+V_FOV+2*M_PI || angle<a1-V_FOV+2*M_PI) &&
            (angle>a1+V_FOV-2*M_PI || angle<a1-V_FOV-2*M_PI))
        return 0;

    Si16 x,y;

    x = Si16(x2-x1);
    y = Si16(y2-y1);
    Si16 sx = sgn(x);
    Si16 sy = sgn(y);
    Si16 dx = abs(x);
    Si16 dy = abs(y);

    x=Si16(x1); y=Si16(y1);

    Si16 Bound, Count=0;
    if(dx>=dy)
    {
        Bound=(dx+1)>>1;
        for(Si16 i=0;i<=dx;i++)
        {
            if(!FF_PlrSee(x,y)) {return 0;}
            Count+=dy;
            if(Count>=Bound) {y+=sy; Count-=dx;}
            x+=sx;
        }
    }
    else
    {
        Bound=(dy+1)>>1;
        for(Si16 i=0;i<=dy;i++)
        {
            if(!FF_PlrSee(x,y)) {return 0;}
            Count+=dx;
            if(Count>=Bound) { x+=sx; Count-=dy; }
            y+=sy;
        }
    }
    return 1;
}
BYTE Visible(BYTE a,double x2,double y2)
{
#define a1 plr[a].a
#define x1 plr[a].x
#define y1 plr[a].y
    if(plr[a].wpn[B_REDMER]>1)
    {
        if(RedmerVisible(blt[plr[a].wpn[B_REDMER]-2].x,blt[plr[a].wpn[B_REDMER]-2].y,a1,x2,y2)) return 1;
    }

    if(x2>x1-V_SZ && x2<x1+V_SZ &&
            y2>y1-V_SZ && y2<y1+V_SZ)
        return 1;
    if(x2<x1-V_SZ2 || x2>x1+V_SZ2 ||
            y2<y1-V_SZ2 || y2>y1+V_SZ2)
        return 0;

    double angle;
    if((Si16)x2==(Si16)x1) {
        if(y2<y1) angle=M_PI/2;
        else angle=M_PI/2+M_PI;
    }
    else {angle=atan2(y1-y2,x2-x1);}
    if(angle<0) angle+=2*M_PI;

    if((angle>a1+V_FOV        || angle<a1-V_FOV       ) &&
            (angle>a1+V_FOV+2*M_PI || angle<a1-V_FOV+2*M_PI) &&
            (angle>a1+V_FOV-2*M_PI || angle<a1-V_FOV-2*M_PI))
        return 0;

    Si16 x,y;

    x = Si16(x2-x1);
    y = Si16(y2-y1);
    Si16 sx = sgn(x);
    Si16 sy = sgn(y);
    Si16 dx = abs(x);
    Si16 dy = abs(y);

    x=Si16(x1); y=Si16(y1);

    Si16 Bound, Count=0;
    if(dx>=dy)
    {
        Bound=(dx+1)>>1;
        for(Si16 i=0;i<=dx;i++)
        {
            if(!FF_PlrSee(x,y)) {return 0;}
            Count+=dy;
            if(Count>=Bound) {y+=sy; Count-=dx;}
            x+=sx;
        }
    }
    else
    {
        Bound=(dy+1)>>1;
        for(Si16 i=0;i<=dy;i++)
        {
            if(!FF_PlrSee(x,y)) {return 0;}
            Count+=dx;
            if(Count>=Bound) { x+=sx; Count-=dy; }
            y+=sy;
        }
    }
    return 1;
#undef x1
#undef y1
#undef a1
}

void CenterFocusing(void *v)
{
    if(plr[vprt->ActPlayer].wpn[B_REDMER]>1) {
        vprt->x=Si16(blt[plr[vprt->ActPlayer].wpn[B_REDMER]-2].x-(vprt->scr_x2-vprt->scr_x1)/2);
        vprt->y=Si16(blt[plr[vprt->ActPlayer].wpn[B_REDMER]-2].y-(vprt->scr_y2-vprt->scr_y1)/2);
    }
    else {
        vprt->x=Si16(plr[vprt->ActPlayer].x-(vprt->scr_x2-vprt->scr_x1)/2);
        vprt->y=Si16(plr[vprt->ActPlayer].y-(vprt->scr_y2-vprt->scr_y1)/2);
    }
}
void FarFocusing(void *v)
{
#ifdef _TEST
    vprt->x=plr[1].x-(vprt->scr_x2-vprt->scr_x1)/2;
    vprt->y=plr[1].y-(vprt->scr_y2-vprt->scr_y1)/2;
#else
    if(plr[vprt->ActPlayer].wpn[B_REDMER]>1) {
        vprt->x=Si16(blt[plr[vprt->ActPlayer].wpn[B_REDMER]-2].x-(vprt->scr_x2-vprt->scr_x1)/2
                +(Si16)(70*cos(plr[vprt->ActPlayer].a)));
        vprt->y=Si16(blt[plr[vprt->ActPlayer].wpn[B_REDMER]-2].y-(vprt->scr_y2-vprt->scr_y1)/2
                -(Si16)(70*sin(plr[vprt->ActPlayer].a)));
    }
    else {
        vprt->x=Si16(plr[vprt->ActPlayer].x+(Si16)(70*cos(plr[vprt->ActPlayer].a))-(vprt->scr_x2-vprt->scr_x1)/2);
        vprt->y=Si16(plr[vprt->ActPlayer].y-(Si16)(70*sin(plr[vprt->ActPlayer].a))-(vprt->scr_y2-vprt->scr_y1)/2);
    }
#endif
}
