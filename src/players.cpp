/*
2D Quake v1.02
Copyleft by bladez-fate
20 Jan 2003 - 7 July 2003
*/

#include "defs.h"

#include <math.h>

#include "defs.h"
#include "2dqdefs.h"
#include "graph.h"
#include "keyboard.h"
#include "game.h"

void C_Demo(PLAYER *pl)
{
	if (KF(kKeyTab)) {
		KeyDown(kKeyTab);
		vp[0].ActPlayer = (vp[0].ActPlayer + 1) % Players;
		vp[0].NoFocusing = (plr[vp[0].ActPlayer].State < 0);
	}
}

void C_KMBattle(PLAYER *pl)
{
	if (KF('W')) { pl->cmnd[C_UP] = 1; }
	if (KF('S')) { pl->cmnd[C_DN] = 1; }
	if (KF('A')) { pl->cmnd[C_LT] = 1; }
	if (KF('D')) { pl->cmnd[C_RT] = 1; }

	if (KF(kKeyMouseRight)) vp[0].ShowNames = 1;
	
	if (KF(kKeyMouseLeft)) { pl->cmnd[C_FIR] = 1; }
	if (KF('Q')) { pl->cmnd[C_PRV] = 1; KeyDown('Q'); }
	if (KF('E')) { pl->cmnd[C_NXT] = 1; KeyDown('E'); }
	if (KF('F')) { pl->cmnd[C_USE] = 1; KeyDown('F'); }
	//if (KF(cfg.Keys1[7])) { pl->cmnd[C_TRN] = 1; KeyDown(cfg.Keys1[7]); }
	//if (KF('C')) {
	//	KeyDown('C');
	//	if (vp[0].Focusing == CenterFocusing) vp[0].Focusing = FarFocusing;
	//	else vp[0].Focusing = CenterFocusing;
	//}

	// Always center on player
	vp[0].Focusing = CenterFocusing;

	// Always use sniper mode
	if (pl->trn != P_TRN2) {
		pl->cmnd[C_TRN] = 1;
	}

	// Obtain world coordinates of mouse cursor
	Vec2Si32 m = MousePos();
	m.y = screen::h - m.y; // invert Oy axis
	//Vec2Si32 wp = vp[0].Screen2World(m);
	Si16 x2 = m.x;
	Si16 y2 = m.y;

	// Look to that point
	Si16 x1 = vp[0].ScreenCenter().x;
	Si16 y1 = vp[0].ScreenCenter().y;
	double a1 = pl->a, angle;
	Si16 Fov = 2;
	if ((Si16)x2 == (Si16)x1) {
		if (y2 < y1) angle = M_PI / 2;
		else angle = M_PI / 2 + M_PI;
	}
	else { angle = atan2(y1 - y2, x2 - x1); }
	if (angle < 0) angle += 2 * M_PI;

	// Use commands for turning
	double z = angle - a1;
	if (z < 0) z += 2 * M_PI;
	if (z < P_TRN2*Fov || z>2 * M_PI - P_TRN2*Fov)
		return;
	if (z > 0 && z <= M_PI) pl->cmnd[C_A_P] = 1;
	else if (z<2 * M_PI && z>M_PI) pl->cmnd[C_A_M] = 1;

	// Just hack for player to be always faced to mouse cursor
	pl->a = angle;
}

