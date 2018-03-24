#ifndef DIGITTRAINER_CPP_
#define DIGITTRAINER_CPP_

using namespace std;
using namespace cv;
using namespace ml;

class dt::digitTrainer
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
	dt:digitTrainer();
	void dt::getROI(Mat& imgROI);
	void dt::singleTrain(Mat imgROI, int i);
	bool dt::isNewTrainer();
	int dt::iniTrainer();
	void dt::iniTrain_processing(Mat img, Mat &trainData, Mat &trainResponces, int serialNum);
	int dt::folTrainer();
	void dt::knnTrain(Ptr<ml::KNearest>& knn, Mat& trainData, Mat& trainResponces);
	void dt::saveTrainer();
	bool dt::isDigit(Rect boundRect,Mat img);
	Mat dt::getDigit(Mat& img);
};

#endif
