#include "opencv.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "Digit_Finder.h"

using namespace std;
using namespace cv;
using namespace ml;

DigitFinder::DigitFinder(int x)
{
	alter = x;
	digitX = 0; digitY = 0; digitWidth = 0; digitHeight = 0;
	predictData = Mat::zeros(1, digitSize*digitSize, CV_32FC1);
	predictResponce = Mat(1, 1, CV_32FC1);
	knn = Algorithm::load<KNearest>("(NewTables)my.yml");
}

Point2i DigitFinder::p2i_getDigitLoc()
{
	return Point2i((digitX + digitWidth/2-320)/640, (digitY + digitHeight/2-240)/480);
}

bool DigitFinder::isDigit_BW(Rect boundRect, Mat& img)
{
	if (boundRect.height >= 60 && boundRect.width <= 260)
	{
		int x = boundRect.x; int height = boundRect.height;
		int y = boundRect.y; int width = boundRect.width;
		int xwidth = x + width;
		int yheight = y + height;
		Vec3b* data1 = img.ptr<Vec3b>(y);
		Vec3b* data2 = img.ptr<Vec3b>(y + height);
		for (int j = 0; j < width; j+=2)
			if (data1[x + j][1] > boundSaturationValve || data1[x + j][2] < boundValueValve ||
				data2[x + j][1] > boundSaturationValve || data2[x + j][2] < boundValueValve)
				return false;
		for (int i = 1; i < height; i += 2)
		{
			data1 = img.ptr<Vec3b>(y + i);
			if (data1[x][1] > boundSaturationValve || data1[x][2] < boundValueValve ||
				data1[xwidth][1] > boundSaturationValve || data1[xwidth][2] < boundValueValve)
				return false;
		}
		return true;}
	else return false;
}

bool DigitFinder::isDigit_RB(Rect boundRect, Mat& img)
{
	if (boundRect.height >= 60 && boundRect.width <= 200)
	{
		int x = boundRect.x;
		int height = boundRect.height;
		int y = boundRect.y;
		int width = boundRect.width;
		int xwidth = x + width;
		int yheight = y + height;
		Vec3b* data1 = img.ptr<Vec3b>(y);
		Vec3b* data2 = img.ptr<Vec3b>(y + height);
		for (int j = 0; j < width; j += 2)
		{
			//if (data1[x + j][1] > 80) return false;
			if (data1[x + j][2] > 180) return false;
			//if (data2[x + j][1] > 80) return false;
			if (data2[x + j][2] > 180) return false;
		}
		for (int i = 1; i < height; i += 2)
		{
			data1 = img.ptr<Vec3b>(y + i);
			//if (data1[x][1] > 80) return false; 
			if (data1[x][2] > 180) return false;
			//if (data1[xwidth][1] > 80) return false; 
			if (data1[xwidth][2] > 180) return false;
		}
		return true; }
	else return false;
}

