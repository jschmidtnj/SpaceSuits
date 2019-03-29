//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
// OpenCVHelper.h

#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>

namespace OpenCVBridge
{
	public ref class OpenCVHelper sealed
	{
	public:
		OpenCVHelper();

		// Image processing operators
		void Handhold_Detect(
			Windows::Graphics::Imaging::SoftwareBitmap^ input,
			Windows::Graphics::Imaging::SoftwareBitmap^ output);
	private:
		// used only for the background subtraction operation
		cv::Mat fgMaskMOG2;
		cv::Ptr<cv::BackgroundSubtractor> pMOG2;

		// helper functions for getting a cv::Mat from SoftwareBitmap
		bool GetPointerToPixelData(Windows::Graphics::Imaging::SoftwareBitmap^ bitmap,
			unsigned char** pPixelData, unsigned int* capacity);

		bool TryConvert(Windows::Graphics::Imaging::SoftwareBitmap^ from, cv::Mat& convertedMat);
	};
}
