#include "PCH.h"
#include "DDH.h"

Mat Preprocessing::ResizeImage(Mat matSrcImage)
{
	int nSize = 224;
	Mat matDstImage;
	cv::resize(matSrcImage, matDstImage, Size(nSize, nSize), 0, 0, cv::INTER_NEAREST);
	return matDstImage;
}

Mat Preprocessing::CreateZeroPadding(cv::InputArray _Input)
{
	cv::Mat input = _Input.getMat();
	int channel = input.channels() * 8;
	if (channel == 8)
	{
		cv::cvtColor(input, input, cv::COLOR_GRAY2BGR);
	}
	cv::Mat kernel = cv::Mat_<float>({ 3, 3 }, { 0,1,0,0,1,0,0,1,0 });
	cv::Mat _Output = input.clone();
	const cv::Size& k = kernel.size();
	const cv::Size& stride = cv::Size(1, 1);
	int nSize;
	if (input.rows > input.cols)
	{
		nSize = input.rows;
	}
	else
	{
		nSize = input.cols;
	}
	//패딩 맞추기 자동화
	double p = 0;
	int oH = (int)((input.rows + 2 * p - k.height) / stride.height) + 1;
	int oW = (int)((input.cols + 2 * p - k.width) / stride.width) + 1;
	//제로 패딩 행렬 생성
	//패딩이 0.5 늘어날 때마다 왼쪽 + 위, 오른쪽 + 아래 순으로 행렬 확장
	while (oH != nSize) {
		p += 0.5;
		if (p - (int)p != 0)
			cv::copyMakeBorder(_Output, _Output, 1, 0, 0, 0, cv::BORDER_CONSTANT, 0);
		else
			cv::copyMakeBorder(_Output, _Output, 0, 1, 0, 0, cv::BORDER_CONSTANT, 0);

		oH = (int)((input.rows + 2 * p - k.height) / stride.height) + 1;
	}
	p = 0;
	while (oW != nSize) {
		p += 0.5;
		if (p - (int)p != 0)
			cv::copyMakeBorder(_Output, _Output, 0, 0, 1, 0, cv::BORDER_CONSTANT, 0);
		else
			cv::copyMakeBorder(_Output, _Output, 0, 0, 0, 1, cv::BORDER_CONSTANT, 0);

		oW = (int)((input.cols + 2 * p - k.width) / stride.width) + 1;
	}
	return _Output;
}

std::vector<float> TfLite::nhwc2nchw(
	cv::Mat src,
	std::vector<float> mean = { 0.f, 0.f, 0.f },
	std::vector<float> std = { 1.f, 1.f, 1.f },
	bool swapRB = false)
{
	assert(src.type() == CV_32FC3 && "Input cv::Mat type should be CV_32FC3!!");

	const float* img_data = reinterpret_cast<float*>(src.data);
	const size_t nPixels = src.total();
	const size_t nCh = src.channels();

	std::vector<float> flatten(nPixels * nCh);

	int nPos = 0;
	for (int row = 0; row < src.rows; row++)
	{
		for (int col = 0; col < src.cols; col += 1)
		{
			int nIdx = 0;
			nIdx = row * src.cols * 3 + col * 3;

			std::cout << img_data[nIdx] << std::endl;
			std::cout << img_data[nIdx + 1] << std::endl;
			std::cout << img_data[nIdx + 2] << std::endl;

			flatten[nPos] = (img_data[nIdx] - mean[0]) / std[0];                     // Blue
			flatten[nPos + nPixels] = (img_data[nIdx + 1] - mean[1]) / std[1];       // Grean
			flatten[nPos + (nPixels * 2)] = (img_data[nIdx + 2] - mean[2]) / std[2]; // Red

			if (swapRB)
			{
				std::swap(flatten[nPos], flatten[nPos + (nPixels * 2)]);
			}

			nPos++;
		}
	}

	return flatten;
}

