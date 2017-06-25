#pragma once

/* 21 Mar 2003 bladez-fate */

#include "engine/easy.h"

template <class T>
bool KF(T t)
{
	return arctic::easy::IsKey(t);
}

template <class T>
void KeyDown(T t)
{
	arctic::easy::SetKey(t, false);
}

template <class T>
void KeyUp(T t)
{
	arctic::easy::SetKey(t, true);
}

using arctic::easy::MousePos;
