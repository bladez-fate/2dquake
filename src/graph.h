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
	constexpr int w = 640;
	constexpr int h = 400;
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

inline Rgba RgbaMult(Rgba c, Ui32 m)
{
	Ui32 rb = c.rgba & 0x00ff00ff;
	Ui32 rbm = ((rb * m) >> 8) & 0x00ff00ff;
	Ui32 ga = (c.rgba >> 8) & 0x00ff00ff;
	Ui32 gam = ((ga * m)) & 0xff00ff00;
	return Rgba(rbm | gam);
}

inline Rgba RgbSum(Rgba x, Rgba y)
{
	// TODO: is there any hardware summation with saturation?
	return Rgba(
		Ui16(x.r) + Ui16(y.r) > 255 ? 255 : x.r + y.r,
		Ui16(x.g) + Ui16(y.g) > 255 ? 255 : x.g + y.g,
		Ui16(x.b) + Ui16(y.b) > 255 ? 255 : x.b + y.b,
		0
	);
}

inline Rgba RgbaSum(Rgba x, Rgba y)
{
	// TODO: is there any hardware summation with saturation?
	return Rgba(
		Ui16(x.r) + Ui16(y.r) > 255 ? 255 : x.r + y.r,
		Ui16(x.g) + Ui16(y.g) > 255 ? 255 : x.g + y.g,
		Ui16(x.b) + Ui16(y.b) > 255 ? 255 : x.b + y.b,
		Ui16(x.a) + Ui16(y.a) > 255 ? 255 : x.a + y.a
	);
}

struct FogFilter {
	// Config
	static constexpr Ui16 angSize = 1024;
	static constexpr float angDelta = 2.0f * float(M_PI) / angSize;
	static constexpr int maxSightDistance = 200;
	static constexpr int clearSightDistance = 140;
	const int minSightDistance = 7;
	const int minLight = 64;
	const int maxLight = 256;

	int x1;
	int y1;
	int x2;
	int y2;
	int cx;
	int cy;
	struct PixelData {
		int r = 0; // distance to the pixel center
		Ui16 a = 0; // ang[] index of center of the pixel

		// relevant ang[] indices are [rho1idx; rho2idx)
		Ui16 a1 = 0;
		Ui16 a2 = 0;
		int maxLight = 1;
	};
	std::vector<PixelData> pxl;
	std::vector<int> fadeLight;

	// State
	bool enabled = false;
	int darkenAll = 0;
	Ui16 fov_a1;
	Ui16 fov_a2;

	// Frame data
	struct AngleData {
		Ui16 sightDistance = maxSightDistance;
	};
	std::vector<AngleData> ang;