bool TfLite::init()
{
	//모델 불러오기
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);
	CString strPath = path;
	int i = strPath.ReverseFind('\\');
	strPath = strPath.Left(i);
	CStringA strTemp = (CStringA)strPath;
	const char* pwd = strTemp.GetBuffer();
	std::string modelname = pwd + std::string("\\model\\MobileNetV2.tflite");
	m_StdPlaneModel = tflite::FlatBufferModel::BuildFromFile(modelname.c_str(), &error_reporter);

	tflite::InterpreterBuilder(*m_StdPlaneModel.get(), m_StdResolver)(&m_StdPlaneInterpreter);

	if (m_StdPlaneInterpreter->AllocateTensors() != kTfLiteOk)
	{
		return false;
	}
	m_StdPlaneInterpreter->SetAllowFp16PrecisionForFp32(true);
	m_StdPlaneInterpreter->SetNumThreads(1);
	m_StdPlaneOutputs.clear();

	modelname = pwd + std::string("\\model\\DeepLabV3Plus.tflite");
	m_GrafSegModel = tflite::FlatBufferModel::BuildFromFile(modelname.c_str(), &error_reporter);

	tflite::InterpreterBuilder(*m_GrafSegModel.get(), m_GrafSegResolver)(&m_GrafSegInterpreter);

	if (m_GrafSegInterpreter->AllocateTensors() != kTfLiteOk)
	{
		return false;
	}
	m_GrafSegInterpreter->SetAllowFp16PrecisionForFp32(true);
	m_GrafSegInterpreter->SetNumThreads(1);
	m_GrafSegOutputs.clear();
	int output = 0;

	initialized = true;
	return initialized;
}
void drawDottedLine(Mat& img, Point start, Point end, Scalar color, int thickness = 1, int dotSpace = 5) {
	double dx = end.x - start.x;
	double dy = end.y - start.y;
	double length = sqrt(dx * dx + dy * dy);

	if (length == 0)
		return;

	dx /= length;
	dy /= length;

	for (double i = 0; i < length; i += dotSpace * 2) {
		Point p1, p2;
		p1.x = cvRound(start.x + i * dx);
		p1.y = cvRound(start.y + i * dy);
		p2.x = cvRound(start.x + (i + dotSpace) * dx);
		p2.y = cvRound(start.y + (i + dotSpace) * dy);
		line(img, p1, p2, color, thickness);
	}
}
void lineDraw(Point2i pt1, Point2i pt2, Mat& matImage, Scalar color) {
	double slope;
	if (pt2.x != pt1.x) {
		slope = (double)(pt2.y - pt1.y) / (pt2.x - pt1.x);
	}
	else {
		slope = std::numeric_limits<double>::infinity(); // 무한대로 설정
	}

	int imgHeight = matImage.rows;
	int imgWidth = matImage.cols;
	Point startPoint, endPoint;

	// 기울기가 무한대인 경우
	if (std::isinf(slope)) {
		startPoint.x = pt1.x;
		startPoint.y = 0;
		endPoint.x = pt1.x;
		endPoint.y = imgHeight - 1;
	}
	else {
		double intercept = pt1.y - slope * pt1.x;
		startPoint.x = 0;
		startPoint.y = cvRound(slope * startPoint.x + intercept);
		endPoint.x = imgWidth - 1;
		endPoint.y = cvRound(slope * endPoint.x + intercept);
	}

	drawDottedLine(matImage, startPoint, endPoint, color, 1, 5);
}
void drawDottedLineFromEdgeToEdge(Mat& img, PCA& pcaPoint, Scalar color, int thickness = 1, int dotSpace = 5) {
	// Get image dimensions
	int width = img.cols;
	int height = img.rows;

	// Get the center point of PCA
	Point cntr = Point(static_cast<int>(pcaPoint.mean.at<double>(0, 0)),
		static_cast<int>(pcaPoint.mean.at<double>(0, 1)));

	// Get the direction of PCA
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i) {
		eigen_vecs[i] = Point2d(pcaPoint.eigenvectors.at<double>(i, 0),
			pcaPoint.eigenvectors.at<double>(i, 1));
	}

	// Calculate the end points of the line
	Point end1 = cntr + 0.5 * Point(static_cast<int>(eigen_vecs[0].x * width),
		static_cast<int>(eigen_vecs[0].y * height));
	Point end2 = cntr - 0.5 * Point(static_cast<int>(eigen_vecs[0].x * width),
		static_cast<int>(eigen_vecs[0].y * height));

	// Draw dotted lines
	drawDottedLine(img, end1, end2, color, thickness, dotSpace);
}

