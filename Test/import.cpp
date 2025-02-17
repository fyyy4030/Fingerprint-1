/*#############################################################################
 * 文件名：import.cpp
 * 功能：  一些基本的图像操作
#############################################################################*/


#include "import.h"

#include <stdio.h>
#include <fstream>
#include <strstream>
using namespace std;
#define DIB_HEADER_MARKER   ((FvsWord_t) ('M' << 8) | 'B')


/******************************************************************************
  * 功能：从文件中加载指纹图像
  * 参数：image       指纹图像
  *       filename    文件名
  * 返回：错误编号
******************************************************************************/
FvsError_t FvsImageImport(FvsImage_t image, const FvsString_t filename,
                          FvsByte_t bmfh[14], BITMAPINFOHEADER *bmih, RGBQUAD *rgbq) {
    FvsError_t ret = FvsOK;
    FvsByte_t*    buffer;
    FvsInt_t      pitch;
    FvsInt_t      height;
    FvsInt_t      width;
    FvsInt_t i,j, x, y;
    FvsFile_t	file;
//	FvsByte_t bmfh0[14];
//	BITMAPINFOHEADER bmih0;
    file	  = FileCreate();
    if(FileOpen(file, filename, FvsFileRead) == FvsFailure) {
        ret = FvsFailure;
    }
    if(FileRead(file, bmfh, 14) != 14)
        ret = FvsFailure;
    if(*(FvsWord_t*)bmfh != DIB_HEADER_MARKER)
        ret = FvsFailure;
    if(FileRead(file, bmih, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER))
        ret = FvsFailure;
   if(bmih->biBitCount != 8)
        ret = FvsFailure;
    if(FileRead(file, rgbq, sizeof(RGBQUAD) * 256) != sizeof(RGBQUAD) * 256)
        ret = FvsFailure;
    if(ret == FvsFailure) {
        printf("File format or Read file error");
        return ret;
    }
    else {
        ret = ImageSetSize(image, (FvsInt_t)bmih->biWidth, (FvsInt_t)bmih->biHeight);
        if (ret == FvsOK) {
            /* 获得缓冲区 */
            buffer = ImageGetBuffer(image);
            pitch  = ImageGetPitch(image);
            height = ImageGetHeight(image);
            width  = ImageGetWidth(image);
            x = *(FvsDword_t*)(bmfh + 10);
            /* 拷贝数据 */
            for (i = 0; i < height; i++) {
                y = (height - 1 - i) * WIDTHBYTES(width * 8);
                FileSeek(file, x + y);
                FileRead(file, buffer + i * pitch, pitch);
            }
			/*FvsUint8_t  bb;
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{

					FileRead(file,&bb, sizeof(FvsUint8_t));

					*(buffer + i*pitch + j)= bb;
				}
				
			}*/
        }
    }
    FileDestroy(file);
    return ret;
}


/******************************************************************************
* 功能：从字节流中加载指纹图像
* 参数：image       指纹图像
*       data    字节流
* 返回：错误编号
******************************************************************************/
FvsError_t FvsImageImport2(FvsImage_t image, const char* data, const int n,
	FvsByte_t bmfh[14], BITMAPINFOHEADER *bmih, RGBQUAD *rgbq) {
	FvsError_t ret = FvsOK;
	FvsByte_t*    buffer;
	FvsInt_t      pitch;
	FvsInt_t      height;
	FvsInt_t      width;
	FvsInt_t i, x, y;

	istrstream is(data,n);
	
	if (!is.read((char*)bmfh, 14))
		ret = FvsFailure;
	if (*(FvsWord_t*)bmfh != DIB_HEADER_MARKER)
		ret = FvsFailure;
	if (!is.read((char*)bmih, sizeof(BITMAPINFOHEADER)))
		ret = FvsFailure;
	if (bmih->biBitCount != 8)
		ret = FvsFailure;
	if (!is.read((char*)rgbq, sizeof(RGBQUAD)* 256))
		ret = FvsFailure;
	if (ret == FvsFailure) {
		printf("File format or Read file error");
		return ret;
	}
	else {
		ret = ImageSetSize(image, (FvsInt_t)bmih->biWidth, (FvsInt_t)bmih->biHeight);
		if (ret == FvsOK) {
			/* 获得缓冲区 */
			buffer = ImageGetBuffer(image);
			pitch = ImageGetPitch(image);
			height = ImageGetHeight(image);
			width = ImageGetWidth(image);
			x = *(FvsDword_t*)(bmfh + 10);
			/* 拷贝数据 */
			for (i = 0; i < height; i++) {
				y = (height - 1 - i) * WIDTHBYTES(width * 8);
				is.seekg(x+y);
				is.read((char*)buffer + i * pitch, pitch);
			}
		}
	}

	is.clear();
	
	return ret;
}


