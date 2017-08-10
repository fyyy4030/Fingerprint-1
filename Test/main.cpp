#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <strstream>
#include "fvs.h"


using namespace std;
using namespace cv;


void my_imfillholes(Mat &src)
{
	// detect external contours
	//
	vector<vector<Point> > contours;
	
	Mat hierarchy;
	findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//
	// fill external contours
	//
	if (!contours.empty() && !hierarchy.empty())
	{
		for (int idx = 0; idx < contours.size(); idx++)
		{
			drawContours(src, contours, idx, Scalar::all(255), CV_FILLED, 8);
		}
	}
}


//Otsu算法获取二值化阈值
int Otsu(IplImage* src)
{
	int height = src->height;
	int width = src->width;

	//计算直方图    
	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)
	{
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep * i;
		for (int j = 0; j < width; j++)
		{
			histogram[*p++]++;
		}
	}
	//直方图均衡    
	int size = height * width;
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = histogram[i] / size;
	}

	//平均    
	float avgValue = 0;
	for (int i = 0; i < 256; i++)
	{
		avgValue += i * histogram[i];   
	}

	int threshold;
	float maxVariance = 0;
	float w = 0, u = 0;
	for (int i = 0; i < 256; i++)
	{
		w += histogram[i];  //假设当前灰度i为阈值, 0~i 灰度的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例  
		u += i * histogram[i];  // 灰度i 之前的像素(0~i)的平均灰度值： 前景像素的平均灰度值  

		float t = avgValue * w - u;
		float variance = t * t / (w * (1 - w));
		if (variance > maxVariance)
		{
			maxVariance = variance;
			threshold = i;
		}
	}

	return threshold;
}


//static FvsError_t ImagePreprocess(FvsImage_t image){
//	FvsError_t nRet = FvsOK;
//	FvsImage_t processed = NULL;
//	FvsInt_t w = ImageGetWidth(image);
//	FvsInt_t h = ImageGetHeight(image);
//	FvsByte_t* pG = ImageGetBuffer(image);
//	processed = ImageCreate();
//	
//	if (processed == NULL || pG == NULL)
//		return FvsMemory;
//	if (nRet == FvsOK)
//		nRet = ImageSetSize(processed, w, h);
//	if (nRet == FvsOK){
//		FvsByte_t* pE = ImageGetBuffer(processed);
//		if (pE == NULL)
//			return FvsMemory;
//		(void)ImageClear(processed);
//		for (int i = 0; i < h; i++){
//			
//		}
//	}
//	(void)ImageDestroy(processed);
//	return nRet;
//}


