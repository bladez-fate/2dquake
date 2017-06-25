#pragma once

/**********************/
/* 19 July 2002 bladez-fate */
/*  Graphics Library  */
/**********************/

#include "defs.h"

#include <algorithm>

#include "engine/easy.h"

using namespace arctic;
using namespace arctic::easy;

namespace screen {
	constexpr int w = 320 * 3 / 2;
	constexpr int h = 200 * 3 / 2;
	constexpr int size = w*h;

	constexpr int cx = w / 2;
	constexpr int cy = h / 2;

	inline int pixel(int x, int y)
	{
		return y * w + x;
	}

	inline int x(int pos)
	{
		return pos % w;
	}

	inline int y(int pos)
	{
		return pos / w;
	}
}

#pragma pack(push, 1)
struct tIMAGE
{
    WORD     Width;
    WORD     Height;
    BYTE    *Bits;
};

struct tFONT
{
    BYTE    *Bits;
    BYTE     Width;
    BYTE     Height;
    BYTE     Intervals;
};

struct tPALETTE {
	BYTE Bits[768];
	Si16 Load(char *FileName);
	void Set(void);
	void DarkAll(Si16 K);
	void LightAll(Si16 K);
	void Clear(void);
};
#pragma pack(pop)

extern tFONT g_font;
extern tPALETTE g_palette;

struct FogFilter {
	// Config
	static constexpr size_t angSize = 1024;
	static constexpr float angDelta = 2.0f * float(M_PI) / angSize;
	static constexpr float blurSightTan = float(M_PI) / 10.0f;
	static constexpr float maxSightDistance = 1000.0f;
	static constexpr float minSightDistance = 7.0f;

	int x1;
	int y1;
	int x2;
	int y2;
	int cx;
	int cy;
	struct PixelData {
		float r = 0.0f; // distance to the pixel center
		size_t a = 0; // ang[] index of center of the pixel

		// relevant ang[] indices are [rho1idx; rho2idx)
		size_t a1 = 0;
		size_t a2 = 0;
	};
	std::vector<PixelData> pxl;

	// State
	bool enabled = false;
	size_t fov_a1;
	size_t fov_a2;
	float min_light = 0.4f;

	// Frame data
	struct AngleData {
		float sightDistance = maxSightDistance;
	};
	std::vector<AngleData> ang;

	FogFilter(int _x1, int _y1, int _x2, int _y2, int _cx, int _cy)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2)
		, cx(_cx), cy(_cy)
	{
		ang.resize(angSize);
		pxl.resize(screen::size);
		for (int y = y1; y <= y2; y++) {
			for (int x = x1; x <= x2; x++) {
				size_t pos = screen::pixel(x, y);
				PixelData& p = pxl[pos];
				int dx = x - cx;
				int dy = y - cy;
				p.r = sqrt(dx*dx + dy*dy);
				if (p.r > 0.0f) {
					p.a = Angle(atan2(dy, dx));
					auto a = std::minmax({
						Angle(atan2(dy + 0.5f, dx + 0.5f)),
						Angle(atan2(dy - 0.5f, dx + 0.5f)),
						Angle(atan2(dy + 0.5f, dx - 0.5f)),
						Angle(atan2(dy - 0.5f, dx - 0.5f))
					});
					if (a.second - a.first > angSize / 2) {
						// special region: pi <-> -pi
						p.a1 = a.second;
						p.a2 = a.first + 1;
					}
					else {
						p.a1 = a.first;
						p.a2 = a.second + 1;
					}
				}
				else {
					// center pixel's data is uninitialized
				}
			}
		}
	}

	size_t Angle(double a)
	{
		return size_t((a + 2 * M_PI) / angDelta) % angSize;
	}

	void Refresh()
	{
		enabled = false;
	}

	void Refresh(float a, float fov)
	{
		enabled = true;
		float goodA = -a;
		fov_a1 = Angle(goodA - fov);
		fov_a2 = Angle(goodA + fov) + 1;
		float ax = cos(a);
		float ay = -sin(a);
		for (auto& an : ang) {
			an.sightDistance = maxSightDistance;
		}
	}

	void AddOpaquePixel(size_t pos)
	{
		PixelData& p = pxl[pos];
		for (size_t a = p.a1; a != p.a2; a++) {
			if (a == angSize) {
				a = 0;
			}
			ang[a].sightDistance = std::max(minSightDistance, std::min(ang[a].sightDistance, p.r));
		}
	}

	bool FovVisible(size_t a) const
	{
		if (fov_a1 <= fov_a2) { // special point outside FOV
			return a >= fov_a1 && a < fov_a2;
		}
		else { // special point inside FOV
			return a >= fov_a1 || a < fov_a2;
		}
	}

	size_t AngleDistance(size_t a, size_t b) const
	{
		if (a <= b) {
			return std::min(b - a, a + angSize - b);
		}
		return AngleDistance(b, a);
	}

	size_t OutOfFov(size_t a) const
	{
		return std::min({
			AngleDistance(a, fov_a1),
			AngleDistance(a, fov_a2)
		});
	}

	Rgba Apply(size_t pos, Rgba c) const
	{
		if (enabled) {
			const PixelData& p = pxl[pos];
			float directSight = ang[p.a].sightDistance;
			float directSightDelta = p.r - directSight;
			float l = std::min(1.0f, std::max(min_light, 1.0f - 0.1f * directSightDelta));
			if (!FovVisible(p.a)) {
				float outOfFov = OutOfFov(p.a);
				float darkness = std::min(outOfFov, std::max(0.0f, p.r - 8.0f) * 16.0f);
				l = std::min(l, std::max(min_light, 1.0f - 0.01f * darkness));
			}
			return Rgba(c.r * l, c.g * l, c.b * l, c.a);
		}
		return c;
	}
};