	FogFilter(int _x1, int _y1, int _x2, int _y2, int _cx, int _cy)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2)
		, cx(_cx), cy(_cy)
	{
		ang.resize(angSize);
		pxl.resize(screen::size);

		fadeLight.resize(screen::w + screen::h);
		for (size_t i = 0; i < fadeLight.size(); i++) {
			fadeLight[i] = std::max(minLight, 256 - 32 * int(i));
		}

		for (int y = y1; y <= y2; y++) {
			for (int x = x1; x <= x2; x++) {
				size_t pos = screen::pixel(x, y);
				PixelData& p = pxl[pos];
				int dx = x - cx;
				int dy = y - cy;
				p.r = (int)(sqrt(dx*dx + dy*dy) + 0.5f);
				p.maxLight = maxLight - (p.r - clearSightDistance) * (maxLight - minLight) / (maxSightDistance - clearSightDistance);
				p.maxLight = std::min(maxLight, std::max(minLight, p.maxLight));
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

	Ui16 Angle(double a)
	{
		return Ui16((a + 2 * M_PI) / angDelta) % angSize;
	}

	void Refresh()
	{
		enabled = false;
	}

	void Refresh(float a, float fov, int darken)
	{
		enabled = true;
		darkenAll = darken;
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
		for (Ui16 a = p.a1; a != p.a2; a++) {
			if (a == angSize) {
				a = 0;
			}
            ang[a].sightDistance = std::max<int>(FogFilter::minSightDistance, std::min<int>(ang[a].sightDistance, p.r));
		}
	}

	bool FovVisible(Ui16 a) const
	{
		if (fov_a1 <= fov_a2) { // special point outside FOV
			return a >= fov_a1 && a < fov_a2;
		}
		else { // special point inside FOV
			return a >= fov_a1 || a < fov_a2;
		}
	}

	Ui16 AngleDistance(Ui16 a, Ui16 b) const
	{
		if (a <= b) {
			return std::min(b - a, a + angSize - b);
		}
		return AngleDistance(b, a);
	}

	Ui16 OutOfFov(Ui16 a) const
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
			int directSight = ang[p.a].sightDistance;
			int directSightDelta = p.r - directSight;
			int l = std::min(p.maxLight, directSightDelta>0? fadeLight[directSightDelta]: 256);
			//int l = std::min(p.maxLight, std::max(minLight, 256 - 32 * directSightDelta));
			if (!FovVisible(p.a)) {
				int outOfFov = (int)OutOfFov(p.a);
				int darkness = std::min(outOfFov, std::max(0, p.r - 8) * 16);
				l = std::min(l, std::max(minLight, 256 - 4*darkness));
			}
			l = std::max(minLight, l - darkenAll);
			return RgbaMult(c, l);
		}
		return c;
	}
};

struct LinFilter {
	static constexpr int maxRadius = 200;

	// Config
	int x1;
	int y1;
	int x2;
	int y2;
	size_t w;
	size_t h;
	size_t size;
	struct PixelData {
		Si16 r16; // distance to the pixel center multiplied by 16
		Si16 x;
		Si16 y;
		Si32 pos_delta;
		bool operator<(const PixelData& rhs) const {
			return r16 < rhs.r16;
		}
	};
	std::vector<PixelData> pxl;

	// State
	int damageLeft = 0;
	int max_damage = 0;

	// Frame data
	std::vector<Rgba> data;

