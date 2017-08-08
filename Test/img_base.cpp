/*#############################################################################
 * �ļ�����img_base.cpp
 * ���ܣ�  һЩ������ͼ�����
#############################################################################*/


#include "img_base.h"

#include "histogram.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

FvsError_t ImagePixelNormalize(FvsImage_t image, const FvsInt_t size)
{
	FvsByte_t* p1 = ImageGetBuffer(image);
	FvsByte_t* p2;
	FvsInt_t   w = ImageGetWidth(image);
	FvsInt_t   h = ImageGetHeight(image);
	FvsInt_t pitch = ImageGetPitch(image);
	FvsInt_t pitch2;
	FvsInt_t x, y, s, p, q, min, max;
	FvsFloat_t fgray;
	FvsImage_t im2;

	im2 = ImageCreate();

	if (im2 == NULL || p1 == NULL)
		return FvsMemory;

	s = size / 2;		/* size */
	if (size <= 0)
		return FvsBadParameter;

	/* copy image to make the computation */
	ImageCopy(im2, image);
	p2 = ImageGetBuffer(im2);
	if (p2 == NULL)
	{
		ImageDestroy(im2);
		return FvsMemory;
	}
	pitch2 = ImageGetPitch(im2);

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
		{
		min = 255;
		max = 0;
		for (q = -s; q <= s; q++)
			for (p = -s; p <= s; p++)
			{
			if (x + p >= 0 && x + p < w && y + q >= 0 && y + q < h) {
				max = p2[(x + p) + (y + q)*pitch2] > max ? p2[(x + p) + (y + q)*pitch2] : max;
				min = p2[(x + p) + (y + q)*pitch2] < min ? p2[(x + p) + (y + q)*pitch2] : min;
			}
			}
		//				fgray = (FvsFloat_t)p2[x + y*pitch2]*255.0/(FvsFloat_t)max*(1.0-(FvsFloat_t)min/255.0);
		fgray = ((FvsFloat_t)(p2[x + y*pitch2] - min)) / ((FvsFloat_t)(max - min)) * 255;
		if (fgray > 255.0)
			fgray = 255.0;
		if (fgray < 0)
			fgray = 0;

		p1[x + y*pitch] = fgray;
		}

	ImageDestroy(im2);
	return FvsOK;
}

/******************************************************************************
  * ���ܣ�ͼ���ֵ��
  * ������image       ָ��ͼ��
  *       size        ��ֵ
  * ���أ�������
******************************************************************************/
FvsError_t ImageBinarize(FvsImage_t image, const FvsByte_t limit) {
    FvsInt_t n;
    FvsByte_t *pimg = ImageGetBuffer(image);
    FvsInt_t size = ImageGetSize(image);
    if (pimg == NULL)
        return FvsMemory;
    /* ѭ������ */
    for (n = 0; n < size; n++, pimg++) {
        /* ��ֵ�� */
        *pimg = (*pimg < limit) ? (FvsByte_t)0xFF : (FvsByte_t)0x00;
    }
    return ImageSetFlag(image, FvsImageBinarized);
}

/******************************************************************************
  * ���ܣ�ͼ���ֵ��
  * ������ridgeimage  ԭָ��ͼ�����룬��ֵ����ͼ�����
  * ������valleyimage ��ֵ����ͼ��
  * ������highsize        ��ֵ
  * ������lowsize        ��ֵ
  * ���أ�������
******************************************************************************/
FvsError_t MyImageBinarize(FvsImage_t ridgeimage, FvsImage_t valleyimage, FvsByte_t highsize, FvsByte_t lowsize) {
    FvsInt_t n;
    FvsByte_t *ridge_pimg = ImageGetBuffer(ridgeimage);
    FvsByte_t *valley_pimg = ImageGetBuffer(valleyimage);
    FvsInt_t size = ImageGetSize(ridgeimage);
    if (ridge_pimg == NULL || valley_pimg == NULL)
        return FvsMemory;
    /* ѭ������ */
    for (n = 0; n < size; n++, ridge_pimg++, valley_pimg++) {
        /* ��ֵ�� */
        *ridge_pimg = (*ridge_pimg < lowsize) ? (FvsByte_t)0x00 : (FvsByte_t)0xFF;
        *valley_pimg = (*ridge_pimg > highsize) ? (FvsByte_t)0x00 : (FvsByte_t)0xFF;
    }
    ImageSetFlag(ridgeimage, FvsImageBinarized);
    ImageSetFlag(valleyimage, FvsImageBinarized);
    return FvsOK;
}


