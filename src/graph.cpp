/********************************/
/* 19 July 2002 bladez-fate */
/*       Graphics Library       */
/********************************/

#include "defs.h"

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

BYTE Color = 0;
Rgba ColorRgba(0, 0, 0);
int bw_x1 = 0;
int bw_y1 = 0;
int bw_x2 = screen::w - 1;
int bw_y2 = screen::h - 1;
BYTE fnJustX;
BYTE fnJustY;

tFONT g_font;
tPALETTE g_palette;
Rgba g_paletteData[256];

static FilterList* g_activeFilters = nullptr;

void EnableFilters(FilterList* filters)
{
	g_activeFilters = filters;
}

void DisableFilters()
{
	g_activeFilters = nullptr;
}

inline Rgba FltCol(size_t pos, Rgba c)
{
	if (g_activeFilters) {
		return g_activeFilters->Apply(pos, c);
	}
	else {
		return c;
	}
}

void SetColor(BYTE a)
{
	Color = a;
	ColorRgba = g_paletteData[a];
}

inline void SetPal(BYTE index, BYTE red, BYTE green, BYTE blue)
{
	g_paletteData[index] = Rgba(red * 4, green * 4, blue * 4);
}

Si16 tPALETTE::Load(char *FileName)
{
	FILE *Source;
	if ((Source = fopen(FileName, "rb")) == NULL) { return 0; }
	fread(Bits, 1, 768, Source);
	fclose(Source);
	return 1;
}

void tPALETTE::Set(void)
{
	for (Si16 i = 0; i < 256; i++) {
		SetPal(i, Bits[i * 3], Bits[i * 3 + 1], Bits[i * 3 + 2]);
	}
}

void tPALETTE::DarkAll(Si16 K)
{
	Si16 T;
	for (Si16 i = 0; i<768; i++)
	{
		T = Bits[i]; T -= K;
		if (T<0) T = 0;
		Bits[i] = T;
	}
}

void tPALETTE::LightAll(Si16 K)
{
	Si16 T;
	for (Si16 i = 0; i<768; i++)
	{
		T = Bits[i]; T += K;
		if (T>63) T = 63;

		Bits[i] = T;
	}
}

void tPALETTE::Clear(void)
{
	memset(Bits, 0, 768);
}

void ToPalAll(tPALETTE &Dst, tPALETTE &Src)
{
	Si16 d;
	Si16 s;
	for (Si16 i = 0; i<768; i++) {
		d = Dst.Bits[i];
		s = Src.Bits[i];
		if (d<0 || d>255 || s<0 || s>255) {
			s = d;
		}

		if (d <= s - 1)
		{
			d += 1;
		}
		else
		{
			if (d >= s + 1)
			{
				d -= 1;
			}
			else
			{
				d = s;
			}
		}

		Dst.Bits[i] = d;
	}
}

void PalCopy(tPALETTE &Dst, tPALETTE &Src)
{
	memcpy(Dst.Bits, Src.Bits, 768);
}

void ShowScreen(void)
{
	ShowFrame();
}

BYTE InitGraph(void)
{
	ResizeScreen(screen::w, screen::h);
	Clear();
	ShowFrame();
    return 1;
}

void SetPixelColorRgba(size_t pos, Rgba colorRgba)
{
	GetEngine()->GetBackbuffer().RgbaData()[pos] = FltCol(pos, colorRgba);
}

void SetPixelColor(size_t pos, BYTE color)
{
	SetPixelColorRgba(pos, g_paletteData[color]);
}

void SetPixelColor(int x, int y, BYTE color)
{
	SetPixelColor(screen::pixel(x, y), color);
}

void SetPixel(size_t pos)
{
	SetPixelColorRgba(pos, ColorRgba);
}

void SetPixel(int x, int y) 
{
	SetPixel(screen::pixel(x, y));
}

void CopyToBackBuffer(size_t pos, BYTE* src, size_t size)
{
	size_t endPos = pos + size;
	for (; pos < endPos; src++, pos++) {
		SetPixelColor(pos, *src);
	}
}

void WinBufPixel(int x, int y)
{
    if(x>=bw_x1 && x<=bw_x2 && y>=bw_y1 && y<=bw_y2)
        SetPixel(x,y);
}

void HorzLine(Si16 x1, Si16 x2, Si16 y) 
{
	for(Si16 x=x1;x<=x2;x++)
		WinBufPixel(x,y);
}

