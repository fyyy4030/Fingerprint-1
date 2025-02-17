/*#############################################################################
 * 文件名：img_base.h
 * 功能：  一些基本的图像操作
 * modified by  PRTsinghua@hotmail.com
#############################################################################*/

#if !defined FVS__IMAGE_BASE_HEADER__INCLUDED__
#define FVS__IMAGE_BASE_HEADER__INCLUDED__

#include "image.h"

typedef enum FvsLogical_t
{
    FvsLogicalOr   = 1,
    FvsLogicalAnd  = 2,
    FvsLogicalXor  = 3,
    FvsLogicalNAnd = 4,
    FvsLogicalNOr  = 5,
    FvsLogicalNXor = 6,
} FvsLogical_t;


FvsError_t ImagePixelNormalize(FvsImage_t image, const FvsInt_t size);

/******************************************************************************
  * 功能：图像二值化
  * 参数：image       指纹图像
  *       size        阈值
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageBinarize(FvsImage_t image, const FvsByte_t limit);


/******************************************************************************
  * 功能：图像二值化
  * 参数：ridgeimage  原指纹图像输入，二值化脊图像输出
  * 参数：valleyimage 二值化谷图像
  * 参数：highsize        阈值
  * 参数：lowsize        阈值
  * 返回：错误编号
******************************************************************************/
FvsError_t MyImageBinarize(FvsImage_t ridgeimage,FvsImage_t valleyimage,FvsByte_t highsize,FvsByte_t lowsize);



/******************************************************************************
  * 功能：图像翻转操作
  * 参数：image       指纹图像
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageInvert(FvsImage_t image);


/******************************************************************************
  * 功能：图像合并操作
  * 参数：image1    第一个指纹图像，用于保存结果
  *       image2    第二个指纹图像
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageAverage(FvsImage_t image1, const FvsImage_t image2);


/******************************************************************************
  * 功能：图像合并操作
  *       使用了模计算，0和255的结果是0而不是上一个函数的127。
  * 参数：image1    第一个指纹图像，用于保存结果
  *       image2    第二个指纹图像
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageAverageModulo(FvsImage_t image1, const FvsImage_t image2);


/******************************************************************************
  * 功能：图像逻辑合并操作
  * 参数：image1    第一个指纹图像，用于保存结果
  *       image2    第二个指纹图像
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageLogical(FvsImage_t image1,  const FvsImage_t image2,  const FvsLogical_t operation);


/******************************************************************************
  * 功能：图像平移操作
  * 参数：image    指纹图像
  *       vx       X方向的平移量
  *       vy       Y方向的平移量
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageTranslate(FvsImage_t image, const FvsInt_t vx, const FvsInt_t vy);


/******************************************************************************
  * 功能：图像纹理
  * 参数：image       指纹图像
  *       horizontal  水平或垂直纹理
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageStripes(FvsImage_t image, const FvsBool_t horizontal);


/******************************************************************************
  * 功能：图像归一化操作，使其具有给定的均值和方差
  * 参数：image     指纹图像
  *       mean      给定的均值
  *       variance  给定的标准方差
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageNormalize(FvsImage_t image, const FvsByte_t mean, const FvsUint_t variance);

///////////////////////////////////////////////////////////////////////////////
FvsError_t ImageNormalize(FvsImage_t image);




/******************************************************************************
  * 功能：改变图像的发光度，使其在[255..255]之间变动
  * 参数：image         指纹图像
  *       luminosity    相关的发光度
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageLuminosity(FvsImage_t image, const FvsInt_t luminosity);


/******************************************************************************
  * 功能：改变图像的对比度，使其在[-127..127]变动
  * 参数：image      指纹图像
  *       contrast   对比度因子
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageContrast(FvsImage_t image, const FvsInt_t contrast);


/******************************************************************************
  * 功能：图像软化操作，通过计算均值实现
  * 参数：image     指纹图像
  *       size      软化窗口大小
  * 返回：错误编号
******************************************************************************/
FvsError_t ImageSoftenMean(FvsImage_t image, const FvsInt_t size);


#endif /* FVS__IMAGE_BASE_HEADER__INCLUDED__ */


