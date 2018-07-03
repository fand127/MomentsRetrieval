#include "UI.hpp"


 void cvUI::getVideoFrame(cv::Mat&frame)
{
	Mat readframe;
	cap.read(readframe);
	readframe.copyTo(frame);
	return ;
}


int cvUI::LoadVideo(std::string& videoname)
{
	_FUNCTION_E_;
	if (cap.isOpened())
		cap.release();
	bool ret = cap.open(videoname);
	if (!cap.isOpened())
		return -1;
	_FUNCTION_X_;
	return 0;
}

int cvUI::LoadVideo()
{
	_FUNCTION_E_;
	if (cap.isOpened())
		cap.release();
	cap.open(0);
	if (!cap.isOpened())
		return -1;
	_FUNCTION_X_;
	return 0;
}

int cvUI::ShowVideo()
{
	if (cap.isOpened())
	{
		while (true)
		{
			Mat frame;
			cap >> frame;
			namedWindow("image", CV_WINDOW_NORMAL);
			imshow("image", frame);
			if (waitKey(30) > 0)
				break;
		}
	}
	else return -1;
	return 0;
}

int cvUI::ShowVideo(const cv::Mat&frame, int waitmm)
{
	if (cap.isOpened())
	{
		namedWindow("image", CV_WINDOW_NORMAL);
		moveWindow("image", 0, 0);
		imshow("image", frame);
		waitKey(waitmm);
	}
	return 0;
}


int cvUI::Tensor2Mat(ftensor&tensor, cv::Mat& mat)
{
	int ret = 0;
	if (tensor.dims[3] != 3 || tensor.dims[0] != 1)
		return -1;
	int height  = tensor.dims[1];
	int width = tensor.dims[2];
	mat = cv::Mat(height, width, CV_32FC3, tensor.pData);
	return ret;
}

int cvUI::Tensor2Mat(utensor&tensor, cv::Mat& mat)
{ 
	int ret = 0;
	if (tensor.dims[3] != 3 || tensor.dims[0] != 1)
		return -1;
	int height = tensor.dims[1];
	int width = tensor.dims[2];
	mat = cv::Mat(height, width, CV_8UC3, tensor.pData); //Mat won't deallocate external memory	
	return ret;
}

int cvUI::Mat2Tensor(cv::Mat& mat, utensor&tensor)
{
	int ret = 0;
	tensor.dims[1] = mat.rows;
	tensor.dims[2] = mat.cols;
	tensor.dims[3] = 3;
	tensor = utensor(mat.rows, mat.cols, 3, mat.data);
	return ret;
}

int cvUI::Mat2Tensor(cv::Mat& mat, ftensor&tensor)
{
	int ret = 0;
	tensor.dims[1] = mat.rows;
	tensor.dims[2] = mat.cols;
	tensor.dims[3] = 3;
	tensor = ftensor(mat.rows, mat.cols, 3, (float*)(mat.data));
	return ret;
}