int main()
{

	char *orifilename = "E:\\测试程序2\\测试图像\\crd_0000f_01.bmp";
	//char *orifilename = "E:\\测试程序2\\测试图像\\tim2g.jpg";
	string fname;
	cv::Mat matrixJprg = imread(orifilename);
	

	cv::Mat gray;

	cvtColor(matrixJprg, gray, CV_BGR2GRAY);//转灰度图

	////计算直方图
	//const int channels[1] = { 0 };
	//const int histSize[1] = { 256 };
	//float hranges[2] = { 0, 255 };
	//const float* ranges[1] = { hranges };
	//cv::MatND hist;
	//calcHist(&gray, 1, channels, cv::Mat(), hist, 1, histSize, ranges);
	//float table[256];
	//int nPix = gray.cols * gray.rows;
	//for (int i = 0; i < 256; i++){
	//	float temp[256];
	//	temp[i] = hist.at<float>(i) / nPix * 255;
	//	if (i != 0){
	//		table[i] = table[i - 1] + temp[i];
	//	}
	//	else{
	//		table[i] = temp[i];
	//	}
	//}
	//cv::Mat lookUpTable(cv::Size(1, 256), CV_8U);
	//for (int i = 0; i < 256; i++){
	//	lookUpTable.at<uchar>(i) = static_cast<uchar>(table[i]);
	//}
	//cv::Mat engray;
	//cv::LUT(gray, lookUpTable, engray);

	//IplImage pGray2 = engray;

	//imwrite("_Step0.jpg", engray);
	////cvSaveImage("_Step0.jpg", &pGray2);

	IplImage pGray2 = gray;

	imwrite("_Step0.jpg", gray);
	//cvSaveImage("_Step0.jpg", &pGray2);


	string  tname;
	FvsImage_t mask;
	FvsImage_t image;
	FvsImage_t image2;
	FvsImage_t directionimage;
	FvsMinutiaSet_t minutia = MinutiaSetCreate(1000,"Persong0001",1);
	FvsFloatField_t direction;
	FvsFloatField_t frequency;
	FvsFloatField_t frequency3;
	//FvsByte_t bmfh[14];
	//BITMAPINFOHEADER bmih;
	//RGBQUAD rgbq[256];

	//FvsHistogram_t histogram;
	
	mask = ImageCreate();
	image = ImageCreate();
	image2 = ImageCreate();

	directionimage = ImageCreate();
	direction = FloatFieldCreate();
	frequency = FloatFieldCreate();
	frequency3 = FloatFieldCreate();
	//histogram = HistogramCreate();

	image = ImageCreate(&pGray2);

	ImageSetSize(image, pGray2.width, pGray2.height);

	FvsByte_t* buffer = ImageGetBuffer(image);

	/* 拷贝数据 */
	memcpy(buffer, pGray2.imageData, pGray2.width*pGray2.height*sizeof(FvsByte_t));

	FvsInt_t w = ImageGetWidth(image);
	FvsInt_t h = ImageGetHeight(image);

	//转opencv的图像，进行存储
	fname = "_Step1.jpg";
	FvsByte_t bb;
	
	buffer = ImageGetBuffer(image);
	cv::Mat mt = Mat(pGray2.height,pGray2.width, CV_8UC1);

	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;
	
		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储
	
	ImageSetSize(directionimage, w, h);

	IplImage thres = mt;
	int threshold = Otsu(&thres);

	//HistogramCompute(histogram, image);
	//ImagePreprocess(image);

	////转opencv的图像，进行存储
	//buffer = ImageGetBuffer(image);
	//fname = "_Preprocess.jpg";
	//for (int ii = 0; ii < pGray2.height; ii++)
	//{
	//	for (int jj = 0; jj < pGray2.width; jj++)
	//	{
	//		bb = *(buffer + ii*pGray2.width + jj);
	//		mt.at<uchar>(ii, jj) = bb;

	//	}
	//}

	//imwrite(fname, mt);
	////转opencv的图像，进行存储

	////图像翻转
	//ImageInvert(image);
	////转opencv的图像，进行存储
	//buffer = ImageGetBuffer(image);
	//fname = "_Invert.jpg";
	//for (int ii = 0; ii < pGray2.height; ii++)
	//{
	//	for (int jj = 0; jj < pGray2.width; jj++)
	//	{
	//		bb = *(buffer + ii*pGray2.width + jj);
	//		mt.at<uchar>(ii, jj) = bb;

	//	}
	//}

	//imwrite(fname, mt);
	////转opencv的图像，进行存储

	//IplImage thres = mt;
	//int threshold = Otsu(&thres);


	//计算直方图
	//计算均值
	//计算方差
	//图像归一化
	//cout << HistogramGetMean(histogram);
	//cout << HistogramGetVariance(histogram);
	//ImageNormalize(image, HistogramGetMean(histogram), HistogramGetVariance(histogram));

	ImageSoftenMean(image, 3);
	buffer = ImageGetBuffer(image);
	fname = "_GetSoften.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}
	imwrite(fname, mt);
	//转opencv的图像，进行存储




	FingerprintGetDirection(image, direction, 7, 3);
	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(image);
	fname = "_GetDir.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储
	


	FingerprintGetFrequency3(image, direction, frequency3);
	FingerprintGetFrequency1(image, direction, frequency);
	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(image);
	fname = "_GetFre.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储


	FingerprintGetMask(image, direction, frequency3, mask);///////////////////////////////////获取指纹有效区

	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(mask);
	fname = "_Mask.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储


	//FingerprintGetFixedMask(image, mask, 9);
	////转opencv的图像，进行存储
	//buffer = ImageGetBuffer(mask);
	//fname = "_FixedMask.jpg";
	//for (int ii = 0; ii < pGray2.height; ii++)
	//{
	//	for (int jj = 0; jj < pGray2.width; jj++)
	//	{
	//		bb = *(buffer + ii*pGray2.width + jj);
	//		mt.at<uchar>(ii, jj) = bb;

	//	}
	//}

	//imwrite(fname, mt);
	////转opencv的图像，进行存储

	ImagePixelNormalize(image, 9);
	buffer = ImageGetBuffer(image);
	fname = "_GetNormalize.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}
	imwrite(fname, mt);
	//转opencv的图像，进行存储

	ImageEnhanceGabor(image, direction, frequency, mask, 4);

	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(image);
	fname = "_EnhanceGabor.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储

	bool genPic = true;
	//saving file....
	if (genPic) {
		
		/*fname = "_dir.bmp";
		FvsImageExport(directionimage, fname, bmfh, &bmih, rgbq);
		
		fname = "_mask.bmp";
		FvsImageExport(mask, fname, bmfh, &bmih, rgbq);
	
	
		fname = "_enh.bmp";
		FvsImageExport(image, fname, bmfh, &bmih, rgbq);
	*/
	}

	


	ImageBinarize(image, threshold);
	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(image);
	fname = "_Binarize.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储

	ImageThinHitMiss(image);///////////////////////////////////////

	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(image);
	fname = "_ThinHitMiss.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储


	
	ImageEnhanceThinned(image, direction);
	//转opencv的图像，进行存储
	buffer = ImageGetBuffer(image);
	fname = "_ThinEnhance.jpg";
	for (int ii = 0; ii < pGray2.height; ii++)
	{
		for (int jj = 0; jj < pGray2.width; jj++)
		{
			bb = *(buffer + ii*pGray2.width + jj);
			mt.at<uchar>(ii, jj) = bb;

		}
	}

	imwrite(fname, mt);
	//转opencv的图像，进行存储

	if (genPic) {
		
	/*	fname = "_bin.bmp";
		FvsImageExport(image, fname, bmfh, &bmih, rgbq);*/
	
	}
	

	if (genPic) {
		
		/*fname = "_thin.bmp";
		FvsImageExport(image, fname, bmfh, &bmih, rgbq);*/
	
	}
	MinutiaSetExtract(minutia, image, direction, mask);////////////////////////////////就在这开始改吧，这个可以做细节点的过滤
	

	if (genPic) {
		ImageClear(image);
		MinutiaSetDraw(minutia, image);
		
		/*fname = "_minu.bmp";
		FvsImageExport(image, fname, bmfh, &bmih, rgbq);*/
		buffer = ImageGetBuffer(image);
		fname = "_MinutiaSet.jpg";
		for (int ii = 0; ii < pGray2.height; ii++)
		{
			for (int jj = 0; jj < pGray2.width; jj++)
			{
				bb = *(buffer + ii*pGray2.width + jj);
				mt.at<uchar>(ii, jj) = bb;

			}
		}

		imwrite(fname, mt);

	
	}

	string str = GetMinutiaBuffer(minutia);

	ofstream File;
	File.open("_1111.txt", ios::out);
	File << str<< endl;
	File.close();



	//cvReleaseImage(&pSrcImage);
	//cvReleaseImage(&pGrayImage);


	return 0;








//FvsImage_t image;
//FvsImage_t mask;
//FvsImage_t temp;
//FvsFloatField_t direction;
//FvsMinutiaSet_t minutia_original;
//FvsMinutiaSet_t minutia;
//FvsFloat_t pgoodness;
//FvsFloatField_t frequency;
//
//image = ImageCreate();
//mask = ImageCreate();
//temp = ImageCreate();
//direction = FloatFieldCreate();
//frequency = FloatFieldCreate();
//minutia = MinutiaSetCreate(200, "Persong0001", 1);
//minutia_original = MinutiaSetCreate(200, "Persong0001", 1);
//
//char *orifilename = "E:\\测试程序2\\测试图像\\crd_0000f_01.bmp";
////char *orifilename = "E:\\测试程序2\\测试图像\\tim2g.jpg";
//
//string fname;
//cv::Mat matrixJprg = imread(orifilename);
//cv::Mat gray;
//cvtColor(matrixJprg, gray, CV_BGR2GRAY);
//
//IplImage pGray2 = gray;
//imwrite("K_Step1.jpg", gray);
//
//image = ImageCreate(&pGray2);
//
//ImageSoftenMean(image, 3);
//FvsByte_t* buffer = ImageGetBuffer(image);
//FvsByte_t bb;
//
//buffer = ImageGetBuffer(image);
//fname = "K_Thin.jpg";
//cv::Mat mt = Mat(pGray2.height, pGray2.width, CV_8UC1);
//for (int ii = 0; ii < pGray2.height; ii++)
//{
//	for (int jj = 0; jj < pGray2.width; jj++)
//	{
//		bb = *(buffer + ii*pGray2.width + jj);
//		mt.at<uchar>(ii, jj) = bb;
//
//	}
//}
//imwrite(fname, mt);
////转opencv的图像，进行存储
//ImagePixelNormalize(image, 9);
//
//(void)FingerprintGetFixedMask(image, mask, 6);
//
//(void)FingerprintGetDirection(image, direction, 7, 4);
//
//(void)ImageLogical(image, mask, FvsLogicalAnd);
//
//(void)FingerprintGetFrequency(image, direction, frequency);
//
//(void)ImageEnhanceGabor(image, direction, frequency, mask, 4.0);
//
//(void)ImageBinarize(image, (FvsByte_t)0x80);
//
//(void)ImageThinHitMiss(image);
//
//
//
//
//MinutiaSetExtract(minutia, image, direction, mask);////////////////////////////////就在这开始改吧，这个可以做细节点的过滤
//
//bool genPic = true;
//if (genPic) {
//	ImageClear(image);
//	MinutiaSetDraw(minutia, image);
//
//	/*fname = "_minu.bmp";
//	FvsImageExport(image, fname, bmfh, &bmih, rgbq);*/
//	buffer = ImageGetBuffer(image);
//	fname = "_MinutiaSet.jpg";
//	for (int ii = 0; ii < pGray2.height; ii++)
//	{
//		for (int jj = 0; jj < pGray2.width; jj++)
//		{
//			bb = *(buffer + ii*pGray2.width + jj);
//			mt.at<uchar>(ii, jj) = bb;
//
//		}
//	}
//
//	imwrite(fname, mt);
//
//
//}
//
//string str = GetMinutiaBuffer(minutia);
//
//ofstream File;
//File.open("_1111.txt", ios::out);
//File << str << endl;
//File.close();
//
//
//
////cvReleaseImage(&pSrcImage);
////cvReleaseImage(&pGrayImage);
//
//
//return 0;

}