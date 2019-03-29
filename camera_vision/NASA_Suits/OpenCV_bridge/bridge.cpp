//*********************************************************
//
// Copyright (c) RONNIE INCORPORATED COMPANY LLC. All rights reserved.
// This code is licensed under the SIT License (SIT).
// THIS CODE IS PROVIDED WITHOUT WARRANTY (BECUASE EVEN I DONT UNDERSTAND IT, SO IF YOU BREAK IT, THATS ON YOU) OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
// OpenCVHelper.cpp

#include "pch.h"
#include "OpenCVHelper.h"
#include "MemoryBuffer.h"

using namespace Microsoft::WRL;

using namespace OpenCVBridge;
using namespace Platform;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

using namespace cv;
using namespace std;

#define threshold 0.9
#define median_window_size 7 //HAS TO BE AN ODD NUMBER
#define ce_window_size 11 //HAS TO BE AN ODD NUMBER
#define fill_const 0.10
uchar color[] = { 255, 255, 255, 100, 220, 100 };
//White = {255,255,255}
//Grenn  = {100, 255, 100}

OpenCVHelper::OpenCVHelper()
{
	pMOG2 = createBackgroundSubtractorMOG2();
}

bool OpenCVHelper::TryConvert(SoftwareBitmap^ from, Mat& convertedMat)
{
	unsigned char* pPixels = nullptr;
	unsigned int capacity = 0;
	if (!GetPointerToPixelData(from, &pPixels, &capacity))
	{
		return false;
	}

	Mat mat(from->PixelHeight,
		from->PixelWidth,
		CV_8UC4, // assume input SoftwareBitmap is BGRA8
		(void*)pPixels);

	// shallow copy because we want convertedMat.data = pPixels
	// don't use .copyTo or .clone
	convertedMat = mat;
	return true;
}

bool OpenCVHelper::GetPointerToPixelData(SoftwareBitmap^ bitmap, unsigned char** pPixelData, unsigned int* capacity)
{
	BitmapBuffer^ bmpBuffer = bitmap->LockBuffer(BitmapBufferAccessMode::ReadWrite);
	IMemoryBufferReference^ reference = bmpBuffer->CreateReference();

	ComPtr<IMemoryBufferByteAccess> pBufferByteAccess;
	if ((reinterpret_cast<IInspectable*>(reference)->QueryInterface(IID_PPV_ARGS(&pBufferByteAccess))) != S_OK)
	{
		return false;
	}

	if (pBufferByteAccess->GetBuffer(pPixelData, capacity) != S_OK)
	{
		return false;
	}
	return true;
}

void prewitt_gradient(const Mat& myImage, Mat& Result){
	Mat grad_x, grad_y;
	float hist[256];
	float step;
	uchar thresh;
	Mat prewitt_y = (Mat_<char>(3, 3) <<
		1, 1, 1,
		0, 0, 0,
		-1, -1, -1);

	Mat prewitt_x = (Mat_<char>(3, 3) <<
		1, 0, -1,
		1, 0, -1,
		1, 0, -1);


	CV_Assert(myImage.depth() == CV_8U);

	filter2D(myImage, grad_x, myImage.depth(), prewitt_x);
	filter2D(myImage, grad_y, myImage.depth(), prewitt_y);

	const int nChannels = myImage.channels();
	Result.create(myImage.size(), myImage.type());

	step = 1 / ((float)myImage.rows*(float)myImage.cols);

	for (int i = 0; i < 256; i++){
		hist[i] = 0;
	}

	for (int j = 0; j < myImage.rows; j++)
	{
		const uchar* x_current = grad_x.ptr<uchar>(j);
		const uchar* y_current = grad_y.ptr<uchar>(j);
		uchar* output = Result.ptr<uchar>(j);
		for (int i = 0; i < nChannels*(myImage.cols); i++)
		{
			*output = saturate_cast<uchar>((abs(x_current[i]) + abs(y_current[i])));
			hist[*output] += step;
			output++;
		}
	}
	float sum = 0;
	for (int i = 0; i < 256; i++){
		sum = sum + hist[i];
		if (sum >= threshold){
			thresh = i;
			break;
		}
	}
	for (int j = 0; j < myImage.rows; j++)
	{
		uchar* output = Result.ptr<uchar>(j);
		for (int i = 0; i < nChannels*(myImage.cols); i++)
		{
			if (*output >= thresh){
				*output = 255;
			}
			else{
				*output = 0;
			}
			output++;
		}
	}
}

