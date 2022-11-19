#include "winbmp.h"
#include "winbase.h"

#include <stdexcept>
#include <sstream>

#include "LunarDLL.h"

// too lazy to figure out another way to setup another way to write bmp data
// that would be compatible with Lunar's functions.

// taken from https://learn.microsoft.com/en-us/windows/win32/gdi/storing-an-image

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height.  
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) {
        throw std::runtime_error("GetObject failed.");
    }
        

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) {
        cClrBits = 1; 
    } else if (cClrBits <= 4) {
        cClrBits = 4; 
    } else if (cClrBits <= 8) {
        cClrBits = 8; 
    } else if (cClrBits <= 16) {
        cClrBits = 16; 
    } else if (cClrBits <= 24) {
        cClrBits = 24; 
    } else { 
        cClrBits = 32; 
    }

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

     if (cClrBits < 24) {
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits));
     } else {
         // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 
     }

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = 512; 
    pbmi->bmiHeader.biHeight = 512; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) {
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
    }

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0; 
    return pbmi; 
}

void CreateBMPFile(LPCWSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP) 
{ 
    HDC hDC = GetDC(NULL);
    HANDLE hf;                  // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits) {
        throw std::runtime_error("GlobalAlloc failed.");
    }
         

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)) {
        throw std::runtime_error("GetDIBits"); 
    }

    // Create the .BMP file.  
    hf = CreateFileW(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("CreateFile failed."); 
    }
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
       throw std::runtime_error("Header WriteFile failed."); 
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, ( NULL))) {
        throw std::runtime_error("RGB WriteFile failed."); 
    }

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) {
        throw std::runtime_error("WriteFile failed"); 
    }

    // Close the .BMP file.  
     if (!CloseHandle(hf)) {
        throw std::runtime_error("CloseHandle failed."); 
     }

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
    ReleaseDC(NULL, hDC);
}

bool BitMapHolder::draw(Tileset& tData, const Chapter& chapter) const {
    if (this->handle == nullptr) {
        return false;
    }
    int x = 0, y = 0, quarter = 0;
    for (auto& tile: tData.tiles) {
        int xCoord = (x * 16) + ((quarter % 2) * 8);
        int yCoord = (y * 16) + ((quarter / 2) * 8);

        LunarRender8x8(
            this->bitPointer,
            512,
            512,
            xCoord,
            yCoord,
            (void*)&tData.pixmap[0],
            &tData.palettes.colors[0],
            tile,
            LC_DRAW
        );
        ++quarter;
        if (quarter == 4) {
            ++x;
            if (x == 32) {
                x = 0;
                ++y;
            }
            quarter = 0;
        }
    }
    auto info = CreateBitmapInfoStruct(this->handle);
    std::wostringstream filenamestrm;
    filenamestrm << chapter << L".bmp";
    CreateBMPFile(filenamestrm.str().c_str(), info, this->handle);

    LocalFree(info);
    return true;
}
