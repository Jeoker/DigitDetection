#include "opencv.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "digitTrainer.h"

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