void color_filter(const Mat& myImage, Mat& Result){
	Mat color;
	uchar local[median_window_size*median_window_size];

	CV_Assert(myImage.depth() == CV_8U);

	//**CREATE COLOR MASK BASED ON PIXEL COLOR VALUES IN ORIGINAL IMAGE**//
	const int nChannels = myImage.channels();
	color.create(myImage.size(), CV_8UC1);

	for (int j = 0; j < myImage.rows; j++){
		const uchar* current_b = myImage.ptr<uchar>(j);
		const uchar* current_g = current_b + 1;
		const uchar* current_r = current_g + 1;
		uchar* output = color.ptr<uchar>(j);
		for (int i = 0; i < myImage.cols; i++){
			*output = 0;
			if ((*current_b > 30) && (*current_b < 140)){
				if ((*current_g > 90) && (*current_g < 200)){
					if ((*current_r > 140) && (*current_r < 250)){
						*output = 255;
					}
				}
			}
			output++;
			current_b += 3;
			current_g += 3;
			current_r += 3;
		}
	}
	Result = color.clone();
}

void binary_image_mix(const Mat& img1, const Mat& img2, Mat& Result, int type){
	CV_Assert(img1.depth() == CV_8U);
	CV_Assert(img2.depth() == CV_8U);
	if (img1.rows != img2.rows){
		//cout << "ERROR: IMAGE ROWS DO NOT MATCH" << endl;
		//cout << "Image 1 rows: " << img1.rows << "\t Image 2 rows: " << img2.rows << endl;
		if (img1.cols != img2.cols){
			//cout << "ERROR: IMAGE COLUMNS DO NOT MATCH" << endl;
			//cout << "Image 1 cols: " << img1.cols << "\t Image 2 cols: " << img2.cols << endl;
			return;
		}
	}

	Result.create(img1.size(), img1.type());
	for (int k = 0; k < Result.rows; k++){
		Result.row(k).setTo(Scalar(0));
	}

	for (int j = 0; j < Result.rows; j++){
		const uchar* current_1 = img1.ptr<uchar>(j);
		const uchar* current_2 = img2.ptr<uchar>(j);
		uchar* output = Result.ptr<uchar>(j);
		for (int i = 0; i < Result.cols; i++){
			if (type == 0){
				//OR FUNCTION
				if ((*current_1 == 255) || (*current_2 == 255)){
					*output = 255;
				}
			}
			if (type == 1){
				//AND FUNCTION
				if ((*current_1 == 255) && (*current_2 == 255)){
					*output = 255;
				}
			}
			current_1++;
			current_2++;
			output++;
		}
	}
}

float white_count(const Mat& input){
	int count = 0;
	for (int j = 0; j < input.rows; j++){
		const uchar* row = input.ptr<uchar>(j);
		for (int i = 0; i < input.cols; i++){
			if (*row > 250){
				count++;
			}
			row++;
		}
	}
	return (float)((float)count / ((float)input.rows*(float)input.cols));
}

void fill(const Mat& img, Mat& Result){
	CV_Assert(img.depth() == CV_8U);

	Result.create(img.size(), img.type());
	for (int k = 0; k < Result.rows; k++){
		Result.row(k).setTo(Scalar(0));
	}

	float percent_color = white_count(img) + fill_const;

	for (int j = ((ce_window_size - 1) / 2); j < (Result.rows - ((ce_window_size - 1) / 2)); j++){
		uchar* center = Result.ptr<uchar>(j);
		for (int i = 1 + ((ce_window_size - 1) / 2); i < (Result.cols - ((ce_window_size - 1) / 2)); i++){
			Mat sub_a(img, Rect((i - (ce_window_size - 1) / 2), (j - (ce_window_size - 1) / 2), ce_window_size, ce_window_size));
			*center = 0;
			if (white_count(sub_a) > percent_color){
				*center = 255;
			}
			center++;
		}
	}

}

void OpenCVHelper::Handhold_Detect(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
	Mat inputMat, outputMat;
	Mat src_gray, src_color, edge_mask, color_mask, binary_mix, final_mask;
	if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
	{
		return;
	}
	//OPENCV CODE HERE
	src_color = inputMat;
	cvtColor(inputMat, src_gray, COLOR_RGB2GRAY); //Gray Scale
	prewitt_gradient(src_gray, edge_mask);
	color_filter(src_color, color_mask);
	binary_image_mix(color_mask, edge_mask, binary_mix, 1);
	fill(binary_mix, outptutMat);
	
}

