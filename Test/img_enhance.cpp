/*#############################################################################
 * �ļ�����imageenhance.cpp
 * ���ܣ�  ʵ����ͼ����ǿ�㷨
#############################################################################*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "imagemanip.h"


/******************************************************************************
** ͼ����ǿ����
**
** ����ǿ�㷨���ָ��ͼ����ƣ��������ָ��ͼ����û��ʹ�õ����򣬶�����������
** ����ǿ�󣬼��߿��Ա������ķ��������ʹ��һ����ֵ����
**
** ���㷨������һ�����߷���ͼ��һ������ͼ��
**
** �ɲο�������ƪ���£�
** 1 - Fingerprint Enhancement: Lin Hong, Anil Jain, Sharathcha Pankanti,
**     and Ruud Bolle. [Hong96]
** 2 - Fingerprint Image Enhancement, Algorithm and Performance Evaluation:
**     Lin Hong, Yifei Wan and Anil Jain. [Hong98]
**
** ��ǿ�㷨ʹ���� ����(2) �еļ������裺
**  A - ��һ��
**  B - ���㷽��ͼ
**  C - ����Ƶ��
**  D - ������������
**  E - �˲�
**
******************************************************************************/

//#define P(x,y)      ((int32_t)p[(x)+(y)*pitch])
#define P(x,y)      ((x)+(y)*pitch)
#define P1  p[P(x  ,y  )]
#define P2  p[P(x  ,y-1)]
#define P3  p[P(x+1,y-1)]
#define P4  p[P(x+1,y  )]
#define P5  p[P(x+1,y+1)]
#define P6  p[P(x  ,y+1)]
#define P7  p[P(x-1,y+1)]
#define P8  p[P(x-1,y  )]
#define P9  p[P(x-1,y-1)]


/******************************************************************************
** ������Gabor�����˲��������£�
**
**                    / 1|x'     y'  |\
** h(x,y:phi,f) = exp|- -|--- + ---| |.cos(2.PI.f.x')
**                    \ 2|dx     dy  |/
**
** x' =  x.cos(phi) + y.sin(phi)
** y' = -x.sin(phi) + y.cos(phi)
**
** �������£�
**  G ��һ�����ͼ��
**  O ����ͼ
**  F Ƶ��ͼ
**  R ����ͼ��
**  E ��ǿ���ͼ��
**  Wg Gabor�˲������ڴ�С
**
**          / 255                                          if R(i,j) = 0
**         |
**         |  Wg/2    Wg/2
**         |  ---     ---
** E(i,j)= |  \       \
**         |   --      --  h(u,v:O(i,j),F(i,j)).G(i-u,j-v) otherwise
**         |  /       /
**          \ ---     ---
**            u=-Wg/2 v=-Wg/2
**
******************************************************************************/
FvsFloat_t EnhanceGabor(FvsFloat_t x, FvsFloat_t y, FvsFloat_t phi,
                        FvsFloat_t f, FvsFloat_t r2) {
    FvsFloat_t dy2 = 1.0 / r2;
    FvsFloat_t dx2 = 1.0 / r2;
    FvsFloat_t x2, y2;
//    phi += M_PI/2;
//    x2 = -x*sin(phi) + y*cos(phi);
//    y2 =  x*cos(phi) + y*sin(phi);
    x2 = x * cos(phi) + y * sin(phi);
    y2 = - x * sin(phi) + y * cos(phi);
    return exp(-0.5 * (x2 * x2 * dx2 + y2 * y2 * dy2)) * cos(2 * M_PI * x2 * f);
}

