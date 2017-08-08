//#include <opencv2/opencv.hpp>
//#include <string>
//#include <stdio.h>
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <strstream>
//#include "fvs.h"
//
//using namespace std;
//using namespace cv;
//
//int main(int argc, char *argv[]){
//	FvsImage_t image;
//	FvsImage_t mask;
//	FvsImage_t temp;
//	FvsFloatField_t direction;
//	FvsMinutiaSet_t minutia_original;
//	FvsMinutiaSet_t minutia;
//	FvsFloat_t pgoodness;
//	FvsFloatField_t frequency;
//
//	image = ImageCreate();
//	mask = ImageCreate();
//	temp = ImageCreate();
//	direction = FloatFieldCreate();
//	frequency = FloatFieldCreate();
//	minutia = MinutiaSetCreate(200, "Persong0001", 1);
//	minutia_original = MinutiaSetCreate(200, "Persong0001", 1);
//
//	char *orifilename = "E:\\测试程序2\\测试图像\\crd_0000f_01.bmp";
//	//char *orifilename = "E:\\测试程序2\\测试图像\\tim2g.jpg";
//
//	string fname;
//	cv::Mat matrixJprg = imread(orifilename);
//	cv::Mat gray;
//	cvtColor(matrixJprg, gray, CV_BGR2GRAY);
//
//	IplImage pGray2 = gray;
//	imwrite("K_Step1.jpg", gray);
//
//	image = ImageCreate(&pGray2);
//
//	ImageSoftenMean(image, 3);
//
//	ImagePixelNormalize(image, 9);
//
//	(void)FingerprintGetFixedMask(image, mask, 6);
//
//	(void)FingerprintGetDirection_k(image, direction, 7, 4);
//
//	(void)ImageLogical(image, mask, FvsLogicalAnd);
//
//	(void)FingerprintGetFrequency(image, direction, frequency);
//
//	(void)ImageEnhanceGabor(image, direction, frequency, mask, 4.0);
//
//	(void)ImageBinarize(image, (FvsByte_t)0x80);
//
//	(void)ImageThinHitMiss(image);
//
//	FvsByte_t* buffer = ImageGetBuffer(image);
//	FvsByte_t bb;
//
//	buffer = ImageGetBuffer(image);
//	fname = "K_Thin.jpg";
//	cv::Mat mt = Mat(pGray2.height, pGray2.width, CV_8UC1);
//	for (int ii = 0; ii < pGray2.height; ii++)
//	{
//		for (int jj = 0; jj < pGray2.width; jj++)
//		{
//			bb = *(buffer + ii*pGray2.width + jj);
//			mt.at<uchar>(ii, jj) = bb;
//
//		}
//	}
//	imwrite(fname, mt);
//	//转opencv的图像，进行存储
//
//
//	MinutiaSetExtract(minutia, image, direction, mask);////////////////////////////////就在这开始改吧，这个可以做细节点的过滤
//
//	bool genPic = true;
//	if (genPic) {
//		ImageClear(image);
//		MinutiaSetDraw(minutia, image);
//
//		/*fname = "_minu.bmp";
//		FvsImageExport(image, fname, bmfh, &bmih, rgbq);*/
//		buffer = ImageGetBuffer(image);
//		fname = "_MinutiaSet.jpg";
//		for (int ii = 0; ii < pGray2.height; ii++)
//		{
//			for (int jj = 0; jj < pGray2.width; jj++)
//			{
//				bb = *(buffer + ii*pGray2.width + jj);
//				mt.at<uchar>(ii, jj) = bb;
//
//			}
//		}
//
//		imwrite(fname, mt);
//
//
//	}
//
//	string str = GetMinutiaBuffer(minutia);
//
//	ofstream File;
//	File.open("_1111.txt", ios::out);
//	File << str << endl;
//	File.close();
//
//
//
//	//cvReleaseImage(&pSrcImage);
//	//cvReleaseImage(&pGrayImage);
//
//
//	return 0;
//
//}