/******************************************************************************
  * ���ܣ�ͼ��ת����
  * ������image       ָ��ͼ��
  * ���أ�������
******************************************************************************/
FvsError_t ImageInvert(FvsImage_t image) {
    FvsByte_t* pimg = ImageGetBuffer(image);
    FvsInt_t size = ImageGetSize(image);
    FvsInt_t n;
    if (pimg == NULL)
        return FvsMemory;
    for (n = 0; n < size; n++, pimg++) {
        *pimg = 0xFF - *pimg;
    }
    return FvsOK;
}


/******************************************************************************
  * ���ܣ�ͼ��ϲ�����
  * ������image1    ��һ��ָ��ͼ�����ڱ�����
  *       image2    �ڶ���ָ��ͼ��
  * ���أ�������
******************************************************************************/
FvsError_t ImageAverage(FvsImage_t image1, const FvsImage_t image2) {
    FvsByte_t* p1 = ImageGetBuffer(image1);
    FvsByte_t* p2 = ImageGetBuffer(image2);
    FvsInt_t size1 = ImageGetSize(image1);
    FvsInt_t size2 = ImageGetSize(image2);
    FvsInt_t i;
    if (p1 == NULL || p2 == NULL)
        return FvsMemory;
    if (size1 != size2)
        return FvsBadParameter;
    for (i = 0; i < size1; i++, p1++) {
        *p1 = (*p1 + *p2++) >> 1;
    }
    return FvsOK;
}



/******************************************************************************
  * ���ܣ�ͼ���߼��ϲ�����
  * ������image1    ��һ��ָ��ͼ�����ڱ�����
  *       image2    �ڶ���ָ��ͼ��
  * ���أ�������
******************************************************************************/
FvsError_t ImageLogical
(
    FvsImage_t image1,
    const FvsImage_t image2,
    const FvsLogical_t operation
) {
    FvsByte_t* p1 = ImageGetBuffer(image1);
    FvsByte_t* p2 = ImageGetBuffer(image2);
    FvsInt_t size1 = ImageGetSize(image1);
    FvsInt_t i;
    if (p1 == NULL || p2 == NULL)
        return FvsMemory;
    if (ImageCompareSize(image1, image2) == FvsFalse)
        return FvsBadParameter;
    switch (operation) {
        case FvsLogicalOr:
            for (i = 0; i < size1; i++, p1++)
                *p1 = (*p1) | (*p2++);
            break;
        case FvsLogicalAnd:
            for (i = 0; i < size1; i++, p1++)
                *p1 = (*p1) & (*p2++);
            break;
        case FvsLogicalXor:
            for (i = 0; i < size1; i++, p1++)
                *p1 = (*p1) ^ (*p2++);
            break;
        case FvsLogicalNAnd:
            for (i = 0; i < size1; i++, p1++)
                *p1 = ~((*p1) & (*p2++));
            break;
        case FvsLogicalNOr:
            for (i = 0; i < size1; i++, p1++)
                *p1 = ~((*p1) | (*p2++));
            break;
        case FvsLogicalNXor:
            for (i = 0; i < size1; i++, p1++)
                *p1 = ~((*p1) ^ (*p2++));
            break;
    }
    return FvsOK;
}


/******************************************************************************
  * ���ܣ�ͼ��ϲ�����
  *       ʹ����ģ���㣬0��255�Ľ����0��������һ��������127��
  * ������image1    ��һ��ָ��ͼ�����ڱ�����
  *       image2    �ڶ���ָ��ͼ��
  * ���أ�������
******************************************************************************/
FvsError_t ImageAverageModulo(FvsImage_t image1, const FvsImage_t image2) {
    FvsByte_t* p1 = ImageGetBuffer(image1);
    FvsByte_t* p2 = ImageGetBuffer(image2);
    FvsInt_t size1 = ImageGetSize(image1);
    FvsInt_t size2 = ImageGetSize(image2);
    FvsInt_t i;
    FvsByte_t v1, v2;
    if (size1 != size2)
        return FvsBadParameter;
    if (p1 == NULL || p2 == NULL)
        return FvsMemory;
    for (i = 0; i < size1; i++) {
        v1 = *p1;
        v2 = *p2;
        if (v1 < 128) v1 += 256;
        if (v2 < 128) v2 += 256;
        v1 += v2;
        v1 >>= 1;
        v1 = v1 % 256;
        *p1++ = (uint8_t)v1;
    }
    return FvsOK;
}


/******************************************************************************
  * ���ܣ�ͼ��ƽ�Ʋ���
  * ������image    ָ��ͼ��
  *       vx       X�����ƽ����
  *       vy       Y�����ƽ����
  * ���أ�������
******************************************************************************/
FvsError_t ImageTranslate(FvsImage_t image, const FvsInt_t vx, const FvsInt_t vy) {
    return FvsOK;
}


#define P(x,y)      p[((x)+(y)*pitch)]