	LinFilter(int _x1, int _y1, int _x2, int _y2)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2)
		, w(x2 - x1 + 1), h(y2 - y1 + 1)
		, size(w * h)
		, data(screen::size)
	{
		for (int y = -maxRadius; y <= maxRadius; y++) {
			for (int x = -maxRadius; x <= maxRadius; x++) {
				int r16 = int(sqrt(x*x + y*y) * 16 + 0.5f);
				if (r16 <= maxRadius * 16) {
					PixelData p;
					p.r16 = r16;
					p.x = x;
					p.y = y;
					p.pos_delta = y * screen::w + x;
					pxl.push_back(p);
				}
			}
		}
		std::sort(pxl.begin(), pxl.end());
	}

	Rgba GradientColor(Rgba c1, Rgba c2, int count, int total)
	{
		if (total == 0) {
			return c2;
		}
		else {
			Ui32 m2 = count * 256 / total;
			Ui32 m1 = 256 - m2;
			Rgba v1 = RgbaMult(c1, m1);
			Rgba v2 = RgbaMult(c2, m2);
			return RgbaSum(v1, v2);
		}
	}

	void TraceLight(int x1, int y1, int x2, int y2, int /*width*/, Rgba c1, Rgba c2)
	{
		Si16 x, y;
		x = x2 - x1;
		y = y2 - y1;
		Si16 sx = sgn(x);
		Si16 sy = sgn(y);
		Si16 dx = abs(x);
		Si16 dy = abs(y);

		x = x1; y = y1;

		Si16 count = 0;
		if (dx >= dy)
		{
			Si16 bound = (dx + 1) >> 1;
			for (Si16 i = 0; i <= dx; i++)
			{
				ClipBlendPixel(x, y, GradientColor(c1, c2, i, dx));
				count += dy;
				if (count >= bound) { y += sy; count -= dx; }
				x += sx;
			}
		}
		else
		{
			Si16 bound = (dy + 1) >> 1;
			for (Si16 i = 0; i <= dy; i++)
			{
				ClipBlendPixel(x, y, GradientColor(c1, c2, i, dy));
				count += dx;
				if (count >= bound) { x += sx; count -= dy; }
				y += sy;
			}
		}
	}

	void SpotLight(int x, int y, int r, Rgba c)
	{
		int r16 = r << 4;
		float ak = float(c.a) / r16;
		size_t pos = screen::pixel(x, y);
		int r016 = 0;
		Rgba c0 = c;
		for (PixelData& p : pxl) {
			if (r16 < p.r16) {
				break;
			}
			if (r016 != p.r16) {
				r016 = p.r16;
				c0.a = c.a - ak * r016;
			}
			int x0 = x + p.x;
			int y0 = y + p.y;
			ClipBlendPixel(x0, y0, pos + p.pos_delta, c0);
		}
	}

	void ClipBlendPixel(int x, int y, Rgba c)
	{
		if (x >= x1 && y >= y1 && x <= x2 && y <= y2) {
			BlendPixel(screen::pixel(x, y), c);
		}
	}

	void ClipBlendPixel(int x, int y, size_t pos, Rgba c)
	{
		if (x >= x1 && y >= y1 && x <= x2 && y <= y2) {
			BlendPixel(pos, c);
		}
	}

	void BlendPixel(size_t pos, Rgba c)
	{
		// Transforms 3-way symmetric blending into 2-way blending 
		// x = k0 * v0 + (k1 * d + k2 * c)  --->   x = k0 * v0 + k * v
		// k0 + k1 + k2 = 1 => k0 = 1 - k1 - k2 => k = 1 - k0 = k1 + k2
		//
		// , where: k = (k1 * k1 + k2 * k2) / (k1 + k2)
		//          v = (k1 * d + k2 * c) / (k1 + k2)
		// Is this right way? TODO: Check the 4-way blending order does not matter
		Rgba& d = data[pos];
		if (d.a == 0) {
			d = c;
		}
		else {
			Ui32 ca = c.a;
			Ui32 da = d.a;
			Ui32 k = ca * 256 / (ca + da);
			Rgba v1 = RgbaMult(c, k);
			Rgba v2 = RgbaMult(d, 256 - k);
			d = RgbSum(v1, v2);
			d.a = 255 - ((255 - ca)*(255 - da) >> 8);
		}
	}

	Rgba Apply(size_t pos, Rgba c) const
	{
		Rgba d = data[pos];
		if (d.a == 0) {
			return c;
		}
		else {
			Ui16 da = d.a;
			Ui16 ca = 256 - da;
			Rgba v1 = RgbaMult(c, ca);
			Rgba v2 = RgbaMult(d, da);
			return RgbSum(v1, v2);
		}
	}

	void Refresh()
	{
		memset(&data[0], 0, sizeof(Rgba) * data.size());
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
	LinFilter* lin_;
	DmgFilter* dmg_;

	Rgba Apply(size_t pos, Rgba c) const
	{
		c = lin_->Apply(pos, c);
		c = fog_->Apply(pos, c);
		c = dmg_->Apply(pos, c);
		return c;
	}
};

void ApplyFilters(FilterList* filters, int x1, int y1, int x2, int y2);

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
void GetTextJust(BYTE& x, BYTE& y);
void SetTextJust(BYTE x, BYTE y);
BYTE LoadImage(char  *FileName,tIMAGE  *Img);
void FreeImage(tIMAGE  *Img);
void DrawSWImage(Si16 x,Si16 y,tIMAGE  *Img);
void DrawImage(Si16 x,Si16 y,tIMAGE  *Img);
void DrawWImage(Si16 x,Si16 y,tIMAGE  *Img);
void DrawSWUImage(Si16 x,Si16 y,BYTE uni,tIMAGE  *Img);
void DrawSImage(Si16 x,Si16 y,tIMAGE  *Img);
BYTE LoadFont(char  *FileName,tFONT  *Fnt);
void FreeFont(tFONT  *Fnt);
