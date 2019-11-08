#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "digit_finder.h"

using namespace std;
using namespace cv;
using namespace ml;

DigitFinder::DigitFinder(int x, int angle)
{
	alter = x;
	angOffset = angle;
	digitX = 0; digitY = 0; digitWidth = 0; digitHeight = 0;
	predictData = Mat::zeros(1, digitSize*digitSize, CV_32FC1);
	predictResponce = Mat(1, 1, CV_32FC1);
	knn = Algorithm::load<KNearest>("model.yml");
}

Point2i DigitFinder::p2i_getDigitLoc()
{
	return Point2i((digitX + digitWidth/2-320)/640, (digitY + digitHeight/2-240)/480);
}

bool DigitFinder::isDigit(Rect boundRect, Mat& img)
{
	if (boundRect.height >= 60 && boundRect.width <= 260)
	{
		int x = boundRect.x; int height = boundRect.height;
		int y = boundRect.y; int width = boundRect.width;
		int xwidth = x + width;
		int yheight = y + height;
		Vec3b* data1 = img.ptr<Vec3b>(y);
		Vec3b* data2 = img.ptr<Vec3b>(y + height);
		if (alter == BlacknWhite)
		{
			for (int j = 0; j < width; j += 2)
				if (data1[x + j][1] > boundSaturationValve_BW || data1[x + j][2] < boundValueValve_BW ||
					data2[x + j][1] > boundSaturationValve_BW || data2[x + j][2] < boundValueValve_BW)
					return false;
			for (int i = 1; i < height; i += 2)
			{
				data1 = img.ptr<Vec3b>(y + i);
				if (data1[x][1] > boundSaturationValve_BW || data1[x][2] < boundValueValve_BW ||
					data1[xwidth][1] > boundSaturationValve_BW || data1[xwidth][2] < boundValueValve_BW)
					return false;
			}
		}
		else if (alter == RednBlack)
		{
			for (int j = 0; j < width; j += 2)
				if (data1[x + j][1] > boundSaturationValve_RB || data1[x + j][2] < boundValueValve_RB ||
					data2[x + j][1] > boundSaturationValve_RB || data2[x + j][2] < boundValueValve_RB)
					return false;
			for (int i = 1; i < height; i += 2)
			{
				data1 = img.ptr<Vec3b>(y + i);
				if (data1[x][1] > boundSaturationValve_RB || data1[x][2] < boundValueValve_RB ||
					data1[xwidth][1] > boundSaturationValve_RB || data1[xwidth][2] < boundValueValve_RB)
					return false;
			}
		}
		return true;}
	else return false;
}

bool DigitFinder::b_PrePredict(Mat &img)
{
	int digitCounter = 0;
	vector<vector <Point> > img_Contours;
	int digitCand[predictNum][2];  //���ڴ�����о���ɸѡ��������ͼ���е�x������
	vector<Rect> boundRect;

	Mat img_ = img.clone();  //����clone���µ������У�����ԭͼ��ᱻǰ��������
	cvtColor(img_, img_, COLOR_BGR2GRAY);
	if (alter == BlacknWhite)
		threshold(img_, img_, threshValve_BW, 255, THRESH_BINARY_INV);
	else if (alter == RednBlack)
		threshold(img_, img_, threshValve_RB, 255, THRESH_BINARY_INV);
	GaussianBlur(img_, img_, Size(5, 5), 1, 1, 4);
	findContours(img_, img_Contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	img_.release();
	cvtColor(img, img, COLOR_BGR2HSV);
	for (int i = 0; i<int(img_Contours.size()); i++)
		if (isDigit(boundingRect(Mat(img_Contours[i])), img))
		{
			boundRect.push_back(boundingRect(Mat(img_Contours[i])));
			digitCand[digitCounter][0] = boundRect[digitCounter].x;
			digitCand[digitCounter][1] = i;
			digitCounter++;
		}
	cvtColor(img, img, COLOR_HSV2BGR);
	if (digitCounter == 0)  //���û�����֣�����
		return false;

	//����ж�����֣���ѡȡ���ģ����ҵ��Ǹ�����
	int loc = 0;
	for (int j = 0; j<digitCounter; j++) {
		int tmp = digitCand[0][0];
		if (tmp>digitCand[j][0]) {
			tmp = digitCand[j][0];
			loc = j;
		}
	}
	int loc1 = 0;
	for (int j = 0; j<digitCounter; j++) {
		int tmp = digitCand[0][0];
		if ( tmp>digitCand[j][0] && j!=loc ) {
			tmp = digitCand[j][0];
			loc1 = j;
		}
	}
	if (boundRect[loc].height > 1.22*boundRect[loc1].height) 
		loc = loc1;

	digitX = boundRect[loc].x;
	digitY = boundRect[loc].y;
	digitWidth = boundRect[loc].width;
	digitHeight = boundRect[loc].height;

	Mat imgROI = img(Rect(boundRect[loc].x, boundRect[loc].y, boundRect[loc].width, boundRect[loc].height));
	cvtColor(imgROI, imgROI, COLOR_BGR2GRAY);
	if (alter == BlacknWhite) {
		threshold(imgROI, imgROI, threshValve_BW, 255, THRESH_BINARY_INV);
		rotateROI(imgROI, angOffset);
		imshow("rotatedImg", imgROI);
	}
	else if (alter == RednBlack)
		threshold(imgROI, imgROI, threshValve_RB, 255, THRESH_BINARY_INV);

	dilate(imgROI, imgROI, Mat());
	if (imgROI.rows>imgROI.cols)
		copyMakeBorder(imgROI, imgROI, 0, 0, 0, (imgROI.rows - imgROI.cols), 0, Scalar(0, 0, 0));
	resize(imgROI, imgROI, Size(digitSize, digitSize), 0, 0);
	imshow("testtest", imgROI);
	for (int k = 0; k<digitSize; k++) {
		Mat temp_row = imgROI(Rect(0, k, digitSize, 1));
		temp_row.copyTo(predictData(Rect(k*digitSize, 0, digitSize, 1)));
	}

	rectangle(img, boundRect[loc].tl(), boundRect[loc].br(), Scalar(0, 205, 105), 1, 8, 0);
	return true;
}

void DigitFinder::rotateROI(Mat& imgROI, int angle)
{
	Point2f center(imgROI.cols / 2, imgROI.rows / 2);
	copyMakeBorder(imgROI, imgROI, 5, 5, 5, 5, 0, Scalar(0, 0, 0));
	Mat imgROI_rotated = getRotationMatrix2D(center, angle, 1);
	warpAffine(imgROI, imgROI, imgROI_rotated, imgROI.size());
}

int DigitFinder::i_KnnPredict()
{
	knn->findNearest(predictData, 12, predictResponce);
	return predictResponce.at<float>(0);
}
