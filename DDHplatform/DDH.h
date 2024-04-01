#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/examples/label_image/get_top_n.h"
#include "tensorflow/lite/model.h"
#include "GrafImageProcessing.h"
using namespace cv;
using namespace tflite;
using TfLiteDelegatePtr = tflite::Interpreter::TfLiteDelegatePtr;
using TfLiteDelegatePtrMap = std::map<std::string, TfLiteDelegatePtr>;
class Preprocessing
{
	
public:
	Mat ResizeImage(Mat matImage);
	Mat CreateZeroPadding(cv::InputArray _Input);
	GrafImageProcessing GrafIP;
	Mat GrafAlgorithm(Mat matImage, Mat matOutputIlium, Mat matOutputLabrum, Mat matOutputBonyRim, Mat matOutputLowerLimb, PCA& pcaPoint, double& dAlphaAngle, double& dBetaAngle);
};

class TfLite
{
public:
	double StandardPlaneClassification(Mat matImage);
	void GrafSegmentation(Mat matImage, Mat& m_matOutputIlium, Mat& m_matOutputLabrum, Mat& m_matOutputBonyRim, Mat& m_matOutputLowerLimb);
	
	bool init();
private:
	bool initialized;
	StderrReporter error_reporter;
	int wanted_height, wanted_width, wanted_channels;
	std::vector<TfLiteTensor *> m_StdPlaneOutputs;
	std::unique_ptr<Interpreter> m_StdPlaneInterpreter;
	std::unique_ptr<FlatBufferModel> m_StdPlaneModel;
	ops::builtin::BuiltinOpResolver m_StdResolver;
	std::vector<float> nhwc2nchw(cv::Mat src,std::vector<float> mean,std::vector<float> std ,bool swapRB);
	std::vector<TfLiteTensor*> m_GrafSegOutputs;
	std::unique_ptr<Interpreter> m_GrafSegInterpreter;
	std::unique_ptr<FlatBufferModel> m_GrafSegModel;
	ops::builtin::BuiltinOpResolver m_GrafSegResolver;
};
