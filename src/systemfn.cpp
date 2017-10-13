/*
2D Quake v1.04
Copyright by bladez-fate
10 July 2002 - 7 July 2003
*/

#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>

#include "2dqdefs.h"
#include "keyboard.h"
#include "game.h"

#define M_TRY_READ 10000000l

BYTE *_Random3;
BYTE *_Random4;
BYTE *_Random6;
BYTE *_Random7;
BYTE *_Random9;
BYTE *_Random21;
BYTE *_Random61;

//extern void (*SerialPortWrite)(char WriteByte);
//extern void (*SerialPortClose)(void);

//void SendCmnd(void);
//BYTE SendBool(BYTE Bool);
//BYTE ReadBool(BYTE First=0,BYTE Offset=0);
//BYTE SendIt(void);

BYTE _FreeBits=8;
BYTE _Byte=0;

/*
BYTE SendIt(void) {
    _Byte<<=_FreeBits;
    SerialPortWrite(_Byte);
    _FreeBits=8;
    BYTE r=_Byte;
    _Byte=0;
    return r;
}
BYTE SendBool(BYTE Bool)
{
    _Byte<<=1;
    if(Bool)_Byte++;
    _FreeBits--;
    if(!_FreeBits) {
        SerialPortWrite(_Byte);
        _FreeBits=8;
        Bool=_Byte;
        _Byte=0;
        return Bool;
    }
    return 0;
}

BYTE ReadBool(BYTE First,BYTE Offset)
{
    static BYTE CurBit=0;
    static BYTE CurByte=0;
    BYTE Result;
    if(First) {CurBit=0;CurByte=Offset;}
    Result=(BYTE)(SerialReadByte[CurByte]<<CurBit)>>7;
    CurBit++;
    if(CurBit==8) {CurBit=0;CurByte++;}
    return Result;
}

void SendCmnd(void)
{
    Si16 a,pln;
    Si16 ErrorNum=0;
    long TryRead;
    BYTE CheckSum;
    if(NetStatus==1) {
        // First Read
        for(;;) {
            TryRead=0;
            while(SerialReaded!=NeedRead)
            {
                TryRead++;
                if(TryRead>M_TRY_READ) {SerialError++;ErrorNum++;break;}
            }
            if(ErrorNum>10) {ExitGame("Serial Port Read 10 Error");}
            if(SerialError) {
                SerialReaded=0;
                SerialError--;
                SerialPortWrite(0x01);
            }
            else {
                CheckSum=0;
                for(Si16 i=0;i<NeedRead-1;i++) {CheckSum+=SerialReadByte[i];}
                if(CheckSum==SerialReadByte[NeedRead-1])
                {
                    SerialPortWrite(0x00); // NO-ERRORS Signal
                    ErrorNum=0;
                    break;
                }
                else {
                    ErrorNum++;
                    SerialReaded=0;
                    SerialPortWrite(0x01); // ERROR Signal
                }
            }
        }

        a=1;
        pln=0;
        while(pln<Players) {
            if(plr[pln].Control==PC_NET) {
                for(Si16 i=0;i<M_CMND;i++) {
                    plr[pln].cmnd[i]=ReadBool(a);
                    a=0;
                }
            }
            pln++;
        }
        TryExit=TryExit||SerialReadByte[NeedRead-2];
        SerialReaded=0;

        // First Write
        for(;;)
        {
            CheckSum=pln=0;
            while(pln<Players) {
                if(plr[pln].Control!=PC_NET) {
                    for(Si16 i=0;i<M_CMND;i++) {
                        CheckSum+=SendBool(plr[pln].cmnd[i]);
                    }
                }
                pln++;
            }
            CheckSum+=SendIt();
            SerialPortWrite(TryExit); CheckSum+=TryExit;
            SerialPortWrite(CheckSum);
            TryRead=0;
            while(SerialReaded<1)
            {
                TryRead++;
                if(TryRead>M_TRY_READ*5l) {ExitGame("No Serial Answer"); break;}
            }
            if(SerialReadByte[0]==0x00) {ErrorNum=0;break;}
            else {
                SerialReaded=0;
                ErrorNum++;
            }
            if(ErrorNum>10) {ExitGame("Serial Port Write 10 Error");}
        }
        SerialReaded=0;
        if(TryExit) ExitGame("User Terminate");
    }
    else
    {
        // Second Write
        for(;;)
        {
            CheckSum=pln=0;
            while(pln<Players) {
                if(plr[pln].Control!=PC_NET) {
                    for(Si16 i=0;i<M_CMND;i++) {
                        CheckSum+=SendBool(plr[pln].cmnd[i]);
                        plr[pln].cmnd[i];
                    }
                }
                pln++;
            }
            CheckSum+=SendIt();
            SerialPortWrite(TryExit); CheckSum+=TryExit;
            SerialPortWrite(CheckSum);
            TryRead=0;
            while(SerialReaded<1)
            {
                TryRead++;
                if(TryRead>M_TRY_READ*5l) {ExitGame("No Serial Answer"); break;}
            }
            if(SerialReadByte[0]==0x00) {ErrorNum=0;break;}
            else {
                SerialReaded=0;
                ErrorNum++;
            }
            if(ErrorNum>10) {ExitGame("Serial Port Write 10 Error");}
        }

        // Second Read
        for(;;) {
            TryRead=0;
            while(SerialReaded!=NeedRead+1)
            {
                TryRead++;
                if(TryRead>M_TRY_READ) {SerialError++;ErrorNum++;break;}
            }
            if(ErrorNum>10) {ExitGame("Serial Port Read 10 Error");}
            if(SerialError) {
                SerialReaded=1;
                SerialError--;
                SerialPortWrite(0x01);
            }
            else {
                CheckSum=0;
                for(Si16 i=1;i<NeedRead;i++) {CheckSum+=SerialReadByte[i];}
                if(CheckSum==SerialReadByte[NeedRead])
                {
                    SerialPortWrite(0x00);
                    ErrorNum=0;
                    break;
                }
                else {
                    ErrorNum++;
                    SerialReaded=1;
                    SerialPortWrite(0x01);
                }
            }
        }

        a=1;
        pln=0;
        while(pln<Players) {
            if(plr[pln].Control==PC_NET) {
                for(Si16 i=0;i<M_CMND;i++) {
                    plr[pln].cmnd[i]=ReadBool(a,a);
                    a=0;
                }
            }
            pln++;
        }
        TryExit=TryExit||SerialReadByte[NeedRead-1];
        SerialReaded=0;

        if(TryExit) ExitGame("User Terminate");
    }
}
*/

