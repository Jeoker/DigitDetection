#include "opencv.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "digitTrainer.h"

using namespace std;
using namespace cv;
using namespace ml;

namespace dt{

digitTrainer::digitTrainer()
{
	knn = KNearest::create();
//	trainData = imread("trainData.png");
//	cvtColor(trainData, trainData, CV_BGR2GRAY);
//	trainData.convertTo(trainData, CV_32FC1);
//	trainResponces = imread("trainResponces.png");
//	cvtColor(trainResponces, trainResponces, CV_BGR2GRAY);
//	trainResponces.convertTo(trainResponces, CV_32FC1);
}

void digitTrainer::getROI(Mat& imgROI)
{
	int left, right, top, bottom;
	left = imgROI.cols;
	right = 0;
	top = imgROI.rows;
	bottom = 0;
	
	for(int i=0; i<imgROI.rows; i++)
		for(int j=0; j<imgROI.cols; j++)
			if(imgROI.at<uchar>(i, j) > 20){
				if(j<left) left = j;  
				if(j>right) right = j;
				if(i<top) top = i;
				if(i>bottom) bottom = i;}

	int width = right - left;
	int height = bottom - top;
	imgROI = imgROI(Rect(left,top,width+1,height+1));
	if (width<height)
		copyMakeBorder(imgROI,imgROI,0,0,0,(height-width),0,Scalar(0,0,0));
	else copyMakeBorder(imgROI,imgROI,0,(width-height),0,0,0,Scalar(0,0,0));
	//imshow("fsample",imgROI);
	//waitKey(0);
}

void digitTrainer::singleTrain(Mat imgROI, int i)
{
	resize(imgROI,imgROI,Size(width,height),0,0);
	Mat newSample = Mat(1,width*height,CV_32F);
	Mat num = Mat(1,1,CV_32F);
	num.at<float>(Point(0,0)) = i;
	for(int k=0; k<height ;k++)
		imgROI(Rect(0,k,width,1)).copyTo(newSample(Rect(k*width,0,width,1)));
	trainData.push_back(newSample);
	trainResponces.push_back(num);
	//imshow("sample",imgROI);
}

bool digitTrainer::isNewTrainer()
{
	bool isempty = trainData.empty();
	return isempty;
}

int digitTrainer::iniTrainer()
{
	trainData = Mat::zeros(0,width*height,CV_32FC1);
	trainResponces = Mat::zeros(0,1,CV_32FC1);
	string angle[9] = {"-16","-12","-8","-4","0","4","8","12","16"};
	string serial = "123";
	
	for (int i=0;i<3;i++)
		for (int j=0;j<9;j++)
		{
			string filename = "Samples/";
			filename = filename + serial[i];
			filename = filename + "printedSamples";
			filename = filename + angle[j];
			filename = filename + ".jpg";
			Mat imgTrain = imread(filename,CV_32FC1);
			if (imgTrain.empty())
				{cout<<filename<<"	"<<"error"<<endl;
				return 0;}
			cout<<filename<<"	";
			threshold(imgTrain,imgTrain,75,255,CV_THRESH_BINARY);
			iniTrain_processing(imgTrain,trainData,trainResponces,i);
		}
	return 1;
}

void digitTrainer::iniTrain_processing(Mat img, Mat &trainData, Mat &trainResponces, int serialNum)
{
	int num_samples = 17;
	int digitCounter=0;
	int j = 0;
	cvtColor(img,img,CV_BGR2GRAY);

	if (serialNum==2) num_samples=16;
	while ( j < num_samples ){
		for ( int i=0; i<num_class; i++ ){
			Mat imgROI = img(Rect(i*originwidth,j*originheight,originwidth,originheight));
			//rectangle(imgROI,Point(0,0),Point(originwidth,originheight),Scalar(255,255,255),1);
			getROI(imgROI);
			singleTrain(imgROI,i);
			digitCounter++;
		}j++;}
	cout << digitCounter << endl;
	//imshow("test1",img);
	//waitKey(0);
}

int digitTrainer::folTrainer()
{
	Mat addSample;
	int i;
	char command;
	//if (addSample.empty())
	//{
	cout<<"使用镜头抓取新数字,按下c键抓取"<<endl;
	while (1){
		VideoCapture cap(0);
		while (1)
		{
			Mat src, srcROI, digit;
			cap >> src;
			rectangle(src, Point(138, 98), Point(482, 392), Scalar(0, 0, 255), 1.5, 8, 0);
			srcROI = src(Rect(140, 100, 340, 290));
			digit = getDigit(srcROI);
			imshow("test", src);
			if (waitKey(10) == 'c')
			{ 
				imwrite("addSample.jpg", digit);
				cout << "数字抓取成功" << endl;
				addSample = imread("addSample.jpg", CV_32FC1);
				break;
			}
			else if (waitKey(10) == 27)
			{
				cout<<"取消抓取数字"<<endl;
				addSample = imread("addSample.jpg",CV_32FC1);
				break;
			}
		}
		if (addSample.empty())
			{ cout << "未抓取数字,退出程序" << endl; return 0; }
		GaussianBlur(addSample, addSample, Size(3, 3), 3);
		cvtColor(addSample, addSample, CV_BGR2GRAY);
		threshold(addSample, addSample, 95, 255, THRESH_BINARY_INV);
		getROI(addSample);
		cout << "请输入实际数字" << endl;
		cin >> i;
		singleTrain(addSample, i);
		cout << "新样本已添加，是否继续？ Y?N" << endl;
		cin >> command;
		if (command == 'y' || command == 'Y');
		else return 0;
	}
}

void digitTrainer::knnTrain(Ptr<ml::KNearest>& knn,Mat& trainData,Mat& trainResponces)
{
	knn->train(trainData, ml::ROW_SAMPLE, trainResponces);
	knn->save("(NewTables)my.xml");
}

void digitTrainer::saveTrainer()
{
		knnTrain(knn,trainData,trainResponces);
//		imwrite("trainData.png",trainData);
//		imwrite("trainResponces.png",trainResponces);
//		imshow("test",trainData);
		waitKey(0);
}

bool digitTrainer::isDigit(Rect boundRect,Mat img)
{
	if (boundRect.height>=75 && boundRect.width<=200)
	{
		for (int i=0; i<boundRect.height/3; i++)
			if (img.at<Vec3b>(boundRect.y+i*3,boundRect.x)[0]<boundColorValve ||
				img.at<Vec3b>(boundRect.y+i*3,boundRect.x+boundRect.width)[1]<boundColorValve)
				return false;
		for (int j=0; j<boundRect.width/3; j++)
			if (img.at<Vec3b>(boundRect.y,boundRect.x+3*j)[0]<boundColorValve ||
				img.at<Vec3b>(boundRect.y+boundRect.height,boundRect.x+3*j)[1]<boundColorValve)
				return false;
		return true;
	}
	else return false;
}

Mat digitTrainer::getDigit(Mat& img)
{
	vector<vector <Point> > img_Contours;
	Rect boundRect;
	Mat img_ = img.clone();
	Mat digit;
	cvtColor(img_,img_,CV_BGR2GRAY);
	threshold(img_,img_,threshValve,255,THRESH_BINARY_INV);
	GaussianBlur(img_,img_,Size(5,5),1,1,4);
	findContours(img_,img_Contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

	for (int i=0; i<int(img_Contours.size()); i++)
		if (isDigit(boundingRect(Mat(img_Contours[i])),img))
		{
			boundRect = boundingRect(Mat(img_Contours[i]));
			digit = img(Rect(boundRect.x,boundRect.y,boundRect.width,boundRect.height)).clone();
			rectangle(img,Point(boundRect.x-1,boundRect.y-1),Point(boundRect.x+1+boundRect.width,boundRect.y+1+boundRect.height),Scalar(0,0,255),1);
		}
	return digit;
}

}