static FvsError_t ImageEnhanceFilter
(
    FvsImage_t        normalized,
    const FvsImage_t  mask,
    const FvsFloat_t* orientation,
    const FvsFloat_t* frequence,
    FvsFloat_t        radius
) {
    FvsInt_t Wg2 = 8;
    FvsInt_t i, j, u, v;
    FvsError_t nRet  = FvsOK;
    FvsImage_t enhanced = NULL;
    FvsInt_t w        = ImageGetWidth (normalized);
    FvsInt_t h        = ImageGetHeight(normalized);
    FvsInt_t pitchG   = ImageGetPitch (normalized);
    FvsByte_t* pG     = ImageGetBuffer(normalized);
    FvsFloat_t sum, f, o;
    FvsFloat_t x2, y2, cosdir, sindir;
    /* ƽ�� */
    radius = radius * radius;
    enhanced = ImageCreate();
    if (enhanced == NULL || pG == NULL)
        return FvsMemory;
    if (nRet == FvsOK)
        nRet = ImageSetSize(enhanced, w, h);
    if (nRet == FvsOK) {
        FvsInt_t pitchE  = ImageGetPitch (enhanced);
        FvsByte_t* pE    = ImageGetBuffer(enhanced);
        if (pE == NULL)
            return FvsMemory;
        (void)ImageClear(enhanced);
        for (j = Wg2; j < h - Wg2; j++)
            for (i = Wg2; i < w - Wg2; i++) {
                if (mask == NULL || ImageGetPixel(mask, i, j) != 0) {
                    sum = 0.0;
                    o = orientation[i + j * w];
                    f = frequence[i + j * w];
                    cosdir = cos(o);
                    sindir = sin(o);
                    /*				if(i<140 && i>120 )
                                                            sum=0.0;
                                                    if(j<135 && j>120 && i==130)
                                                            sum=0.0;
                    */
                    for (v = -Wg2; v <= Wg2; v++)
                        for (u = -Wg2; u <= Wg2; u++) {
                            /*sum += EnhanceGabor
                                                        (
                                                        (FvsFloat_t)u,
                                                                        (FvsFloat_t)v,
                                                                        o,f,radius
                                                                )
                                                                * pG[(i+u)+(j+v)*pitchG];*/
                            x2 = u * cosdir + v * sindir;
                            y2 = - u * sindir + v * cosdir;
                            sum += exp(-0.5 * (x2 * x2 / radius + y2 * y2 / radius)) * cos(2 * M_PI * x2 * f) * pG[(i + u) + (j + v) * pitchG];
                        }
                    if (sum > 255.0)
                        sum = 255.0;
                    if (sum < 0.0)
                        sum = 0.0;
                    pE[i + j * pitchE] = (uint8_t)sum;
                }
            }
        nRet = ImageCopy(normalized, enhanced);
    }
    (void)ImageDestroy(enhanced);
    return nRet;
}


/******************************************************************************
** ������Gabor�����˲��������£�
**	���ݴ��ڲ��÷��ϼ��߷����16*16��
**
******************************************************************************/
static FvsError_t ImageEnhanceFilter1
(
    FvsImage_t        normalized,
    const FvsImage_t  mask,
    const FvsFloat_t* orientation,
    const FvsFloat_t* frequence,
    FvsFloat_t        radius
) {
    FvsInt_t Wg2 = 8;
    FvsInt_t i, j, u, v, x, y, x1, y1;
    FvsError_t nRet  = FvsOK;
    FvsImage_t enhanced = NULL;
    FvsInt_t w        = ImageGetWidth (normalized);
    FvsInt_t h        = ImageGetHeight(normalized);
    FvsInt_t pitchG   = ImageGetPitch (normalized);
    FvsByte_t* pG     = ImageGetBuffer(normalized);
    FvsFloat_t sum, f, o, o1, o2, rate = 1.0;
    FvsFloat_t cosdir, sindir, x2, y2;
    /* ƽ�� */
    radius = radius * radius;
    enhanced = ImageCreate();
    if (enhanced == NULL || pG == NULL)
        return FvsMemory;
    if (nRet == FvsOK)
        nRet = ImageSetSize(enhanced, w, h);
    if (nRet == FvsOK) {
        FvsInt_t pitchE  = ImageGetPitch (enhanced);
        FvsByte_t* pE    = ImageGetBuffer(enhanced);
        if (pE == NULL)
            return FvsMemory;
        (void)ImageClear(enhanced);
        for (j = Wg2; j < h - Wg2; j++)
            for (i = Wg2; i < w - Wg2; i++) {
                if (mask == NULL || ImageGetPixel(mask, i, j) != 0) {
                    sum = 0.0;
                    o = orientation[i + j * w];
                    f = frequence[i + j * w];
                    cosdir = cos(o);
                    sindir = sin(o);
                    x = (FvsInt_t)(-Wg2 / 2 * sindir) + i;
                    y = (FvsInt_t)(Wg2 / 2 * cosdir) + j;
                    o1 = frequence[x + y * w];
                    x = (FvsInt_t)(Wg2 / 2 * sindir) + i;
                    y = (FvsInt_t)(-Wg2 / 2 * cosdir) + j;
                    o2 = frequence[x + y * w];
                    /*				if(i>68 && i<139 && j>43 && j<175 && (1.0-4*fabs(o1-o2))<rate)
                                                    {
                                                            rate=1.0-4*fabs(o1-o2);
                                                            x1=i;
                                                            y1=j;
                                                    }*/
                    rate = fabs(1.0 - 10 * fabs(o1 - o2));
//				if(i<140 && i>120 )
//					sum=0.0;
//				if(j<73 && j>70 && i==130)
//					sum=0.0;
                    for (v = -Wg2; v <= Wg2; v++)
                        for (u = -Wg2; u <= Wg2; u++) {
                            x2 = u * cosdir + v * sindir;
                            y2 = - u * sindir + v * cosdir;
                            sum += exp(-0.5 * (x2 * x2 / rate / radius + y2 * y2 / rate / radius)) \
                                   *cos(2 * M_PI * x2 * f) * pG[(i + u) + (j + v) * pitchG];
                        }
                    if (sum > 255.0)
                        sum = 255.0;
                    if (sum < 0.0)
                        sum = 0.0;
                    pE[i + j * pitchE] = (uint8_t)sum;
                }
            }
        nRet = ImageCopy(normalized, enhanced);
    }
    (void)ImageDestroy(enhanced);
    return nRet;
}


