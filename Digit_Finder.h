#ifndef DIGIT_FINDER_H_
#define DIGIT_FINDER_H_

using namespace std;
using namespace cv;
using namespace ml;

class DigitFinder
{
private:
	static const int RednBlack = 0;
	static const int BlacknWhite = 1;
	static const int threshValve_BW = 110;
	static const int threshValve_RB = 95;
	static const int boundSaturationValve = 45;
	static const int boundValueValve = 200;
	static const int digitSize = 12;
	static const int predictNum = 6;
	int digitX, digitY;
	int digitWidth, digitHeight;
	int alter;
	
	Mat predictData;
	Mat predictResponce;
	Ptr<ml::KNearest>  knn;

public:
	DigitFinder(int x);
	Point2i p2i_getDigitLoc();
	bool isDigit_BW(Rect boundRect, Mat& img);
	bool isDigit_RB(Rect boundRect, Mat& img);
	bool b_PrePredict(Mat &img);
	int i_KnnPredict();
};

#endif
