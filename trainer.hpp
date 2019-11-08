#ifndef DIGITTRAINER_CPP_
#define DIGITTRAINER_CPP_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>

namespace dt{

using namespace std;
using namespace cv;
using namespace ml;

class digitTrainer
{
private:
	static const int width = 12;
	static const int height = 12;
	static const int originwidth = 60;
	static const int originheight = 48;
	static const int num_class = 10;
	static const int boundColorValve = 110;
	static const int threshValve = 110;
	Ptr<ml::KNearest> knn;
	Mat trainData;
	Mat trainResponces;

public:
	digitTrainer();
	void getROI(Mat& imgROI);
	void singleTrain(Mat imgROI, int i);
	bool isNewTrainer();
	int iniTrainer();
	void iniTrain_processing(Mat img, Mat &trainData, Mat &trainResponces, int serialNum);
	int folTrainer();
	void knnTrain(Ptr<ml::KNearest>& knn, Mat& trainData, Mat& trainResponces);
	void saveTrainer();
	bool isDigit(Rect boundRect,Mat img);
	Mat getDigit(Mat& img);
};

}

#endif
