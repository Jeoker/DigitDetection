#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "trainer.hpp"

using namespace std;
using namespace cv;
using namespace ml;
using namespace dt;

const int boundColorValve = 110;
const int threshValve = 110;

int main()
{
	digitTrainer Trainer;
	if (Trainer.isNewTrainer())
		Trainer.iniTrainer();
	else Trainer.folTrainer();
	Trainer.saveTrainer();
}