void VertLine(Si16 y1, Si16 y2, Si16 x)
{
	for(Si16 y=y1;y<=y2;y++)
		WinBufPixel(x,y);
}

void FrameRect(Si16 x1, Si16 y1, Si16 x2, Si16 y2)
{
    HorzLine(x1,x2,y1);
    VertLine(y1+1,y2-1,x1);
    VertLine(y1+1,y2-1,x2);
    HorzLine(x1,x2,y2);
}

void FillRect(Si16 x1, Si16 y1, Si16 x2, Si16 y2)
{
    for(Si16 y=y1;y<=y2;y++)
        for(Si16 x=x1;x<=x2;x++)
            WinBufPixel(x,y);
}

void ClearScreen(void)
{
	for (Si32 i = 0; i < screen::size; i++)
		SetPixel(i);
}

void Line(Si16 x1, Si16 y1, Si16 x2, Si16 y2)
{
	Si16 x,y;
    x = x2-x1;
    y = y2-y1;
    Si16 sx = sgn(x);
    Si16 sy = sgn(y);
    Si16 dx = abs(x);
    Si16 dy = abs(y);

    x=x1; y=y1;

    Si16 Bound, Count=0;
    if(dx>=dy)
    {
        Bound=(dx+1)>>1;
        for(Si16 i=0;i<=dx;i++)
        {
            WinBufPixel(x,y);
            Count+=dy;
            if(Count>=Bound) { y+=sy; Count-=dx; }
            x+=sx;
        }
    }
    else
    {
        Bound=(dy+1)>>1;
        for(Si16 i=0;i<=dy;i++)
        {
            WinBufPixel(x,y);
            Count+=dx;
            if(Count>=Bound) { x+=sx; Count-=dy; }
            y+=sy;
        }
    }
}

void MakeBufferWindow(Si16 x1,Si16 y1,Si16 x2,Si16 y2)
{
	bw_x1 = x1;
	bw_y1 = y1;
	bw_x2 = x2;
	bw_y2 = y2;
}

void NoBufferWindow(void)
{
	bw_x1 = 0; 
	bw_y1 = 0;
	bw_x2 = screen::w - 1; 
	bw_y2 = screen::h - 1;
}

BYTE LoadImage(char  *FileName, tIMAGE  *Img)
{
    FILE *Source;
    char NearFileName[64];
    strcpy(NearFileName,FileName);
    if ((Source = fopen(NearFileName, "rb")) == NULL) {return 0;}
    WORD W,H;
    fread(&W,2,1,Source);
    fread(&H,2,1,Source);
    Img->Width=W; Img->Height=H;
    if((Img->Bits = (BYTE  *) malloc(Img->Width*Img->Height)) == NULL) {
		fclose(Source);
		return 0;
	}
    for(WORD i=0;i<W*H;i++) {
        BYTE b;
        fread(&b,1,1,Source);
        Img->Bits[i]=b;
    }
    fclose(Source);
    return 1;
}

void FreeImage(tIMAGE  *Img) {free(Img->Bits);}

void DrawSWImage(Si16 x, Si16 y, tIMAGE  *Img)
{
	long BufOff;
	long ImgOff;
	{
		BufOff = (long)screen::pixel(x, y);
		ImgOff = 0;
		for (long i = 0; i < Img->Height; i++) {
			for (long j = 0; j < Img->Width; j++) {
				if (Img->Bits[ImgOff + j] && x + j >= bw_x1 && y + i >= bw_y1 && x + j <= bw_x2 && y + i <= bw_y2) {
					SetPixelColor(BufOff + j, Img->Bits[ImgOff + j]);
				}
			}
			BufOff += screen::w;
			ImgOff += Img->Width;
		}
	}
}

void DrawImage(Si16 x, Si16 y, tIMAGE  *Img)
{
	long BufOff = (long)screen::pixel(x, y);
	long ImgOff = 0;
	for (long i = 0; i < Img->Height; i++)
	{
		//memcpy(Buffer+BufOff,Img->Bits+ImgOff,Img->Width);
		CopyToBackBuffer(BufOff, Img->Bits + ImgOff, Img->Width);
		BufOff += screen::w;
		ImgOff += Img->Width;
	}
}

