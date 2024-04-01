#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;
class GrafImageProcessing
{
public:
	double getOrientation(const vector<Point>&, Mat&, PCA&);
};

