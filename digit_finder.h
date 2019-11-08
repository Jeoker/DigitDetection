#ifndef DIGIT_FINDER_H_
#define DIGIT_FINDER_H_

using namespace std;
using namespace cv;
using namespace ml;

class DigitFinder
{
private:
	static const int RednBlack = 0;  //���Ӻ�������
	static const int BlacknWhite = 1;//�׵׺����������
	static const int threshValve_BW = 110;  //�׵׺�����ɫ���˷�
	static const int threshValve_RB = 120;  //��������ɫ���˷�
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
	DigitFinder(int x, int angle);
	Point2i p2i_getDigitLoc();
	void rotateROI(Mat& imgROI, int angle);
	bool isDigit(Rect boundRect, Mat& img);
	bool b_PrePredict(Mat &img);
	int i_KnnPredict();
};

#endif