static FvsError_t ImageEnhanceFilter2
(
    FvsImage_t        normalized,
    const FvsImage_t  mask,
    const FvsFloat_t* orientation,
    const FvsFloat_t* frequence,
    FvsFloat_t        radius
) {
    FvsInt_t Wg2 = 8;
    FvsInt_t i, j, u, v;
    FvsError_t nRet  = FvsOK;
    FvsImage_t enhanced = NULL;
    FvsInt_t w        = ImageGetWidth (normalized);
    FvsInt_t h        = ImageGetHeight(normalized);
    FvsInt_t pitchG   = ImageGetPitch (normalized);
    FvsByte_t* pG     = ImageGetBuffer(normalized);
    FvsFloat_t sum, f, o;
    FvsFloat_t x2, y2, cosdir, sindir;
    FvsFloat_t expv[17][17];
    radius = radius * radius;
    for (v = -Wg2; v <= Wg2; v++)
        for (u = -Wg2; u <= Wg2; u++) {
            expv[8 + v][8 + u] = exp(-0.5 * (u * u + v * v) / radius);
        }
    enhanced = ImageCreate();
    if (enhanced == NULL || pG == NULL)
        return FvsMemory;
    if (nRet == FvsOK)
        nRet = ImageSetSize(enhanced, w, h);
    if (nRet == FvsOK) {
        FvsInt_t pitchE  = ImageGetPitch (enhanced);
        FvsByte_t* pE    = ImageGetBuffer(enhanced);
        if (pE == NULL)
            return FvsMemory;
        (void)ImageClear(enhanced);
        for (j = Wg2; j < h - Wg2; j++)
            for (i = Wg2; i < w - Wg2; i++) {
                if (mask == NULL || ImageGetPixel(mask, i, j) != 0) {
                    sum = 0.0;
                    o = orientation[i + j * w];
                    f = frequence[i + j * w];
                    cosdir = cos(o);
                    sindir = sin(o);
                    if(i < 140 && i > 120 && j == 85 )
                        sum = 0.0;
                    if(j < 128 && j > 120 && i == 162)
                        sum = 0.0;
                    for (v = -Wg2; v <= Wg2; v++)
                        for (u = -Wg2; u <= Wg2; u++) {
                            x2 = u * cosdir + v * sindir;
//    y2 = - u*sindir + v*cosdir;
                            sum += expv[8 + v][8 + u] * cos(2 * M_PI * x2 * f) * pG[(i + u) + (j + v) * pitchG];
                        }
                    if (sum > 255.0)
                        sum = 255.0;
                    if (sum < 0.0)
                        sum = 0.0;
                    pE[i + j * pitchE] = (uint8_t)sum;
                }
                else	pE[i + j * pitchE] = 255;
            }
        nRet = ImageCopy(normalized, enhanced);
    }
    (void)ImageDestroy(enhanced);
    return nRet;
}



