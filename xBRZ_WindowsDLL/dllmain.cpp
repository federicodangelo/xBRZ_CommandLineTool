// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include "../xBRZ/xbrz.h"

extern "C" __declspec(dllexport)
void scale(int scaleFactor, uint32_t *originalPixels, uint32_t *resizedPixels, int originalWidth, int originalHeight)
{
	xbrz::scale(scaleFactor, originalPixels, resizedPixels, originalWidth, originalHeight, xbrz::ColorFormat::ARGB);
}