/******************************************************************************
  * ���ܣ�ͼ������
  * ������image       ָ��ͼ��
  *       horizontal  ˮƽ��ֱ����
  * ���أ�������
******************************************************************************/
FvsError_t ImageStripes(FvsImage_t image, const FvsBool_t horizontal) {
    FvsByte_t* p = ImageGetBuffer(image);
    FvsInt_t w     = ImageGetWidth (image);
    FvsInt_t h     = ImageGetHeight(image);
    FvsInt_t pitch = ImageGetPitch (image);
    FvsInt_t x, y;
    if (p == NULL)
        return FvsMemory;
    if (horizontal == FvsFalse) {
        for (y = 0; y < h; y++)
            for (x = 0; x < w; x++)
                P(x, y) = (FvsByte_t)x % 256;
    }
    else {
        for (y = 0; y < h; y++)
            for (x = 0; x < w; x++)
                P(x, y) = (FvsByte_t)y % 256;
    }
    return FvsOK;
}


/******************************************************************************
  * ���ܣ��ı�ͼ��ķ���ȣ�ʹ����[255..255]֮��䶯
  * ������image         ָ��ͼ��
  *       luminosity    ��صķ����
  * ���أ�������
******************************************************************************/
FvsError_t ImageLuminosity(FvsImage_t image, const FvsInt_t luminosity) {
    FvsByte_t* p = ImageGetBuffer(image);
    FvsInt_t  w = ImageGetWidth (image);
    FvsInt_t  h = ImageGetHeight(image);
    FvsInt_t pitch = ImageGetPitch (image);
    FvsInt_t x, y;
    FvsFloat_t fgray, a, b;
    if (p == NULL)
        return FvsMemory;
    if (luminosity > 0) {
        a = (255.0 - abs(luminosity)) / 255.0;
        b = (FvsFloat_t)luminosity;
    }
    else {
        a = (255.0 - abs(luminosity)) / 255.0;
        b = 0.0;
    }
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            fgray = (FvsFloat_t)P(x, y);
            fgray = b + a * fgray;
            if (fgray < 0.0)    fgray = 0.0;
            if (fgray > 255.0)  fgray = 255.0;
            P(x, y) = (uint8_t)fgray;
        }
    return FvsOK;
}


/******************************************************************************
  * ���ܣ��ı�ͼ��ĶԱȶȣ�ʹ����[-127..127]�䶯
  * ������image      ָ��ͼ��
  *       contrast   �Աȶ�����
  * ���أ�������
******************************************************************************/
FvsError_t ImageContrast(FvsImage_t image, const FvsInt_t contrast) {
    FvsByte_t* p = ImageGetBuffer(image);
    FvsInt_t  w = ImageGetWidth (image);
    FvsInt_t  h = ImageGetHeight(image);
    FvsInt_t pitch = ImageGetPitch (image);
    FvsInt_t x, y;
    FvsFloat_t fgray, a, b;
    if (p == NULL)
        return FvsMemory;
    a = (FvsFloat_t)((127.0 + contrast) / 127.0);
    b = (FvsFloat_t)(-contrast);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            fgray = (FvsFloat_t)P(x, y);
            fgray = b + a * fgray;
            if (fgray < 0.0)    fgray = 0.0;
            if (fgray > 255.0)  fgray = 255.0;
            P(x, y) = (uint8_t)fgray;
        }
    return FvsOK;
}


/******************************************************************************
  * ���ܣ�ͼ����������ͨ�������ֵʵ��
  * ������image     ָ��ͼ��
  *       size      �����ڴ�С
  * ���أ�������
******************************************************************************/
FvsError_t ImageSoftenMean(FvsImage_t image, const FvsInt_t size) {
    FvsByte_t* p1  = ImageGetBuffer(image);
    FvsByte_t* p2;
    FvsInt_t   w   = ImageGetWidth (image);
    FvsInt_t   h   = ImageGetHeight(image);
    FvsInt_t pitch = ImageGetPitch (image);
    FvsInt_t pitch2;
    FvsInt_t x, y, s, p, q, a, c;
    FvsImage_t im2;
    im2 = ImageCreate();
    if (im2 == NULL || p1 == NULL)
        return FvsMemory;
    s = size >> 1;		/* ��С */
    a = size * size;	/* ��� */
    if (a == 0)
        return FvsBadParameter;
    /* ����ͼ����м��� */
    ImageCopy(im2, image);
    p2 = ImageGetBuffer(im2);
    if (p2 == NULL) {
        ImageDestroy(im2);
        return FvsMemory;
    }
    pitch2 = ImageGetPitch (im2);
    for (y = s; y < h - s; y++)
        for (x = s; x < w - s; x++) {
            c = 0;
            for (q = -s; q <= s; q++)
                for (p = -s; p <= s; p++) {
                    c += p2[(x + p) + (y + q) * pitch2];
                }
            p1[x + y * pitch] = c / a;
        }
    ImageDestroy(im2);
    return FvsOK;
}