/******************************************************************************
  * ���ܣ�ָ��ͼ����ǿ�㷨
  *       ���㷨���������Ƚϸ��ӣ������Ĳ����ǻ���Gabor�˲����ģ�
          ������̬���㡣ͼ����ʱ�������θı䣬����Ҫ��һ��ԭͼ�ı��ݡ�
  * ������image        ָ��ͼ��
  *       direction    ���߷�����Ҫ���ȼ���
  *       frequency    ����Ƶ�ʣ���Ҫ���ȼ���
  *       mask         ָʾָ�Ƶ���Ч����
  *       radius       �˲����뾶�����������£�4.0���ɡ�
                       ֵԽ�����������ܵ��������ƣ�������������α������
  * ���أ�������
******************************************************************************/
FvsError_t ImageEnhanceGabor(FvsImage_t image, const FvsFloatField_t direction,
                             const FvsFloatField_t frequency, const FvsImage_t mask,
                             const FvsFloat_t radius) {
    FvsError_t nRet = FvsOK;
    FvsFloat_t * image_orientation = FloatFieldGetBuffer(direction);
    FvsFloat_t * image_frequence   = FloatFieldGetBuffer(frequency);
    if (image_orientation == NULL || image_frequence == NULL)
        return FvsMemory;
    nRet = ImageEnhanceFilter2(image, mask, image_orientation,
                               image_frequence, radius);
    return nRet;
}

