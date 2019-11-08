#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

void getROI(Mat& imgROI)  
{
	int left, right, top, bottom;  
	left = imgROI.cols;  
	right = 0;  
	top = imgROI.rows;  
	bottom = 0;  
	
	for(int i=0; i<imgROI.rows; i++)
		for(int j=0; j<imgROI.cols; j++)
			if(imgROI.at<uchar>(i, j) <200)  
			{
				if(j<left) left = j;  
				if(j>right) right = j;
				if(i<top) top = i;
				if(i>bottom) bottom = i;
			}
	
	imgROI = imgROI(Rect(left,top,right - left,bottom - top));
}

void rotateROI(Mat& imgROI, int angle)
{
	Point2f center(imgROI.cols/2, imgROI.rows/2);
	Mat imgROI_rotated = getRotationMatrix2D(center, angle, 1);
	warpAffine(imgROI, imgROI, imgROI_rotated, imgROI.size());  
}

int main()
{
	string num_serial = "123";
	int angles[] = {-16,-12,-8,-4,0,4,8,12,16};
	//bitwise_xor(img,Scalar(255,255,255),img);
	//copyMakeBorder(img,img,1,0,0,0,0,Scalar(255,255,255));
	//cout<<img.rows<<"	"<<img.cols<<endl;
	for (int i=0;i<3;i++)
		for (int j=0;j<9;j++)
		{
			int lines = 17;
			string filename = "printedSamples0.jpg";
			char angle[3];
			filename = num_serial[i] + filename;
			Mat img = imread(filename,CV_32FC1);
			int l = 0;
			if (num_serial[i]=='3')
				lines = 16;
			while (l<lines)
			{
				for (int num=0; num<10; num++)
				{
					Mat imgROI = img(Rect(num*60,l*48,60,48));
					//Point imgROI_tl(i*60,j*48);
					//Point imgROI_br(i*60+60,j*48+48);
					//getROI(imgROI);
					rotateROI(imgROI, angles[j]);
					//rectangle(img,Point(i*60,j*48),Point(i*60+60,j*48+48),Scalar(255,0,0),2);
				}
				l++;
			}
			sprintf(angle, "%d", angles[j]);
			filename = filename.substr(0,filename.length()-5);
			filename = filename + string(angle);
			filename = filename + ".jpg";
			imwrite(filename,img);
			imshow("test",img);
		}
	//imwrite("3printedSamples16.jpg",img);
	//imshow("test1",imgClone);
	//imshow("origin",x2);
	//system("pause");
	waitKey(0);
}