Si16 Random(Si16 Num)
{
    if(Num==3) {
        static Si16 _R3=0;
        _R3=(_R3+1)%30;
        return (Si16)_Random3[_R3];
    }
    if(Num==4) {
        static Si16 _R4=0;
        _R4=(_R4+1)%40;
        return (Si16)_Random4[_R4];
    }
    if(Num==6) {
        static Si16 _R6=0;
        _R6=(_R6+1)%60;
        return (Si16)_Random6[_R6];
    }
    if(Num==7) {
        static Si16 _R7=0;
        _R7=(_R7+1)%70;
        return (Si16)_Random7[_R7];
    }
    if(Num==9) {
        static Si16 _R9=0;
        _R9=(_R9+1)%90;
        return (Si16)_Random9[_R9];
    }
    if(Num==21) {
        static Si16 _R21=0;
        _R21=(_R21+1)%210;
        return (Si16)_Random21[_R21];
    }
    if(Num==61) {
        static Si16 _R61=0;
        _R61=(_R61+1)%610;
        return (Si16)_Random61[_R61];
    }
    return 0;
}
Si16 InitRandom(void)
{
    FILE *Source;
    if ((Source = fopen("system/random3", "rb")) == NULL) {return 0;}
    if((_Random3 = (BYTE *) malloc(30)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random3,1,30,Source);
    fclose(Source);
    if ((Source = fopen("system/random4", "rb")) == NULL) {return 0;}
    if((_Random4 = (BYTE *) malloc(40)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random4,1,40,Source);
    fclose(Source);
    if ((Source = fopen("system/random6", "rb")) == NULL) {return 0;}
    if((_Random6 = (BYTE *) malloc(60)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random6,1,60,Source);
    fclose(Source);
    if ((Source = fopen("system/random7", "rb")) == NULL) {return 0;}
    if((_Random7 = (BYTE *) malloc(70)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random7,1,70,Source);
    fclose(Source);
    if ((Source = fopen("system/random9", "rb")) == NULL) {return 0;}
    if((_Random9 = (BYTE *) malloc(90)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random9,1,90,Source);
    fclose(Source);
    if ((Source = fopen("system/random21", "rb")) == NULL) {return 0;}
    if((_Random21 = (BYTE *) malloc(210)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random21,1,210,Source);
    fclose(Source);
    if ((Source = fopen("system/random21", "rb")) == NULL) {return 0;}
    if((_Random61 = (BYTE *) malloc(610)) == NULL)
    {fclose(Source);return 0;}
    fread(_Random61,1,610,Source);
    fclose(Source);
    return 1;
}

void DefaultKeys(BYTE *Key,char PlrNum)
{
    if(PlrNum=='1') {
		using namespace arctic::easy;
		//       Fwd           Bck          Left          Right
        //    StrLft      StrRight         Shoot         Sniper  
		//    PrvWpn        NxtWpn           Use           View

		Key[0] = 'W'; Key[1] = 'S'; Key[2] = 'A';  Key[3] = 'D';
		Key[4] = 'Q'; Key[5] = 'E'; Key[6] = 'R';  Key[7] = 'T';
		Key[9] = '4'; Key[8] = '5'; Key[10] = 'F'; Key[11] = 'G';
    }
    if(PlrNum=='2') {
		Key[0] = 'O'; Key[1] = 'L'; Key[2] = 'K'; Key[3] = ';';
		Key[4] = 'I'; Key[5] = 'P'; Key[6] = '['; Key[7] = ']';
		Key[9] = '-'; Key[8] = '='; Key[10] = '\''; Key[11] = '\\';
    }
}

void CONFIG::Load(char *FileName)
{
    FILE *strm;
    if((strm=fopen(FileName,"rb"))==NULL) {
        printf("Can't Find %s: Default Configuration Will Be Used.\nPlease, Run SETUP.EXE\nPress any Key to Continue ...",FileName);
        //Getch();
        Default();
    } else {
        fread(&cfg,1,sizeof(CONFIG),strm);
        fclose(strm);
    }
	Default(); // Use default anyway unless we port SETUP.EXE
}

void CONFIG::Default(void)
{
    DefaultKeys(Keys1,'1');
    DefaultKeys(Keys2,'2');
    PortNum = 1;
    PortSpeed = 0x01;
    strcpy(Name1,"Игрок1");
    strcpy(Name2,"Игрок2");
    for(Si16 i=0;i<22;i++)
    {
        sprintf(BotName[i],"Bot%hd",i);
        BotSkill[i]=0;
        BotWpn[i]=9;
        BotTaktic[i]=random(BT_MAX);
    }
    BotChs=0;
    MaxFps=60;
    FragLimit=15;
    TimeLimit=0;
    for(Si16 i=1;i<M_WPN;i++) StartWeapon[i]=0;
    StartWeapon[B_SHELL]=50;
}