FvsInt_t RemoveSpursRec(FvsImage_t image, FvsInt_t x, FvsInt_t y, FvsInt_t lastpos, FvsInt_t distance)
{
	FvsInt_t w = ImageGetWidth(image);
	FvsInt_t h = ImageGetHeight(image);
	FvsInt_t pitch = ImageGetPitch(image);
	FvsByte_t* p = ImageGetBuffer(image);
	FvsInt_t sum, ret;

	sum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;

	//ignore image border
	if (x == 1 || y == 1 || x == w - 2 || y == w - 2)
	{

		if (distance < 15)
		{
			P1 = 0;
			return 1;
		}
		else

			return 0;
	}
	//went far enough
	else if (distance > 20)
	{
		return 0;
	}
	//normal line or start of Spur
	else if (sum == 510 || distance == 0)
	{
		if (P2 == 0xFF && P(x, y - 1) != lastpos)
			ret = RemoveSpursRec(image, x, y - 1, P(x, y), distance + 1);
		else if (P3 == 0xFF && P2 == 0 && P4 == 0 && P(x + 1, y - 1) != lastpos)
			ret = RemoveSpursRec(image, x + 1, y - 1, P(x, y), distance + 1);
		else if (P4 == 0xFF && P(x + 1, y) != lastpos)
			ret = RemoveSpursRec(image, x + 1, y, P(x, y), distance + 1);
		else if (P5 == 0xFF && P4 == 0 && P6 == 0 && P(x + 1, y + 1) != lastpos)
			ret = RemoveSpursRec(image, x + 1, y + 1, P(x, y), distance + 1);
		else if (P6 == 0xFF && P(x, y + 1) != lastpos)
			ret = RemoveSpursRec(image, x, y + 1, P(x, y), distance + 1);
		else if (P7 == 0xFF && P6 == 0 && P8 == 0 && P(x - 1, y + 1) != lastpos)
			ret = RemoveSpursRec(image, x - 1, y + 1, P(x, y), distance + 1);
		else if (P8 == 0xFF && P(x - 1, y) != lastpos)
			ret = RemoveSpursRec(image, x - 1, y, P(x, y), distance + 1);
		else // P9 == 0xFF && P(x-1,y-1) != lastpos
			ret = RemoveSpursRec(image, x - 1, y - 1, P(x, y), distance + 1);
	}
	// isolated line, may be of length 20
	else if (sum == 255)
	{
		P1 = 0;
		return 1;
	}
	//Spur - may be of length 15
	else if (sum > 510 && distance < 15)
	{
		p[lastpos] = 0;
		if ((P9 + P2 + P3 > 0 && P7 + P6 + P5 > 0) || (P9 + P8 + P7 > 0 && P3 + P4 + P5 > 0))
		{
		}//Pixel is a relevant connector, leave it at 1
		else
			P1 = 0;
		return 1;
	}
	//shouldn't get there
	else
	{
		return 0;
	}

	if (ret == 1)
	{
		P1 = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

FvsInt_t FindCirclesRec(FvsImage_t image, FvsInt_t x, FvsInt_t y, FvsInt_t lastpos, FvsInt_t *targetpos, FvsInt_t distance)
{
	FvsInt_t pitch = ImageGetPitch(image);
	FvsByte_t* p = ImageGetBuffer(image);
	FvsInt_t sum, ret;

	sum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
	//either a normal line segment (sum == 510) or a false minitua
	if (distance > 30)
	{
		*targetpos = -1;
		return -1;
	}
	else if (sum == 510 || (sum > 510 && ((P2 && P3) || (P3 && P4) || (P4 && P5) || (P5 && P6) || (P6 && P7) || (P7 && P8) || (P8 && P9) || (P9 && P2))))
	{
		//P2,P4,P6,P8 first
		if (P2 == 0xFF && P(x, y - 1) != lastpos)
			ret = FindCirclesRec(image, x, y - 1, P(x, y), targetpos, distance + 1);
		else if (P4 == 0xFF && P(x + 1, y) != lastpos)
			ret = FindCirclesRec(image, x + 1, y, P(x, y), targetpos, distance + 1);
		else if (P6 == 0xFF && P(x, y + 1) != lastpos)
			ret = FindCirclesRec(image, x, y + 1, P(x, y), targetpos, distance + 1);
		else if (P8 == 0xFF && P(x - 1, y) != lastpos)
			ret = FindCirclesRec(image, x - 1, y, P(x, y), targetpos, distance + 1);
		else if (P3 == 0xFF && P2 == 0 && P4 == 0 && P(x + 1, y - 1) != lastpos)
			ret = FindCirclesRec(image, x + 1, y - 1, P(x, y), targetpos, distance + 1);
		else if (P5 == 0xFF && P4 == 0 && P6 == 0 && P(x + 1, y + 1) != lastpos)
			ret = FindCirclesRec(image, x + 1, y + 1, P(x, y), targetpos, distance + 1);
		else if (P7 == 0xFF && P6 == 0 && P8 == 0 && P(x - 1, y + 1) != lastpos)
			ret = FindCirclesRec(image, x - 1, y + 1, P(x, y), targetpos, distance + 1);
		else //if(P9 == 0xFF && P8 == 0 && P2 == 0 && P(x-1,y-1) != lastpos)
			ret = FindCirclesRec(image, x - 1, y - 1, P(x, y), targetpos, distance + 1);
	}
	//next minutia reached may be of length 15
	else if (sum > 510)
	{
		*targetpos = P(x, y);
		return distance;
	}
	else
	{
		return -1;
	}
	return ret;
}

static inline FvsFloat_t anglediff(FvsFloat_t a, FvsFloat_t b)
{
	FvsFloat_t dif;

	dif = a - b;

	if (dif > -M_PI / 2 && dif <= M_PI / 2)
		return dif;
	else if (dif <= -M_PI / 2)
		return (M_PI + dif);
	else
		return (M_PI - dif);
}

FvsError_t ImageEnhanceThinned(FvsImage_t image, FvsFloatField_t direction)
{
	FvsInt_t w = ImageGetWidth(image);
	FvsInt_t h = ImageGetHeight(image);
	FvsInt_t pitch = ImageGetPitch(image);
	FvsByte_t* p = ImageGetBuffer(image);
	FvsFloat_t* dir = FloatFieldGetBuffer(direction);
	FvsFloat_t diffangle;
	FvsInt_t x, y, sum, targetpos;
	FvsInt_t i, j, ret, bifurcation, sumdist[8], sumtarget[8], sumstart[8];


	//First remove spurs with length of up to 15 or isolated lines with length of up to 20
	for (y = 1; y<h - 1; y++)
		for (x = 1; x<w - 1; x++)
		{
		if (P1 == 0xFF)
		{
			sum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
			//isolated pixel
			if (sum == 0) {
				P1 = 0;
				continue;
			}
			//starting point
			if (sum == 255) {
				RemoveSpursRec(image, x, y, -1, 0);
			}
		}
		}

	//Then remocve cycles
	for (y = 1; y<h - 1; y++)
		for (x = 1; x<w - 1; x++)
		{
		if (P1 == 0xFF)
		{
			sum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
			//bisection point
			//look in every direction to find another bifurcation
			if (sum > 510 && !((P2 && P3) || (P3 && P4) || (P4 && P5) || (P5 && P6) || (P6 && P7) || (P7 && P8) || (P8 && P9) || (P9 && P2)))
			{
				//	      	RemoveSpursRec(image,x,y,-1, 0);
				//	      	fprintf(stdout, "%d,%d\n",x,y);
				bifurcation = 0;

				if (P2 == 0xFF)
				{
					ret = FindCirclesRec(image, x, y - 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x, y - 1); bifurcation++;
					}
				}
				if (P3 == 0xFF)
				{
					ret = FindCirclesRec(image, x + 1, y - 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x + 1, y - 1); bifurcation++;
					}
				}
				if (P4 == 0xFF)
				{
					ret = FindCirclesRec(image, x + 1, y, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x + 1, y); bifurcation++;
					}
				}
				if (P5 == 0xFF)
				{
					ret = FindCirclesRec(image, x + 1, y + 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x + 1, y + 1); bifurcation++;
					}
				}
				if (P6 == 0xFF)
				{
					ret = FindCirclesRec(image, x, y + 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x, y + 1); bifurcation++;
					}
				}
				if (P7 == 0xFF)
				{
					ret = FindCirclesRec(image, x - 1, y + 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x - 1, y + 1); bifurcation++;
					}
				}
				if (P8 == 0xFF)
				{
					ret = FindCirclesRec(image, x - 1, y, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x - 1, y); bifurcation++;
					}
				}
				if (P9 == 0xFF)
				{
					ret = FindCirclesRec(image, x - 1, y - 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x - 1, y - 1); bifurcation++;
					}
				}

				//					fprintf(stdout, "%d,%d\n",x,y);
				//now look if a circle was found
				for (i = 0; i<bifurcation; i++)
					for (j = i + 1; j<bifurcation; j++)
					{
					if (sumdist[i] > 0 && sumdist[j] > 0)
					{
						if (sumtarget[i] == sumtarget[j])
							if (sumdist[i] > sumdist[j])
							{
							//remove line i
							RemoveSpursRec(image, sumstart[i] % pitch, (FvsInt_t)(sumstart[i] / pitch), P(x, y), 0);
							}
							else
							{
								//remove line j
								RemoveSpursRec(image, sumstart[j] % pitch, (FvsInt_t)(sumstart[j] / pitch), P(x, y), 0);
							}
					}
					}
			}
		}
		}

	//finally remove connections between parrallel ridges (very similar algorithm to circle removement)
	for (y = 1; y<h - 1; y++)
		for (x = 1; x<w - 1; x++)
		{
		if (P1 == 0xFF)
		{
			sum = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
			//bisection point
			//look in every direction to find another bifurcation
			if (sum > 510 && !((P2 && P3) || (P3 && P4) || (P4 && P5) || (P5 && P6) || (P6 && P7) || (P7 && P8) || (P8 && P9) || (P9 && P2)))
			{
				bifurcation = 0;

				if (P2 == 0xFF)
				{
					ret = FindCirclesRec(image, x, y - 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x, y - 1); bifurcation++;
					}
				}
				if (P3 == 0xFF)
				{
					ret = FindCirclesRec(image, x + 1, y - 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x + 1, y - 1); bifurcation++;
					}
				}
				if (P4 == 0xFF)
				{
					ret = FindCirclesRec(image, x + 1, y, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x + 1, y); bifurcation++;
					}
				}
				if (P5 == 0xFF)
				{
					ret = FindCirclesRec(image, x + 1, y + 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x + 1, y + 1); bifurcation++;
					}
				}
				if (P6 == 0xFF)
				{
					ret = FindCirclesRec(image, x, y + 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x, y + 1); bifurcation++;
					}
				}
				if (P7 == 0xFF)
				{
					ret = FindCirclesRec(image, x - 1, y + 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x - 1, y + 1); bifurcation++;
					}
				}
				if (P8 == 0xFF)
				{
					ret = FindCirclesRec(image, x - 1, y, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x - 1, y); bifurcation++;
					}
				}
				if (P9 == 0xFF)
				{
					ret = FindCirclesRec(image, x - 1, y - 1, P(x, y), &targetpos, 1);
					if (ret != -1)
					{
						sumdist[bifurcation] = ret; sumtarget[bifurcation] = targetpos; sumstart[bifurcation] = P(x - 1, y - 1); bifurcation++;
					}
				}

				//fprintf(stdout, "%d,%d\n",x,y);
				//now look if a normal line was found
				for (i = 0; i<bifurcation; i++)
				{
					diffangle = atan2(y - sumtarget[i] / pitch, x - sumtarget[i] % pitch);
					// x < M_PI/8 because in direction matrix the directions are stored as normals to the ridge directions
					if (anglediff(dir[P(x, y)], diffangle) < M_PI / 8)
					{
						RemoveSpursRec(image, sumstart[i] % pitch, (FvsInt_t)(sumstart[i] / pitch), P(x, y), 0);
						break;
					}
				}
			}
		}
		}

	return FvsOK;
}