void C_K1Battle(PLAYER *pl)
{
	if (KF(cfg.Keys1[0])) { pl->cmnd[C_ACC] = 1; }
	if (KF(cfg.Keys1[1])) { pl->cmnd[C_BRK] = 1; }
	if (KF(cfg.Keys1[4])) { pl->cmnd[C_LFT] = 1; }
	if (KF(cfg.Keys1[5])) { pl->cmnd[C_RHT] = 1; }
	if (KF(cfg.Keys1[2])) { pl->cmnd[C_A_P] = 1; pl->cmnd[C_LT] = 1; }
	if (KF(cfg.Keys1[3])) { pl->cmnd[C_A_M] = 1; pl->cmnd[C_RT] = 1; 
							if (pl->cmnd[C_A_P]) vp[0].ShowNames = 1; }
	if (KF(cfg.Keys1[6])) { pl->cmnd[C_FIR] = 1; }
	if (KF(cfg.Keys1[9])) { pl->cmnd[C_PRV] = 1; KeyDown(cfg.Keys1[9]); }
	if (KF(cfg.Keys1[8])) { pl->cmnd[C_NXT] = 1; KeyDown(cfg.Keys1[8]); }
	if (KF(cfg.Keys1[10])) { pl->cmnd[C_USE] = 1; KeyDown(cfg.Keys1[10]); }
	if (KF(cfg.Keys1[7])) { pl->cmnd[C_TRN] = 1; KeyDown(cfg.Keys1[7]); }
	if (KF(cfg.Keys1[11])) {
		KeyDown(cfg.Keys1[11]);
		if (vp[0].Focusing == CenterFocusing) vp[0].Focusing = FarFocusing;
		else vp[0].Focusing = CenterFocusing;
	}
}

void C_K2Battle(PLAYER *pl)
{
	if (KF(cfg.Keys2[0])) { pl->cmnd[C_ACC] = 1; }
	if (KF(cfg.Keys2[1])) { pl->cmnd[C_BRK] = 1; }
	if (KF(cfg.Keys2[4])) { pl->cmnd[C_LFT] = 1; }
	if (KF(cfg.Keys2[5])) { pl->cmnd[C_RHT] = 1; }
	if (KF(cfg.Keys2[2])) { pl->cmnd[C_A_P] = 1; }
	if (KF(cfg.Keys2[3])) { pl->cmnd[C_A_M] = 1; 
							if (pl->cmnd[C_A_P]) vp[1].ShowNames = 1; }
	if (KF(cfg.Keys2[6])) { pl->cmnd[C_FIR] = 1; }
	if (KF(cfg.Keys2[9])) { pl->cmnd[C_PRV] = 1; KeyDown(cfg.Keys2[9]); }
	if (KF(cfg.Keys2[8])) { pl->cmnd[C_NXT] = 1; KeyDown(cfg.Keys2[8]); }
	if (KF(cfg.Keys2[10])) { pl->cmnd[C_USE] = 1; KeyDown(cfg.Keys2[10]); }
	if (KF(cfg.Keys2[7])) { pl->cmnd[C_TRN] = 1; KeyDown(cfg.Keys2[7]); }
	if (KF(cfg.Keys2[11])) {
		KeyDown(cfg.Keys2[11]);
		if (vp[1].Focusing == CenterFocusing) vp[1].Focusing = FarFocusing;
		else vp[1].Focusing = CenterFocusing;
	}
}

void M_NoMove(PLAYER *) {}