/******************************************************************************
  * ���ܣ�ͼ���һ��������ʹ����и����ľ�ֵ�ͷ���
  * ������image     ָ��ͼ��
  *       mean      �����ľ�ֵ
  *       variance  �����ı�׼����
  * ���أ�������
******************************************************************************/
FvsError_t ImageNormalize(FvsImage_t image, const FvsByte_t mean, const FvsUint_t variance) {
    FvsByte_t* p = ImageGetBuffer(image);
    FvsInt_t   w = ImageGetWidth (image);
    FvsInt_t   h = ImageGetHeight(image);
    FvsInt_t   pitch = ImageGetPitch (image);
    FvsInt_t   x, y;
    FvsFloat_t fmean, fsigma, fmean0, fsigma0, fgray;
    FvsFloat_t fcoeff = 0.0;
    FvsHistogram_t histogram = NULL;
    FvsError_t nRet;
    if (p == NULL)
        return FvsMemory;
    histogram = HistogramCreate();
    if (histogram != NULL) {
        /* ����ֱ��ͼ */
        nRet = HistogramCompute(histogram, image);
        if (nRet == FvsOK) {
            /* ���㷽��;�ֵ */
            fmean   = (FvsFloat_t)HistogramGetMean(histogram);
            fsigma  = sqrt((FvsFloat_t)HistogramGetVariance(histogram));
            fmean0  = (FvsFloat_t)mean;
            fsigma0 = sqrt((FvsFloat_t)variance);
           /* if (fsigma > 0.0)
                fcoeff = fsigma0 / fsigma;
            for (y = 0; y < h; y++)
                for (x = 0; x < w; x++) {
                    fgray = (FvsFloat_t)P(x, y);
                    fgray = fmean0 + fcoeff * (fgray - mean);
                    if (fgray < 0.0)    fgray = 0.0;
                    if (fgray > 255.0)  fgray = 255.0;
                    P(x, y) = (uint8_t)fgray;
                }*/

			if (fsigma > 0.0)
			    fcoeff = fsigma0 / fsigma;
			for (y = 0; y < h; y++)
			   for (x = 0; x < w; x++) 
			   {
					fgray = (FvsFloat_t)P(x, y);
					fgray =fmean0 + fsigma0 * (fgray - fmean) / fsigma;
					if (fgray < 0.0)    fgray = 0.0;
					if (fgray > 255.0)  fgray = 255.0;
					P(x, y) = (uint8_t)fgray;
			}


        }
        HistogramDestroy(histogram);
    }
    return nRet;
}


/******************************************************************************
* ���ܣ�ͼ���һ��������ʹ����и����ľ�ֵ�ͷ���
* ������image     ָ��ͼ��
*       mean      �����ľ�ֵ
*       variance  �����ı�׼����
* ���أ�������
******************************************************************************/
FvsError_t ImageNormalize(FvsImage_t image) 
{
	FvsByte_t* p = ImageGetBuffer(image);
	FvsInt_t   w = ImageGetWidth(image);
	FvsInt_t   h = ImageGetHeight(image);
	FvsInt_t   pitch = ImageGetPitch(image);
	FvsInt_t   x, y;
	FvsFloat_t fmean, fsigma, fmean0, fsigma0, fgray;
	FvsFloat_t fcoeff = 0.0;
	FvsHistogram_t histogram = NULL;
	FvsError_t nRet = FvsOK;
	if (p == NULL)
		return FvsMemory;
	FvsInt_t i, j;
	
	FvsFloat_t nn[256],pp[256], cc[256];
	for (i = 0; i < 256; i++)
	{
		nn[i] = 0;
		pp[i] = 0;
		cc[i] = 0;
	}

	for (i = 0; i<h; i++)
	{
		for (j = 0; j<w; j++){
			nn[P(i,j)]++;
		}
	}
	for (i = 0; i<256; i++)
	{
		pp[i] = nn[i]/(w*h);
	}

	for (i = 0; i < 256; i++)
	{
		for (j = 0; j <= i; j++)
		{
			cc[i] += pp[j];
		}
	}

	FvsInt_t max, min;
    max = min = P(0,0);
	for (i = 0; i<h; i++){
		for (j = 0; j<w; j++){
			if (max<P(i,j))
			{
				max = P(i,j);
			}
			else if (min>P(i, j))
			{
				min = P(i, j);
			}
		}
	}

	for (i = 0; i<h; i++){
		for (j = 0; j<w; j++){
			P(i,j)= cc[P(i,j)] * (max - min) + min;
		}
	}

	
	return nRet;
}


