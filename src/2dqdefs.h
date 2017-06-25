#pragma once

/*
2D Quake v1.02
Copyleft by bladez-fate
10 July 2002 - 4 Jan 2003
*/

#include "graph.h"

/* Command Defines */
#define C_ACC     0
#define C_BRK     1
#define C_A_P     2
#define C_A_M     3
#define C_LFT     4
#define C_RHT     5
#define C_FIR     6
#define C_TRN     7
#define C_NXT     8
#define C_PRV     9
#define C_USE     10
#define C_UP      11
#define C_DN      12
#define C_LT      13
#define C_RT      14
#define M_CMND    15


/* Blt Types Defines */
#define A_BLOOD   35   // Blood Life Time
#define A_SMOKE   15   // Smoke Life Time
#define A_SMOKE2  15   // Smoke2 Life Time
#define A_GILZA   400   // (Gilza Life Time)*6
#define BALL_FRM_CHANGE 20
#define BALL_SZ   8
#define BALL_MASS 4
#define BALL_DAMAGE 400
#define BALL_FRICTION 0.995

#define B_SHELL   0   // ID Number //
#define BA_SHELL  20  // Ammo in Pack //
#define D_SHELL   15  // Damage //
#define R_SHELL   30  // Reload After Fire It //
#define A_SHELL   90  // Life Time //
#define S_SHELL   2   // Bullet Speed / --- / *C_SHELL //
#define C_SHELL   8   // Speed Multiplier //

#define B_ARROW   1
#define BA_ARROW  15
#define D_ARROW   70
#define R_ARROW   60
#define A_ARROW   80
#define S_ARROW   2
#define C_ARROW   2

#define B_AKM     2
#define BA_AKM    50
#define R_AKM     8
#define S_AKM     2

#define B_NAPALM  3
#define BA_NAPALM 15
#define D_NAPALM  50
#define D_NAPALM2 1
#define R_NAPALM  18
#define A_NAPALM  120
#define A_NAPALM2 2000
#define FD_NAPALM2 20
#define BRK_NAPALM 1.5
#define S_NAPALM  1.5
#define C_NAPALM  2

#define B_ROCKET     4
#define BA_ROCKET    5
#define DN_ROCKET  200
#define DF_ROCKET  100
#define RNGN_ROCKET 25
#define RNGF_ROCKET 60
#define R_ROCKET    80
#define A_ROCKET   250
#define S_ROCKET     4

#define B_PHOTO      5
#define BA_PHOTO     1
#define R_PHOTO      30
#define NA_PHOTO     150
#define DN_PHOTO     300
#define DF_PHOTO     150
#define CR_PHOTO      8
#define RNGN_PHOTO   40
#define RNGF_PHOTO   100

#define B_GRENADE    6
#define BA_GRENADE   3
#define DN_GRENADE   200
#define DF_GRENADE   130
#define RNGN_GRENADE 20
#define RNGF_GRENADE 55
#define S_GRENADE    3
#define SP_GRENADE   2
#define A_GRENADE    150
#define R_GRENADE    40

#define B_REDMER     7
#define BA_REDMER    1
#define DN_REDMER    400
#define DF_REDMER    200
#define RNGN_REDMER  60
#define RNGF_REDMER  140
#define S_REDMER     3
#define R_REDMER     60
#define SZ_REDMER    1
#define SZB_REDMER   3

#define B_LASER      8
#define BA_LASER     125
#define D_LASER      24
#define MD_LASER     200
#define R_LASER      24
#define S_LASER      4
#define C_LASER      4

/* Izvrat Defines */
#define MSG_TIME      1
#define MSG_DEATH     4
#define MSG_KILL      2
#define D_TELEFRAG  500
#define DEAD_TIME   200
#define NEEDDEAD_TIME 25

/* Items Defines */
#define T_RESPAWN   1500
#define T_CORPSE    20000
#define H_MEDIKIT   75
#define H_BODYARMR  150

/* Items Type Defines (for Bots) */
#define M_IT         16
#define IT_ARMOUR    0
#define IT_HEALTH    1
#define IT_BOW       2
#define IT_AKM       3
#define IT_NAPALM    4
#define IT_LAUNCHER  5
#define IT_LASER     6
#define IT_REDMER    7
#define IT_PHOTO     8
#define IT_GRENADE   9
#define IT_PISTOLA   10
#define IT_BOWA      11
#define IT_AKMA      12
#define IT_NAPALMA   13
#define IT_LAUNCHERA 14
#define IT_LASERA    15
#define IT_UNKNOWN   16