Point2i orthogonalPoint(Mat& img, PCA& pcaPoint) {

	Point2i dst2iPoint;
	int step = 1;
	Point cntr = Point(static_cast<int>(pcaPoint.mean.at<double>(0, 0)),
		static_cast<int>(pcaPoint.mean.at<double>(0, 1)));
	vector<Point2d> eigen_vecs(2);
	for (int i = 0; i < 2; ++i) {
		eigen_vecs[i] = Point2d(pcaPoint.eigenvectors.at<double>(i, 0),
			pcaPoint.eigenvectors.at<double>(i, 1));
	}
	int temp_x = cntr.x + static_cast<int>(eigen_vecs[0].x * step);
	int temp_y = cntr.y + static_cast<int>(eigen_vecs[0].y * step);
	int new_x, new_y;
	int ColStep = 1;
	while (1)
	{
		temp_x = cntr.x + static_cast<int>(eigen_vecs[0].x * 1);
		temp_y = cntr.y + static_cast<int>(eigen_vecs[0].y * 1) + ColStep;
		int RowStep = 1;
		while (1) {
			new_x = cntr.x + static_cast<int>(eigen_vecs[0].x * RowStep);
			new_y = cntr.y + static_cast<int>(eigen_vecs[0].y * RowStep) + ColStep;

			// 이미지 경계를 벗어나면 종료
			if (new_x < 0 || new_x >= img.cols || new_y < 0 || new_y >= img.rows)
				break;
			// 현재 픽셀 값이 255이고, 다음 픽셀 값이 0인 경우
			if (img.at<uchar>(temp_y, temp_x) == 255)
			{
				if (img.at<uchar>(new_y, new_x) == 0) {
					dst2iPoint = Point(new_x, new_y);
					break;
				}
			}
			temp_x = new_x;
			temp_y = new_y;
			++RowStep;
		}
		if (new_y < 0 || new_y >= img.rows)
			break;
		++ColStep;
	}
	return dst2iPoint;
}
double TfLite::StandardPlaneClassification(Mat matImage)
{
	cv::Mat tempImage;
	matImage.convertTo(tempImage, CV_32FC3);
	float* input = m_StdPlaneInterpreter->typed_input_tensor<float>(0);
	memcpy(input, tempImage.data, sizeof(float) * 224 * 224 * 3);

	m_StdPlaneInterpreter->Invoke();
	auto output = m_StdPlaneInterpreter->typed_output_tensor<float>(0);
	double dPredictProb = *output;
	return dPredictProb;
}

void TfLite::GrafSegmentation(Mat matImage, Mat& pMatOutputIlium,Mat& pMatOutputLabrum,Mat& pMatOutputBonyRim,cv::Mat& pMatOutputLowerLimb)
{
	matImage.convertTo(matImage, CV_32FC3, 1.f / 255.0);
	std::vector<float> vec_img_my_blob = nhwc2nchw(matImage);
	cv::Mat tempImage(1, vec_img_my_blob.size(), CV_32FC1, vec_img_my_blob.data());
	float* input = m_GrafSegInterpreter->typed_input_tensor<float>(0);
	memcpy(input, tempImage.data, sizeof(float) * 224 * 224 * 3);
	m_GrafSegInterpreter->Invoke();
	int output_tensor_count = m_GrafSegInterpreter->outputs().size();
	auto output_tensor = m_GrafSegInterpreter->outputs();
	for (int i = 0; i < output_tensor_count; ++i) {
		int output_tensor_index = m_GrafSegInterpreter->outputs()[i];
		TfLiteIntArray* output_dims = m_GrafSegInterpreter->tensor(output_tensor_index)->dims;
	}

	int output_tensor_index = 0;
	// output image with size of [4x224x224]
	const int output_image_width = 224;
	const int output_image_height = 224;

	// Pointer to the output sensor data
	float* output_data = m_GrafSegInterpreter->typed_output_tensor<float>(output_tensor_index);

	cv::Mat matOutputLabrum(output_image_height, output_image_width, CV_32FC1);
	cv::Mat matOutputIlium(output_image_height, output_image_width, CV_32FC1);
	cv::Mat matOutputBonyRim(output_image_height, output_image_width, CV_32FC1);
	cv::Mat matOutputLowerLimb(output_image_height, output_image_width, CV_32FC1);

	for (int y = 0; y < output_image_height; ++y) {
		for (int x = 0; x < output_image_width; ++x) {
			matOutputLabrum.at<float>(y, x) = static_cast<float>(output_data[y * output_image_width + x]);
			matOutputIlium.at<float>(y, x) = static_cast<float>(output_data[output_image_width * output_image_height * 1 + y * output_image_width + x]);
			matOutputBonyRim.at<float>(y, x) = static_cast<float>(output_data[output_image_width * output_image_height * 2 + y * output_image_width + x]);
			matOutputLowerLimb.at<float>(y, x) = static_cast<float>(output_data[output_image_width * output_image_height * 3 + y * output_image_width + x]);
		}
	}
	pMatOutputLabrum = matOutputLabrum.clone();
	pMatOutputIlium = matOutputIlium.clone();
	pMatOutputBonyRim = matOutputBonyRim.clone();
	pMatOutputLowerLimb = matOutputLowerLimb.clone();
}



