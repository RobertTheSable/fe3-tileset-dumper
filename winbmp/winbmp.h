#pragma once 

#include <windows.h>
#include <wingdi.h>
#include <fileapi.h>

#include "tileset.h"

struct BitMapHolder {
    unsigned int* bitPointer;
    BITMAPINFO bitmap;
    HBITMAP handle;
    BitMapHolder() {
        bitPointer = nullptr;
        HDC hdcScreen = GetDC(NULL);

        bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
        bitmap.bmiHeader.biWidth = 512;
        // the height needs ot be negative or else the final bmp is upside down
        bitmap.bmiHeader.biHeight = -512;
        bitmap.bmiHeader.biPlanes = 1;
        bitmap.bmiHeader.biBitCount = 32;
        bitmap.bmiHeader.biCompression = BI_RGB;
        bitmap.bmiHeader.biSizeImage = 512 * 512 * sizeof(unsigned int);
        bitmap.bmiHeader.biClrUsed = 0;
        bitmap.bmiHeader.biClrImportant = 0;

        handle = CreateDIBSection(hdcScreen, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, 0);
        ReleaseDC(NULL, hdcScreen);
    }
    ~ BitMapHolder() {
        DeleteObject(handle);
        delete [] bitPointer;
    }
    bool draw(Tileset& t, const Chapter& chapter) const;
};

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
void CreateBMPFile(LPCWSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP);