void M_Battle(PLAYER *pl)
{
	Si16 ix, iy;
	BYTE xb = 0, yb = 0;

	ix = Si16(pl->x + pl->sx + P_SZ); iy = Si16(pl->y + P_SZ); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx - P_SZ); iy = Si16(pl->y + P_SZ); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx - P_SZ); iy = Si16(pl->y - P_SZ); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx + P_SZ); iy = Si16(pl->y); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx + P_SZ); iy = Si16(pl->y - P_SZ); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx); iy = Si16(pl->y - P_SZ); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx); iy = Si16(pl->y + P_SZ); if (!FF_PlrMov(ix, iy)) xb = 1;
	ix = Si16(pl->x + pl->sx - P_SZ); iy = Si16(pl->y); if (!FF_PlrMov(ix, iy)) xb = 1;
	for (Si16 i = 0; i < Players; i++) {
		if (plr[i].State <= 0 || pl->n == i) continue;
		ix = Si16(pl->x + pl->sx * 2 + P_SZ); iy = Si16(pl->y + P_SZ);
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) xb = 1;
		ix = pl->x + pl->sx * 2 - P_SZ; iy = pl->y - P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) xb = 1;
		ix = pl->x + pl->sx * 2 + P_SZ; iy = pl->y - P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) xb = 1;
		ix = pl->x + pl->sx * 2 - P_SZ; iy = pl->y + P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) xb = 1;
	}
	if (!xb) pl->x = pl->x + pl->sx;

	ix = pl->x + P_SZ; iy = pl->y + pl->sy + P_SZ; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x + P_SZ; iy = pl->y + pl->sy - P_SZ; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x - P_SZ; iy = pl->y + pl->sy + P_SZ; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x - P_SZ; iy = pl->y + pl->sy - P_SZ; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x + P_SZ; iy = pl->y + pl->sy; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x; iy = pl->y + pl->sy - P_SZ; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x; iy = pl->y + pl->sy + P_SZ; if (!FF_PlrMov(ix, iy)) yb = 1;
	ix = pl->x - P_SZ; iy = pl->y + pl->sy; if (!FF_PlrMov(ix, iy)) yb = 1;
	for (Si16 i = 0; i < Players; i++) {
		if (plr[i].State <= 0 || pl->n == i) continue;
		ix = pl->x + P_SZ; iy = pl->y + pl->sy * 2 + P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) yb = 1;
		ix = pl->x - P_SZ; iy = pl->y + pl->sy * 2 - P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) yb = 1;
		ix = pl->x + P_SZ; iy = pl->y + pl->sy * 2 - P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) yb = 1;
		ix = pl->x - P_SZ; iy = pl->y + pl->sy * 2 + P_SZ;
		if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) yb = 1;
	}
	if (!yb) pl->y = pl->y + pl->sy;

	pl->sx = pl->sx*0.88;
	pl->sy = pl->sy*0.88;
	if (pl->firefrm) pl->firefrm--;
	if (pl->damgfrm) pl->damgfrm--;
	if (pl->reload) pl->reload--;
}

void MC_Dead(PLAYER *pl)
{
	pl->reload--;
	DrawWeapon[pl->n] = SHOW_WPN + 1;

	if (!pl->reload || (pl->cmnd[C_FIR] && pl->reload < DEAD_TIME - NEEDDEAD_TIME)) {
		pl->Move = M_Battle;
		pl->MakeCmnd = MC_Battle;
		pl->reload = 0;
		pl->State = PS_BATTLE;
		if (pl->n == vp[0].ActPlayer) { vp[0].NoFocusing = 0; }
		if (pl->n == vp[1].ActPlayer) { vp[1].NoFocusing = 0; }
		DrawWeapon[pl->n] = SHOW_WPN;

		Si16 X = pl->x, Y = pl->y, checkpos = 0;
		Si16 i = 0;
		for (; i < Players; i++) {
			if (pl->n == i || plr[i].State <= 0) continue;
			Si16 ix, iy;
			ix = X + P_SZ; iy = Y + P_SZ;
			if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) { checkpos = 1; break; }
			ix = X - P_SZ; iy = Y - P_SZ;
			if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) { checkpos = 1; break; }
			ix = X + P_SZ; iy = Y - P_SZ;
			if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) { checkpos = 1; break; }
			ix = X - P_SZ; iy = Y + P_SZ;
			if (ix >= plr[i].x - P_SZ && ix <= plr[i].x + P_SZ && iy >= plr[i].y - P_SZ && iy <= plr[i].y + P_SZ) { checkpos = 1; break; }
		}
		if (checkpos) {
			plr[i].damage += D_TELEFRAG;
			plr[i].damageown = pl->n;
			CheckFrag = 1;
		}
	}
}