/*
void OpenCVHelper::Blur(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
	Mat inputMat, outputMat;
	if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
	{
		return;
	}

	blur(inputMat, outputMat, cv::Size(5, 5));
}

void OpenCVHelper::MotionDetector(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
	Mat inputMat, outputMat;
	if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
	{
		return;
	}

	pMOG2->apply(inputMat, fgMaskMOG2);
	int type = fgMaskMOG2.type();
	Mat temp;
	cvtColor(fgMaskMOG2, temp, CV_GRAY2BGRA);

	Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
	erode(temp, temp, element);
	temp.copyTo(outputMat);
}

void OpenCVHelper::Histogram(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
	Mat inputMat, outputMat;
	if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
	{
		return;
	}

	std::vector<Mat> bgr_planes;
	split(inputMat, bgr_planes);
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	int hist_w = outputMat.cols; int hist_h = outputMat.rows;
	double bin_w = (double)outputMat.cols / histSize;

	normalize(b_hist, b_hist, 0, outputMat.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, outputMat.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, outputMat.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < histSize; i++)
	{
		int x1 = cvRound(bin_w * (i - 1));
		int x2 = cvRound(bin_w * i);
		line(outputMat, cv::Point(x1, hist_h - cvRound(b_hist.at<float>(i - 1))),
			cv::Point(x2, hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0, 255), 2, 8, 0);
		line(outputMat, cv::Point(x1, hist_h - cvRound(g_hist.at<float>(i - 1))),
			cv::Point(x2, hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0, 255), 2, 8, 0);
		line(outputMat, cv::Point(x1, hist_h - cvRound(r_hist.at<float>(i - 1))),
			cv::Point(x2, hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255, 255), 2, 8, 0);
	}
}

void OpenCVHelper::Contours(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
	Mat inputMat, outputMat;
	if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
	{
		return;
	}

	Mat src_gray;
	Mat canny_output;
	int thresh = 50;
	int max_thresh = 255;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<Vec4i> hierarchy;

	cvtColor(inputMat, src_gray, CV_BGRA2GRAY);
	blur(src_gray, src_gray, cv::Size(3, 3));
	Canny(src_gray, canny_output, thresh, thresh * 3, 3);
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	for (int i = 0; i < contours.size(); i++)
	{
		drawContours(outputMat, contours, i, Scalar(255, 0, 0, 255), 2, 8, hierarchy, 0);
	}
}

void OpenCVHelper::HoughLines(SoftwareBitmap^ input, SoftwareBitmap^ output)
{
	Mat inputMat, outputMat;
	if (!(TryConvert(input, inputMat) && TryConvert(output, outputMat)))
	{
		return;
	}

	inputMat.copyTo(outputMat);

	Mat edges, cdst, src_gray;
	cvtColor(inputMat, src_gray, CV_BGRA2GRAY);
	Canny(src_gray, edges, 100, 200, 3);
	std::vector<Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI / 180, 50, input->PixelWidth / 4, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(outputMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), Scalar(0, 255, 0, 255), 3, CV_AA);
	}
}

bool OpenCVHelper::TryConvert(SoftwareBitmap^ from, Mat& convertedMat)
{
	unsigned char* pPixels = nullptr;
	unsigned int capacity = 0;
	if (!GetPointerToPixelData(from, &pPixels, &capacity))
	{
		return false;
	}

	Mat mat(from->PixelHeight,
		from->PixelWidth,
		CV_8UC4, // assume input SoftwareBitmap is BGRA8
		(void*)pPixels);

	// shallow copy because we want convertedMat.data = pPixels
	// don't use .copyTo or .clone
	convertedMat = mat;
	return true;
}

bool OpenCVHelper::GetPointerToPixelData(SoftwareBitmap^ bitmap, unsigned char** pPixelData, unsigned int* capacity)
{
	BitmapBuffer^ bmpBuffer = bitmap->LockBuffer(BitmapBufferAccessMode::ReadWrite);
	IMemoryBufferReference^ reference = bmpBuffer->CreateReference();

	ComPtr<IMemoryBufferByteAccess> pBufferByteAccess;
	if ((reinterpret_cast<IInspectable*>(reference)->QueryInterface(IID_PPV_ARGS(&pBufferByteAccess))) != S_OK)
	{
		return false;
	}

	if (pBufferByteAccess->GetBuffer(pPixelData, capacity) != S_OK)
	{
		return false;
	}
	return true;
}
*/