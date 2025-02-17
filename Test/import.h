/*#############################################################################
 * 文件名：import2.h
 * 功能：  加载图像
 * modified by  PRTsinghua@hotmail.com
#############################################################################*/

#if !defined FVS__IMPORT_HEADER__INCLUDED__
#define FVS__IMPORT_HEADER__INCLUDED__

#include "file.h"
#include "image.h"


/******************************************************************************
  * 功能：从文件中加载指纹图像
  * 参数：image       指纹图像
  *       filename    文件名
  * 返回：错误编号
******************************************************************************/
extern FvsError_t FvsImageImport(FvsImage_t image, const FvsString_t filename,
		FvsByte_t bmfh[14],BITMAPINFOHEADER *bmih,RGBQUAD *rgbq);


/******************************************************************************
* 功能：从字节流中加载指纹图像
* 参数：image       指纹图像
*       data        字节数组
* 返回：错误编号
******************************************************************************/
extern FvsError_t FvsImageImport2(FvsImage_t image, const char* data,const int n,
	FvsByte_t bmfh[14], BITMAPINFOHEADER *bmih, RGBQUAD *rgbq);

#endif /* FVS__IMPORT_HEADER__INCLUDED__ */

