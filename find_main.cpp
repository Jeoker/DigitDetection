#include "opencv.hpp"
#include <iostream>
#include "Digit_Finder.h"

using namespace std;
using namespace cv;
int alter;

bool b_GetSequence(int& confirmedDigit, int& verifyCounter, int& predictResult, int& ds, int* digitSequence)
{
	int confirmCounter;
	if (alter == 0) confirmCounter = 3;
	else if (alter == 1) confirmCounter = 8;
	if (verifyCounter >= confirmCounter)
	{
		cout << predictResult << '\t';
		if (confirmedDigit == -1)
			{ confirmedDigit = predictResult; digitSequence[ds++] = confirmedDigit; }
		else if (confirmedDigit != predictResult)
		{
			if (ds == 10)
				{ cout << "10个数字已读取完毕，无法加入更多数字" << endl; return false; }
			else
				{ confirmedDigit = predictResult; digitSequence[ds++] = confirmedDigit; }
		}
	}
	return true;
}

int main()
{
	alter = 1;
	int angle = 0;
	DigitFinder digitFinder(alter, angle);
	VideoCapture cap(0);
	Mat src,srcROI;
	int verifyCounter = 0;
	int lastRecult = -1;
	int predictResult = 0;
	int confirmedDigit = -1;
	int digitSequence[10] = { 0 };
	int ds = 0;

	while (1)
	{
		cap >> src;
		//rectangle(src, Point(119, 9), Point(501, 471), Scalar(0, 0, 255), 1, 8, 0);
		//srcROI = src(Rect(140, 100, 340, 290));
		srcROI = src(Rect(120, 10, 380, 460));
		//如果识别黑底红字的电子屏，则先对图像进行反色处理，否则无法正确获得数字轮廓
		if (alter == 0)
		{
			for (int i = 0; i<460; i++)
			{
				Vec3b* data = srcROI.ptr<Vec3b>(i);
				for (int j = 0; j<380; j++)
				{
					data[j][0] = 255 - data[j][0];
					data[j][1] = 255 - data[j][1];
					data[j][2] = 255 - data[j][2];
				}
			}
		}
		if (digitFinder.b_PrePredict(srcROI))  //识别时，为提高识别准确性，三次识别出同样的结果才被接受
		{
			predictResult = digitFinder.i_KnnPredict();
			if (predictResult == lastRecult)
			{
				if (!b_GetSequence(confirmedDigit, ++verifyCounter, predictResult, ds, digitSequence))  //该函数中设定了识别数量上限，去年比赛为10个数字
					break;
			}
			else
				{ lastRecult = predictResult; verifyCounter = 0; }
		}
		else lastRecult = -1;
		imshow("test", src);
		if (waitKey(150) >= 0)
			break;
	}
	for (int i = 0; i < 10; i++)
		cout << digitSequence[i] << '\t';
	system("pause");
	return 0;
}