Mat Preprocessing::GrafAlgorithm(Mat matImage, Mat matOutputIlium, Mat matOutputLabrum, Mat matOutputBonyRim, Mat matOutputLowerLimb, PCA& pcaPoint, double& dAlphaAngle, double& dBetaAngle)
{
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
	Mat matDstImage = matImage.clone();
	findContours(matOutputIlium, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);

	// Calculate the area of each contour
	double area = contourArea(contours[0]);

	double dBaselineAngle =GrafIP.getOrientation(contours[0], matDstImage, pcaPoint);

	// Draw dotted lines
	drawDottedLineFromEdgeToEdge(matDstImage, pcaPoint, Scalar(0, 255, 255), 1, 5);

	Point cntr = Point(static_cast<int>(pcaPoint.mean.at<double>(0, 0)),
		static_cast<int>(pcaPoint.mean.at<double>(0, 1)));
	vector<Point2d> eigen_vecs(2);
	for (int i = 0; i < 2; ++i) {
		eigen_vecs[i] = Point2d(pcaPoint.eigenvectors.at<double>(i, 0),
			pcaPoint.eigenvectors.at<double>(i, 1));
	}

	//ilium point
	Point2i iliumPoint;
	int step = 1;
	int temp_x= cntr.x + static_cast<int>(eigen_vecs[0].x * step);
	int temp_y= cntr.y + static_cast<int>(eigen_vecs[0].y * step);
	int new_x, new_y;
	while (1) {
		new_x = cntr.x + static_cast<int>(eigen_vecs[0].x * step);
		new_y = cntr.y + static_cast<int>(eigen_vecs[0].y * step);

		// 이미지 경계를 벗어나면 종료
		if (new_x < 0 || new_x >= matOutputIlium.cols || new_y < 0 || new_y >= matOutputIlium.rows)
		{ 
			circle(matDstImage, iliumPoint, 3, Scalar(0, 0, 255), 1, LINE_AA);
			break;
		}
		// 현재 픽셀 값이 255이고, 다음 픽셀 값이 0인 경우
		if (matOutputIlium.at<uchar>(temp_y, temp_x) == 255)
		{ 
			if (matOutputIlium.at<uchar>(new_y, new_x) == 0) {
				iliumPoint = Point(new_x, new_y);
			}
		}
		temp_x = new_x;
		temp_y = new_y;

		++step;
	}

	//Labrum point
	Point2i labrumPoint;
	vector<Point> whitePixels;
	for (int y = 0; y < matOutputLabrum.rows; ++y) {
		for (int x = 0; x < matOutputLabrum.cols; ++x) {
			if (matOutputLabrum.at<uchar>(y, x) == 255) { // 흰색 픽셀 (밝기가 255)
				whitePixels.push_back(Point(x, y));
			}
		}
	}
	for (const auto& pixel : whitePixels) {
		labrumPoint.x += pixel.x;
		labrumPoint.y += pixel.y;
	}
	labrumPoint.x /= whitePixels.size();
	labrumPoint.y /= whitePixels.size();;
	circle(matDstImage, labrumPoint, 3, Scalar(255, 0,0), 1, LINE_AA);
	
	//Bonyrim point
	Point2i bonyrimPoint = orthogonalPoint(matOutputBonyRim, pcaPoint);
	circle(matDstImage, bonyrimPoint, 3, Scalar(0, 255, 255), 1, LINE_AA);

	
	//Lowerlimb point
	Point2i lowerlimbPoint= orthogonalPoint(matOutputLowerLimb, pcaPoint);
	circle(matDstImage, lowerlimbPoint, 3, Scalar(0, 255, 0), 1, LINE_AA);

	//Bony roof line
	lineDraw(bonyrimPoint, labrumPoint, matDstImage, Scalar(255,0,0));
	dAlphaAngle = fabs(atan2(labrumPoint.y - bonyrimPoint.y, labrumPoint.x - bonyrimPoint.x) * 180.0 / CV_PI- dBaselineAngle);

	//Carilage roof line
	lineDraw(iliumPoint, lowerlimbPoint, matDstImage, Scalar(0, 0, 255));
	dBetaAngle = fabs(atan2(lowerlimbPoint.y - iliumPoint.y, lowerlimbPoint.x - iliumPoint.x) * 180.0 / CV_PI - dBaselineAngle);
	return matDstImage;
}