void MC_Battle(PLAYER *pl)
{
	int xdir = 0;
	int ydir = 0;
	if (pl->cmnd[C_UP]) {
		ydir--;
	}
	if (pl->cmnd[C_DN]) {
		ydir++;
	}
	if (pl->cmnd[C_LT]) {
		xdir--;
	}
	if (pl->cmnd[C_RT]) {
		xdir++;
	}
	if (xdir !=0 || ydir != 0) {
		if (xdir != 0 && ydir != 0) { // diagonals
			pl->sx += xdir * pl->acc * M_SQRT1_2;
			pl->sy += ydir * pl->acc * M_SQRT1_2;
		}
		else { // non-diagonals
			pl->sx += xdir * pl->acc;
			pl->sy += ydir * pl->acc;
		}

		// Select right moving frame
		double prod = xdir*cos(pl->a) + ydir*sin(pl->a); // scalar product
		if (prod > 0) { // moving forward
			pl->movefrm = (pl->movefrm + 1) % (4 * M_STEPLEN);
		} 
		else if (prod < 0) {
			pl->movefrm = (pl->movefrm + 4 * M_STEPLEN - 1) % (4 * M_STEPLEN);
		}
		else {
			pl->movefrm = 0;
		}
	}
	else {
		switch (pl->cmnd[C_ACC] + 2 * pl->cmnd[C_BRK] + 4 * pl->cmnd[C_LFT] + 8 * pl->cmnd[C_RHT]) {
		case 1: case 13: // Forward
			pl->sx += cos(pl->a)*pl->acc; pl->sy -= sin(pl->a)*pl->acc;
			pl->movefrm = (pl->movefrm + 1) % (4 * M_STEPLEN);
			break;
		case 14: case 2: // Back
			pl->sx -= cos(pl->a)*pl->acc; pl->sy += sin(pl->a)*pl->acc; 
			pl->movefrm = (pl->movefrm + 4 * M_STEPLEN - 1) % (4 * M_STEPLEN);
			break;
		case 4: case 7: // Left
			pl->sx += cos(pl->a + M_PI / 2)*pl->acc;
			pl->sy -= sin(pl->a + M_PI / 2)*pl->acc;
			pl->movefrm = 0;
			break;
		case 8: case 11: // Right
			pl->sx += cos(pl->a - M_PI / 2)*pl->acc;
			pl->sy -= sin(pl->a - M_PI / 2)*pl->acc;
			pl->movefrm = 0;
			break;
		case 9: // Right-Forward
			pl->sx += cos(pl->a - M_PI / 4)*pl->acc;
			pl->sy -= sin(pl->a - M_PI / 4)*pl->acc;
			pl->movefrm = (pl->movefrm + 1) % (4 * M_STEPLEN);
			break;
		case 5: // Left-Forward
			pl->sx += cos(pl->a + M_PI / 4)*pl->acc;
			pl->sy -= sin(pl->a + M_PI / 4)*pl->acc;
			pl->movefrm = (pl->movefrm + 1) % (4 * M_STEPLEN);
			break;
		case 10: // Right-Back
			pl->sx += cos(pl->a - 3 * M_PI / 4)*pl->acc;
			pl->sy -= sin(pl->a - 3 * M_PI / 4)*pl->acc;
			pl->movefrm = (pl->movefrm + 4 * M_STEPLEN - 1) % (4 * M_STEPLEN);
			break;
		case 6: // Left-Back
			pl->sx += cos(pl->a + 3 * M_PI / 4)*pl->acc;
			pl->sy -= sin(pl->a + 3 * M_PI / 4)*pl->acc; 
			pl->movefrm = (pl->movefrm + 4 * M_STEPLEN - 1) % (4 * M_STEPLEN);
			break;
		default: pl->movefrm = 0; break;
		}
	}
	if (pl->cmnd[C_A_P]) { pl->a = pl->a + pl->trn; if (pl->a >= M_PI * 2)pl->a -= 2 * M_PI; }
	if (pl->cmnd[C_A_M]) { pl->a = pl->a - pl->trn; if (pl->a < 0)pl->a += 2 * M_PI; }
	if (pl->cmnd[C_TRN]) { if (pl->trn == P_TRN) pl->trn = P_TRN2; else pl->trn = P_TRN; }
	if (pl->cmnd[C_USE]) {}
	if (pl->cmnd[C_FIR])
	{
		if (pl->reload == 0 && pl->actwpn == B_GRENADE) {
			if (pl->wpn[B_GRENADE] == 1 && pl->ammo[B_GRENADE]) {
				pl->ammo[B_GRENADE]--;
				DrawStatistics = 1;
			}
			if (pl->wpn[B_GRENADE] - 1 < A_GRENADE) pl->wpn[B_GRENADE]++;
		}
		else if (pl->reload == 0 && pl->ammo[pl->actwpn]) {
			if (pl->actwpn == B_LASER) {
				if (pl->wpn[B_LASER] == 1) {
					DrawStatistics = 1;
					if (pl->ammo[B_LASER] >= D_LASER / 4)
					{
						pl->wpn[B_LASER] = D_LASER + 1; pl->ammo[B_LASER] -= D_LASER / 4;
					}
					else
					{
						pl->wpn[B_LASER] = pl->ammo[B_LASER] * 4 + 1; pl->ammo[B_LASER] = 0;
					}
				}
				else if (pl->wpn[B_LASER] > 1 && pl->wpn[B_LASER] - 1 < MD_LASER)
				{
					DrawStatistics = 1; pl->wpn[B_LASER]++; if (!(pl->wpn[B_LASER] % 4))pl->ammo[B_LASER]--;
				}
			}
			else {
				Si16 i;
				NextFreeBlt();
				i = BltFst;
				DrawStatistics = 1;
				blt[i].age = 1;
				blt[i].own = pl->n;
				blt[i].x = pl->x + WPN_RANGE*cos(pl->a);
				blt[i].y = pl->y - WPN_RANGE*sin(pl->a);
				switch (pl->actwpn) {
				case B_SHELL:
					pl->firefrm = F_LEN;
					pl->ammo[B_SHELL]--;
					blt[i].sx = S_SHELL*cos(pl->a);
					blt[i].sy = -S_SHELL*sin(pl->a);
					pl->reload = R_SHELL;
					blt[i].Handler = B_ShellHandler;
					blt[i].Draw = B_ShellDraw;
					break;
				case B_ARROW:
					pl->firefrm = F_LEN;
					pl->ammo[B_ARROW]--;
					blt[i].sx = S_ARROW*cos(pl->a);
					blt[i].sy = -S_ARROW*sin(pl->a);
					pl->reload = R_ARROW;
					blt[i].Handler = B_ArrowHandler;
					blt[i].Draw = B_ArrowDraw;
					break;
				case B_AKM:
					pl->firefrm = R_AKM / 2;
					pl->ammo[B_AKM]--;
					blt[i].sx = S_AKM*cos(pl->a);
					blt[i].sy = -S_AKM*sin(pl->a);
					pl->reload = R_AKM;
					blt[i].Handler = B_ShellHandler;
					blt[i].Draw = B_ShellDraw;
					break;
				case B_NAPALM:
					pl->firefrm = F_LEN;
					pl->ammo[B_NAPALM]--;
					blt[i].sx = S_NAPALM*cos(pl->a);
					blt[i].sy = -S_NAPALM*sin(pl->a);
					pl->reload = R_NAPALM;
					blt[i].Handler = B_NapalmHandler;
					blt[i].Draw = B_NapalmDraw;
					break;
				case B_ROCKET:
					pl->firefrm = F_LEN;
					pl->ammo[B_ROCKET]--;
					blt[i].sx = S_ROCKET*cos(pl->a);
					blt[i].sy = -S_ROCKET*sin(pl->a);
					pl->reload = R_ROCKET;
					blt[i].Handler = B_RocketHandler;
					blt[i].Draw = B_RocketDraw;
					break;
				case B_PHOTO: {
					Si16 f = Si16(blt[i].y / 20)*Map.Width + Si16(blt[i].x / 20);
					Si16 ii = Map.Bits[f];
					if ((ii < 18 || ii>23) && ii != 28 && !FF_PlrMov(blt[i].x, blt[i].y)) {
						pl->reload = R_PHOTO;
						pl->ammo[B_PHOTO]--;
						if ((pl->a > A_7 || pl->a <= A_1) && (Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f - 1
							|| Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f)) {
							blt[i].Handler = B_PhotoA0Handler;
							blt[i].Draw = B_PhotoHorzDraw;
							blt[i].x = Si16(blt[i].x);
							for (Si16 k = 0; k < 20; k++) {
								if (FF_PlrMov(blt[i].x, blt[i].y)) break;
								blt[i].x--;
							}
						}
						else if (pl->a > A_1 && pl->a <= A_3 && (Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f + Map.Width
							|| Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f)) {
							blt[i].Handler = B_PhotoA2Handler;
							blt[i].Draw = B_PhotoVertDraw;
							blt[i].y = Si16(blt[i].y);
							for (Si16 k = 0; k < 20; k++) {
								if (FF_PlrMov(blt[i].x, blt[i].y)) break;
								blt[i].y++;
							}
						}
						else if (pl->a > A_3 && pl->a <= A_5 && (Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f + 1
							|| Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f)) {
							blt[i].Handler = B_PhotoA4Handler;
							blt[i].Draw = B_PhotoHorzDraw;
							blt[i].x = Si16(blt[i].x);
							for (Si16 k = 0; k < 20; k++) {
								if (FF_PlrMov(blt[i].x, blt[i].y)) break;
								blt[i].x++;
							}
						}
						else if (pl->a > A_5 && pl->a <= A_7 && (Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f - Map.Width
							|| Si16(pl->y / 20)*Map.Width + Si16(pl->x / 20) == f)) {
							blt[i].Handler = B_PhotoA6Handler;
							blt[i].Draw = B_PhotoVertDraw;
							blt[i].y = Si16(blt[i].y);
							for (Si16 k = 0; k < 20; k++) {
								if (FF_PlrMov(blt[i].x, blt[i].y)) break;
								blt[i].y--;
							}
						}
						else { blt[i].age = 0; pl->ammo[B_PHOTO]++; }
					}
					else blt[i].age = 0;
					if (!pl->ammo[B_PHOTO]) { pl->wpn[B_PHOTO] = 0; }
					break;
				}
				case B_REDMER:
					pl->ammo[B_REDMER]--;
					blt[i].sx = S_REDMER*cos(pl->a);
					blt[i].sy = -S_REDMER*sin(pl->a);
					pl->reload = R_REDMER;
					blt[i].Handler = B_RedmerHandler;
					blt[i].Draw = B_RedmerDraw;
					pl->wpn[pl->actwpn] = 2 + i; // RedmerControl: ON
				}
			}
		}
		if (!pl->ammo[pl->actwpn] && pl->actwpn != B_LASER && pl->actwpn != B_GRENADE) {
			DrawWeapon[pl->n] = SHOW_WPN;
			BYTE maxprior = 0;
			for (Si16 i = 0; i < M_WPN; i++) {
				if (pl->wpn[i] && pl->ammo[i] && PlrWpnPrior[i] > maxprior)
				{
					pl->actwpn = i; maxprior = PlrWpnPrior[i];
				}
			}
			plrevn[pl->n].Code[plrevn[pl->n].Length] = PE_NOAMMO;
			plrevn[pl->n].Length += 1;
		}
	}
	else
	{ // Change Weapon & FIRE for laser & grenade //
		Si16 TryChange = 0;
		if (pl->actwpn == B_LASER && pl->wpn[B_LASER] > 1) {
			Si16 i;
			NextFreeBlt();
			i = BltFst;
			DrawStatistics = 1;
			pl->reload = R_LASER;
			pl->firefrm = F_LEN;
			blt[i].age = pl->wpn[B_LASER] - 1;
			blt[i].own = pl->n;
			blt[i].x = pl->x + WPN_RANGE*cos(pl->a);
			blt[i].y = pl->y - WPN_RANGE*sin(pl->a);
			blt[i].sx = S_LASER*cos(pl->a);
			blt[i].sy = -S_LASER*sin(pl->a);
			blt[i].Handler = B_LaserHandler;
			blt[i].Draw = B_LaserDraw;
			TryChange = 1;
			pl->wpn[B_LASER] = 1;
		}
		else if (pl->actwpn == B_GRENADE && pl->wpn[B_GRENADE] > 1) {
			Si16 i;
			NextFreeBlt();
			i = BltFst;
			DrawStatistics = 1;
			pl->reload = R_GRENADE;
			blt[i].age = pl->wpn[B_GRENADE] - 1;
			blt[i].own = pl->n;
			blt[i].x = pl->x + WPN_RANGE*cos(pl->a);
			blt[i].y = pl->y - WPN_RANGE*sin(pl->a);
			blt[i].sx = SP_GRENADE * pl->sx + S_GRENADE*cos(pl->a);
			blt[i].sy = SP_GRENADE * pl->sy - S_GRENADE*sin(pl->a);
			if (pl->cmnd[C_ACC]) {
				blt[i].sx += (fabs(pl->sx) < 1) ? (pl->sx * 6) : (sgn(pl->sx) * 6);
				blt[i].sy += (fabs(pl->sy) < 1) ? (pl->sy * 6) : (sgn(pl->sy) * 6);
			}
			if (pl->cmnd[C_BRK]) { blt[i].sx = 0; blt[i].sy = 0; }
			pl->reload = R_GRENADE;
			blt[i].Handler = B_GrenadeHandler;
			blt[i].Draw = B_GrenadeDraw;
			TryChange = 1;
			if (!pl->ammo[B_GRENADE]) pl->wpn[B_GRENADE] = 0;
			else pl->wpn[B_GRENADE] = 1;
		}
		if (TryChange && !pl->ammo[pl->actwpn]) {
			DrawWeapon[pl->n] = SHOW_WPN;
			BYTE maxprior = 0;
			for (Si16 i = 0; i < M_WPN; i++) {
				if (pl->wpn[i] && pl->ammo[i] && PlrWpnPrior[i] > maxprior)
				{
					pl->actwpn = i; maxprior = PlrWpnPrior[i];
				}
			}
			plrevn[pl->n].Code[plrevn[pl->n].Length] = PE_NOAMMO;
			plrevn[pl->n].Length += 1;
		}
		if (pl->cmnd[C_NXT]) {
			DrawWeapon[pl->n] = SHOW_WPN;
			pl->actwpn = (pl->actwpn + 1) % M_WPN;
			while (!pl->wpn[pl->actwpn]) {
				pl->actwpn = (pl->actwpn + 1) % M_WPN;
			}
			DrawStatistics = 1;
		}

		if (pl->cmnd[C_PRV]) {
			DrawWeapon[pl->n] = SHOW_WPN;
			pl->actwpn = (pl->actwpn + M_WPN - 1) % M_WPN;
			while (!pl->wpn[pl->actwpn]) {
				pl->actwpn = (pl->actwpn + M_WPN - 1) % M_WPN;
			}
			DrawStatistics = 1;
			if (pl->cmnd[C_A_M]) {
				DrawWeapon[pl->n] = SHOW_WPN + 1;
			}
		}
	}
	/* Player See */
	{
		Si16 j = 0;
		for (; j < Players; j++) {
			if (pl->n == j || plr[j].State <= 0) continue;
			if (Visible(pl->n, plr[j].x, plr[j].y)) {
				MakePlayerEvent(pl->n, PE_SEEPLR, (BYTE)j);
			}
		}
		for (j = 0; j < BltExt; j++) {
			if (!blt[j].age) continue;
			BYTE vis = Visible(pl->n, blt[j].x, blt[j].y);
			if (vis) {
				/* && blt[j].Handler!=B_SmokeHandler
			 && blt[j].Handler!=B_BloodHandler) {*/
				MakePlayerEvent(pl->n, PE_SEEBLT, j);
			}
		}
	}
}