void DrawWImage(Si16 x, Si16 y, tIMAGE *Img)
{
	long BufOff;
	long ImgOff;
	if (x >= bw_x1 && y >= bw_y1 && x + Img->Width - 1 <= bw_x2 && y + Img->Height - 1 <= bw_y2)
	{
		BufOff = (long)screen::pixel(x, y);
		ImgOff = 0;
		for (long i = 0; i < Img->Height; i++)
		{
			CopyToBackBuffer(BufOff, Img->Bits + ImgOff, Img->Width);
			BufOff += screen::w;
			ImgOff += Img->Width;
		}
	}
	else {
		BufOff = (long)screen::pixel(x, y);
		ImgOff = 0;
		for (long i = 0; i < Img->Height; i++)
		{
			for (long j = 0; j < Img->Width; j++)
			{
				if (x + j >= bw_x1 && y + i >= bw_y1 && x + j <= bw_x2 && y + i <= bw_y2) {
					SetPixelColor(BufOff + j, Img->Bits[ImgOff + j]);
				}
			}
			BufOff += screen::w;
			ImgOff += Img->Width;
		}
	}
}

void DrawSWUImage(Si16 x, Si16 y, BYTE uni, tIMAGE  *Img)
{
	uni = uni % 4;
	long BufOff;
	long ImgOff;
	BYTE cplus = uni * 18;
	{
		BufOff = (long)screen::pixel(x, y);
		ImgOff = 0;
		for (long i = 0; i < Img->Height; i++) {
			for (long j = 0; j < Img->Width; j++) {
				if (Img->Bits[ImgOff + j] && x + j >= bw_x1 && y + i >= bw_y1 && x + j <= bw_x2 && y + i <= bw_y2) {
					BYTE c = Img->Bits[ImgOff + j];
					if (c >= 176 && c < 194) c += cplus;
					SetPixelColor(BufOff + j, c);
				}
			}
			BufOff += screen::w;
			ImgOff += Img->Width;
		}
	}
}

void DrawSImage(Si16 x, Si16 y, tIMAGE  *Img)
{
	long BufOff = (long)screen::pixel(x, y);
	long ImgOff = 0;
	for (long i = 0; i < Img->Height; i++)
	{
		for (long j = 0; j < Img->Width; j++)
		{
			if (Img->Bits[ImgOff + j] != 0) {
				SetPixelColor(BufOff + j, Img->Bits[ImgOff + j]);
			}
		}
		BufOff += screen::w;
		ImgOff += Img->Width;
	}
}

BYTE LoadFont(char *FileName, tFONT *Fnt)
{
	FILE *Source;
	if ((Source = fopen(FileName, "rb")) == NULL) { return 0; }
	fread(&(Fnt->Width), 1, 1, Source);
	fread(&(Fnt->Height), 1, 1, Source);
	fread(&(Fnt->Intervals), 1, 1, Source);
	if ((Fnt->Bits = (BYTE *)malloc((WORD)Fnt->Width*Fnt->Height * 255)) == NULL)
	{
		fclose(Source); return 0;
	}
	fread(Fnt->Bits, 1, (WORD)Fnt->Width*Fnt->Height * 255, Source);
	fclose(Source);
	return 1;
}

void FreeFont(tFONT  *Fnt)
{
    free(Fnt->Bits);
}

void OutText(Si16 x, Si16 y, const char* text, tFONT* font)
{
	if (fnJustX == 1) {
		x -= (Si16)(strlen(text)*(font->Width + font->Intervals) - font->Intervals) / 2;
	}
	if (fnJustX == 2) {
		x -= (Si16)strlen(text)*(font->Width + font->Intervals) - font->Intervals + 1;
	}
	if (fnJustY == 1) {
		y -= font->Height / 2;
	}
	if (fnJustY == 2) {
		y -= font->Height + 1;
	}
	size_t BufOff;
	size_t ImgOff;
	size_t SymbolSize = font->Width * font->Height;
	for (size_t s = 0; text[s] != 0; s++) {
		BufOff = (size_t)screen::pixel(x, y);
		ImgOff = 0;
		for (size_t i = 0; i < font->Height; i++)
		{
			for (size_t j = 0; j < font->Width; j++)
			{
				if (font->Bits[SymbolSize*(BYTE)text[s] + ImgOff + j] != 0) {
					SetPixel(BufOff + j);
				}
			}
			BufOff += screen::w;
			ImgOff += font->Width;
		}
		x += font->Height + font->Intervals;
	}
}

void SetTextJust(BYTE x,BYTE y)
{
	fnJustX=x;
	fnJustY=y;
}
