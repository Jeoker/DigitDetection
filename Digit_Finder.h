#ifndef DIGIT_FINDER_H_
#define DIGIT_FINDER_H_

using namespace std;
using namespace cv;
using namespace ml;

class df::DigitFinder
{
private:
	static const int RednBlack = 0;  //µç×ÓºìºÚÆÁÐòºÅ
	static const int BlacknWhite = 1;//°×µ×ºÚ×ÖÊý×ÖÐòºÅ
	static const int threshValve_BW = 110;  //°×µ×ºÚ×ÖÑÕÉ«¹ýÂË·§
	static const int threshValve_RB = 120;  //µç×ÓÆÁÑÕÉ«¹ýÂË·§
	static const int boundSaturationValve_BW = 50;
	static const int boundValueValve_BW = 200;
	static const int boundSaturationValve_RB = 100;
	static const int boundValueValve_RB = 160;
	static const int digitSize = 12;
	static const int predictNum = 6;
	int digitX, digitY;
	int digitWidth, digitHeight;
	int alter, angOffset;
	
	Mat predictData;
	Mat predictResponce;
	Ptr<ml::KNearest>  knn;

public:
	df::DigitFinder(int x, int angle);
	Point2i df::p2i_getDigitLoc();
	void df::rotateROI(Mat& imgROI, int angle);
	bool df::isDigit(Rect boundRect, Mat& img);
	bool df::b_PrePredict(Mat &img);
	int df::i_KnnPredict();
};

#endif
