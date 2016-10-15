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

int main(int argc, char *argv[])
{
	alter = int(argv[2]);
	DigitFinder digitFinder(alter);
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
		rectangle(src, Point(119, 0), Point(501, 480), Scalar(0, 0, 255), 1.5, 8, 0);
		//srcROI = src(Rect(140, 100, 340, 290));
		srcROI = src(Rect(120, 1, 380, 478));
		if (digitFinder.b_PrePredict(srcROI))
		{
			predictResult = digitFinder.i_KnnPredict();
			if (predictResult == lastRecult)
			{
				if (!b_GetSequence(confirmedDigit, ++verifyCounter, predictResult, ds, digitSequence))
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