/* End Game Defines */
#define EG_NOEG       0
#define EG_TIMELIMIT  1
#define EG_FRAGLIMIT  2
#define EG_SYSPAUSE   3

/* Other Defines */
#define M_WPN     9
#define M_BLT     1000
#define M_PLR     22
#define M_ITEM    17
#define M_POS     12
#define M_MAPELM  80
#define M_MASK    31
#define M_PLRFRM  16
#define V_SZ      12
#define V_SZ2     250
#define V_FOV     (M_PI/2)
#define P_SZ      5
#define F_LEN     8
#define D_LEN     10
#define M_STEPLEN 7
#define CUR_RANGE 60
#define WPN_RANGE 8
#define A_0       0
#define A_1       (M_PI/4)
#define A_2       (M_PI/2)
#define A_3       (M_PI/4*3)
#define A_4       M_PI
#define A_5       (M_PI/4*5)
#define A_6       (M_PI/2*3)
#define A_7       (M_PI/4*7)
#define P_ACC     0.2
#define P_TRN     (M_PI/64)
#define P_TRN2    (M_PI/256)
#define M_HEALTH  200
#define M_ARMOUR  200
#define SHOW_WPN  200
#define KILL_BLOOD  5
#define BLOWN_BLOOD 70
#define MSG_READWAIT 200
#define M_KILLTEXT 5
#define M_BLOWKILLTEXT 7
#define M_SELFKILLTEXT 7
#define N_NLPS 75    // Nuclon Loops per Second //
#define MENU_COL1  255
#define MENU_COL2  233
#define MENU_COL3  0
#define MENU_COL4  219

/* Player CONTROLs */
#define PC_NOCONTROL   0 // PC - Player Control
#define PC_KEY1        1
#define PC_KEY2        2
#define PC_NET         3
#define PC_BOT         4

/* Player STATEs ::: >0 - life ::: <0 - dead ::: =0 - out of game */
#define PS_DEAD        -1
#define PS_OUT         0
#define PS_BATTLE      1

/* Player Events */
#define M_EVNCODE   2048
#define PE_DEATH       0  // (2 Bytes) 1: MapPos
#define PE_DAMAGE      1  // (4 Bytes) 1,2: Damage 3: DamageOwn
#define PE_SEEPLR      2  // (2 Bytes) 1: Index
#define PE_SEEBLT      3  // (3 Bytes) 1: Index
#define PE_NOAMMO      4  // (1 Bytes)
#define PE_TELEPORT    5  // (5 Bytes) 1,2: X 3,4: Y

/* Player(Bot) Avoid Events */
#define M_AVOIDCODE   150
#define AE_NEXT       (-1)
#define AE_END        (-2)

/* Bots */
#define BDM_EQ       0  // BDM - Bot Distance Mode
#define BDM_HI       1
#define BDM_LO       2
#define BM_NEED      2
#define BM_POINTS    100
#define BM_NEXTPNT   16

/* Bot's Skills */
#define BS_DEATH     0
#define BS_CYBORG    1
#define BS_MASTER    2
#define BS_BUTCHER   3
#define BS_KILLER    4
#define BS_WARRIOR   5
#define BS_GORZ      6
#define BS_PROFI     7
#define BS_LAMO      8
#define BS_MEAT      9

/* Bot's Taktics */
#define BT_NORMAL    0
#define BT_AGRESSIVE 1
#define BT_SAFETY    2
#define BT_MAX       3

/* Bot's Needs */
#define BN_HP        0
#define BN_ANYWPN    1
#define BN_ANYAMMO   2  // Ammo for Weapon you have //
#define BN_FWPN      3
#define BN_FAMMO     4
#define BN_ARMR      5
#define BN_FRAG      6

/* Choose Bot for Play */
#define BC_BYNUM     0
#define BC_RANDOM    1

//#define _TEST

#pragma pack(push, 1)
struct PLAYER
{
    double x;
    double y;
    double a;              // Angle
    double sx;             // Projection of Vector of Speed on Axis X
    double sy;             // Projection of Vector of Speed on Axis Y
    double acc;            // Accelerate
    double trn;            // Angle Change Speed
    Si16   hp;             // Hit Points (Lifes)
    BYTE   damgfrm;        // Damage Frame
    BYTE   firefrm;        // Fire Frame
    BYTE   movefrm;        // Move Frame
    Si16   damage;         // Damage
    Si16   damageown;      // Damage Owner
	Si16   damageLast;     // Damage Last Frame
	Si16   armr;           // Armour
    BYTE   cmnd[M_CMND];   // Commands from Controller
    BYTE   wpn[M_WPN];     // Weapon State
    BYTE   actwpn;         // Active Weapon
    Si16   ammo[M_WPN];    // Amount of Ammo for Weapons
    Si16   frag;           // Amount of Frags
    Si16   reload;         // Reload Time Left (0 - Can Fire   >0 - Can't Fire)

