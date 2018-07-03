#ifndef _UI_HPP_
#define _UI_HPP_

#include "Common/Utils.hpp"
#include "Common/DateType.hpp"
#define OPENCVUI

#ifdef OPENCVUI
#include "opencv2\opencv.hpp"
#endif

using namespace cv;
class cvUI {
	cv::VideoCapture cap;
public:
	cvUI() {};
	virtual ~cvUI() {
		if (cap.isOpened())
			cap.release();
	};
	int LoadVideo(std::string& videoname);
	int LoadVideo();
	int CloseVideo() { if (cap.isOpened())cap.release(); }
	int ShowVideo();
	int ShowVideo(const cv::Mat&, int waitmm = 0);
	void getVideoFrame(cv::Mat&);

	int Tensor2Mat(ftensor&tensor, cv::Mat& mat);
	int Tensor2Mat(utensor&tensor, cv::Mat& mat);
	int Mat2Tensor(cv::Mat& mat, utensor&tensor);
	int Mat2Tensor(cv::Mat& mat, ftensor&tensor);
};


#endif