struct DmgFilter {
	// Config
	int x1;
	int y1;
	int x2;
	int y2;
	float max_light = 0.7f;

	// State
	bool enabled = false;
	int damageLeft = 0;
	int max_damage = 0;

	// Frame data
	float light[screen::size];

	DmgFilter(int _x1, int _y1, int _x2, int _y2)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{}

	Rgba Apply(size_t pos, Rgba c) const
	{
		if (enabled) {
			float l = light[pos];
			float s = 1 - l;
			return Rgba(
				c.r * s + 255 * l,
				c.g * s + 0 * l,
				c.b * s + 0 * l,
				c.a);
		}
		else {
			return c;
		}
	}

	void Refresh(int damage)
	{
		damageLeft += damage;
		if (damage > 0) {
			max_damage = damageLeft;
		}
		if (damageLeft) {
			enabled = true;
			for (int y = y1; y <= y2; y++) {
				float* l_iter = &light[screen::pixel(x1, y)];
				for (int x = x1; x <= x2; x++) {
					int ax = x - x1;
					int bx = x2 - x;
					int ay = y - y1;
					int by = y2 - y;
					float dist = ax;
					if (bx < dist) dist = bx;
					if (ay < dist) dist = ay;
					if (by < dist) dist = by;
					float maxdist = std::min(max_damage / 3, 70);
					float height = max_damage * (maxdist - dist) / maxdist; // f(seed, max_damage) < max_damage;
					float l = (height - max_damage + damageLeft) / damageLeft;
					*(l_iter++) = std::min(max_light, std::max(l, 0.0f));
				}
			}
			damageLeft -= 5;
			if (damageLeft < 0) {
				damageLeft = 0;
			}
		}
		else {
			enabled = false;
		}
	}
};

struct FilterList {
	FogFilter* fog_;
	DmgFilter* dmg_;

	Rgba Apply(size_t pos, Rgba c) const
	{
		c = fog_->Apply(pos, c);
		c = dmg_->Apply(pos, c);
		return c;
	}
};

void EnableFilters(FilterList* filters);
void DisableFilters();

inline void ToPalAll(tPALETTE &Dst, tPALETTE &Src);
inline void PalCopy(tPALETTE &Dst, tPALETTE &Src);

void SetPixelColor(size_t pos, BYTE color);
void SetPixelColor(int x, int y, BYTE color);
void SetPixel(size_t pos);
void SetPixel(int x, int y);
void CopyToBackBuffer(size_t pos, BYTE* src, size_t size);
void WinBufPixel(int x, int y);
void SetColor(BYTE a);
void ShowScreen(void);
BYTE InitGraph(void);
void SetPal(BYTE Index, BYTE Red, BYTE Green, BYTE Blue);
void HorzLine(Si16 x1, Si16 x2, Si16 y);
void VertLine(Si16 y1, Si16 y2, Si16 x);
void FrameRect(Si16 x1, Si16 y1, Si16 x2, Si16 y2);
void FillRect(Si16 x1, Si16 y1, Si16 x2, Si16 y2);
void ClearScreen(void);
void Line(Si16 x1, Si16 y1, Si16 x2, Si16 y2);
void MakeBufferWindow(Si16 x1,Si16 y1,Si16 x2,Si16 y2);
void NoBufferWindow(void);
void OutText(Si16 x, Si16 y, const char *Text, tFONT *g_font);
void SetTextJust(BYTE x,BYTE y);
BYTE LoadImage(char  *FileName,tIMAGE  *Img);
void FreeImage(tIMAGE  *Img);
void DrawSWImage(Si16 x,Si16 y,tIMAGE  *Img);
void DrawImage(Si16 x,Si16 y,tIMAGE  *Img);
void DrawWImage(Si16 x,Si16 y,tIMAGE  *Img);
void DrawSWUImage(Si16 x,Si16 y,BYTE uni,tIMAGE  *Img);
void DrawSImage(Si16 x,Si16 y,tIMAGE  *Img);
BYTE LoadFont(char  *FileName,tFONT  *Fnt);
void FreeFont(tFONT  *Fnt);