    BYTE   n;              // Player Index in Primary Array

    char   Name[16];
    BYTE   Control;        // Controller Type
    Si16   State;          // Player State

    void (*Move)(PLAYER *);       // Realize Velocity of Player
    void (*MakeCmnd)(PLAYER *);   // Realize Command from Controller
    void ResetCmnd(void);         // Clear Command from Controller
};

struct PACK;

struct OBJECT
{
    double x;
    double y;
	union {
		double sx;
	    PACK* pack;
	};
    double sy;
    Si16 age;
    Si16 own;
    void (*Handler)(void*);
    void (*Draw)(Si16,Si16,void*);
    //OBJECT(void) {x=0;y=0;sx=0;sy=0;age=0;own=0;}
};

struct VIEWPORT
{
    BYTE ActPlayer;
    BYTE NoFocusing;
    char Message[4][64];
    Si16 MsgCounter[4];
    Si16 ShowNames;
	BYTE showBotPoints = 0;
    Si16 x;
    Si16 y;
    Si16 scr_x1;
    Si16 scr_y1;
    Si16 scr_x2;
    Si16 scr_y2;
    void (*Focusing)(void*);

	std::unique_ptr<FogFilter> fogFilter;
	std::unique_ptr<DmgFilter> dmgFilter;
	FilterList filters;

	VIEWPORT();
	void Init(Si16 _scr_x1, Si16 _scr_y1, Si16 _scr_x2, Si16 _scr_y2);
	void ShowPoints(void);
    void Show(void);
    void MakeMessage(char *,Si16 Important=0);
    void ClearMessages(void);

	Vec2Si32 Screen2World(const Vec2Si32& scr) const
	{
		return Vec2Si32(scr.x - scr_x1 + x, scr.y - scr_y1 + y);
	}

	Vec2Si32 ScreenCenter() const
	{
		return Vec2Si32((scr_x1 + scr_x2) / 2, (scr_y1 + scr_y2) / 2);
	}
};

struct PLREVENT
{
    Si16 Length;
    BYTE Code[M_EVNCODE];
};

struct POINT {
    Si16 x;
    Si16 y;
    BYTE NPnts; // Number of adjacent points
    Si16 NPnt[BM_NEXTPNT]; // Indices of adjacent points
    Si16 Weight[BM_NEXTPNT];
};

struct ITEMPOINT {
	BYTE Initialized;
    BYTE Type;
    Si16 Pnt;
    Si16 ItmNum;
    OBJECT *Item;
};

struct BOT {
    PLAYER *Own;
    Si16 *ItmMem;
    BYTE Target; // Player
    Si16 Distance;
    Si16 AimSize;
    Si16 AimFov;
    Si16 Fire;
    Si16 TurnAround;
    BYTE cnt1;
    BYTE cnt2;
    BYTE cnt3;
    Si16 tx;
    Si16 ty;
    Si16 rtx;
    Si16 rty;
    Si16 PntNum;
    Si16 NPntNum;
    BYTE Weapon;
    BYTE NoAmmos;
    BYTE OwnNum;
    BYTE Skill;      // --.
    BYTE LikeWpn;    // ---=- Defines by CONFIG file
    BYTE Taktic;     // --'

    Si16 AvoidCode[M_AVOIDCODE];
    void (*Handler)(BOT *);

	// anti-stall-in-the-wall
	Si16 prev_x;
	Si16 prev_y;
	Si16 prev_cnt;
};
/*****************************/
/* for SETUP --- *.CFG files */
/*****************************/

struct CONFIG
{
    BYTE Keys1[12];
    BYTE Keys2[12];
    BYTE PortNum;
    BYTE PortSpeed;
    char Name1[16];
    char Name2[16];
    char BotName[22][16];
    BYTE BotSkill[22];
    BYTE BotWpn[22];
    BYTE BotTaktic[22];
    BYTE BotChs;
    Si16 MaxFps;
	Si16 TimeLimit;
	Si16 FragLimit;
	Si16 StartWeapon[M_WPN];

    void Load(char *FileName);
    void Default(void);
};

struct PACK
{
    double sx;
    Si16 Shell;
    Si16 Arrow;
    Si16 AkmAmmo;
    Si16 NapalmAmmo;
    Si16 Rocket;
    Si16 LaserAmmo;
    BYTE Weapon;
};

#pragma pack(pop)