bool DigitFinder::b_PrePredict(Mat &img)
{
	int digitCounter = 0;
	vector<vector <Point> > img_Contours;
	Rect allRect;
	//vector<Vec4i> hierarchy;
	int digitCand[predictNum][2];
	vector<Rect> boundRect;

	Mat img_ = img.clone();
	if (alter == BlacknWhite)
	{
		cvtColor(img_, img_, CV_BGR2GRAY);
		threshold(img_, img_, threshValve_BW, 255, THRESH_BINARY_INV);
		GaussianBlur(img_, img_, Size(5, 5), 1, 1, 4);
	}
	else if (alter == RednBlack)
	{
		Mat img_2;
		Mat dark = Mat::zeros(img_.size(), CV_16SC3);
		addWeighted(img_, 0.4, dark, 0.65, 0, img_, img_.depth());
		cvtColor(img_, img_, CV_BGR2HSV);
		inRange(img_, Scalar(164, 68, 81), Scalar(180, 255, 255), img_2);
		inRange(img_, Scalar(0, 68, 81), Scalar(10, 255, 255), img_);
		//threshold(img_, img_, threshValve_RB, 255, THRESH_BINARY);
		addWeighted(img_2, 1, img_, 1, 0, img_);
		img_2.release();
//		cvtColor(img_, img_, CV_HSV2BGR);
	}
	imshow("img_", img_);
	findContours(img_, img_Contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	img_.release();
	cvtColor(img, img, CV_BGR2HSV);
	if (alter == BlacknWhite)
		for (int i = 0; i<int(img_Contours.size()); i++)
			if (isDigit_BW(boundingRect(Mat(img_Contours[i])), img))
			{
				boundRect.push_back(boundingRect(Mat(img_Contours[i])));
				digitCand[digitCounter][0] = boundRect[digitCounter].x;
				digitCand[digitCounter][1] = i;
				digitCounter++;
			} else;
	else if (alter == RednBlack)
		for (int i = 0; i<int(img_Contours.size()); i++)
		{
			allRect = boundingRect(Mat(img_Contours[i]));
			rectangle(img, Rect(allRect.x, allRect.y, allRect.width, allRect.height), Scalar(0, 0, 0), 1);
			if (isDigit_RB(boundingRect(Mat(img_Contours[i])), img))
			{
				Mat digitROI2;
				Mat digitROI = img(Rect(allRect.x, allRect.y, allRect.width, allRect.height));
				inRange(digitROI, Scalar(162, 64, 81), Scalar(180, 255, 255), digitROI2);
				inRange(digitROI, Scalar(0, 64, 81), Scalar(10, 255, 255), digitROI);
				//threshold(img_, img_, threshValve_RB, 255, THRESH_BINARY);
				addWeighted(digitROI2, 1, digitROI, 1, 0, digitROI);
				digitROI2.release();
				imshow("digit", digitROI); waitKey(100);
				boundRect.push_back(boundingRect(Mat(img_Contours[i])));
				digitCand[digitCounter][0] = boundRect[digitCounter].x;
				digitCand[digitCounter][1] = i;
				digitCounter++;
			} else;
		}
	cvtColor(img, img, CV_HSV2BGR);
	if (digitCounter == 0) return false;

	int loc = 0;
	for (int j = 0; j<digitCounter; j++)
	{
		int tmp = digitCand[0][0];
		if (tmp>digitCand[j][0])
			{ tmp = digitCand[j][0]; loc = j; }
	}
	int loc1 = 0;
	for (int j = 0; j<digitCounter; j++)
	{
		int tmp = digitCand[0][0];
		if ( tmp>digitCand[j][0] && j!=loc )
		{
			tmp = digitCand[j][0]; loc1 = j;
		}
	}
	if (boundRect[loc].height > 1.22*boundRect[loc1].height) loc = loc1;
////	if (pointPolygonTest(img_Contours[digitCand[loc][1]], 
////		Point(boundRect[loc1].x + boundRect[loc1].width / 2, boundRect[loc1].y + boundRect[loc1].height / 2),false))
////		loc = loc1;
	//	int level = hierarchy[digitCand[loc1][1]][1];
//	int root = hierarchy[digitCand[loc][1]][2];
//	while (level != -1) 
//	{
//		if (root == level)
//			{ loc = loc1; break; }
//		else level = hierarchy[level][1];
//	}
	digitX = boundRect[loc].x;
	digitY = boundRect[loc].y;
	digitWidth = boundRect[loc].width;
	digitHeight = boundRect[loc].height;

	Mat imgROI = img(Rect(boundRect[loc].x, boundRect[loc].y, boundRect[loc].width, boundRect[loc].height));
	cvtColor(imgROI, imgROI, CV_BGR2GRAY);
	if (alter == BlacknWhite)
		threshold(imgROI, imgROI, threshValve_BW, 255, THRESH_BINARY_INV);
	else if (alter == RednBlack)
		threshold(imgROI, imgROI, threshValve_RB, 255, THRESH_BINARY);
	dilate(imgROI, imgROI, Mat());
	if (imgROI.rows>imgROI.cols)
		copyMakeBorder(imgROI, imgROI, 0, 0, 0, (imgROI.rows - imgROI.cols), 0, Scalar(0, 0, 0));
	resize(imgROI, imgROI, Size(digitSize, digitSize), 0, 0);
	//imshow("testtest", imgROI);
	for (int k = 0; k<digitSize; k++)
	{
		Mat temp_row = imgROI(Rect(0, k, digitSize, 1));
		temp_row.copyTo(predictData(Rect(k*digitSize, 0, digitSize, 1)));
	}
	rectangle(img, boundRect[loc].tl(), boundRect[loc].br(), Scalar(0, 205, 105), 1, 8, 0);
	return true;
}

int DigitFinder::i_KnnPredict()
{
	knn->findNearest(predictData, 12, predictResponce);
	return predictResponce.at<float>(0);
}

