#include "Inference.hpp"

using namespace cv;
template <class dType>
void Inference<dType>::LoadModel(const std::string &str1, const std::string &str2)
{
	_FUNCTION_E_;
	if (ptype == Platform::Caffe)
	{
		if (pNet)
			delete pNet;
		pNet = new cv::dnn::Net();
		*pNet = cv::dnn::readNetFromCaffe(str1, str2);
	}
	else if (ptype == Platform::Tensorflow)
	{
		if (pNet)
			delete pNet;
		pNet = new cv::dnn::Net();
		*pNet = cv::dnn::readNetFromTensorflow(str1, str2);
	}
	else if (ptype == Platform::Torch)
	{
		if (pNet)
			delete pNet;
		pNet = new cv::dnn::Net();
		*pNet = cv::dnn::readNetFromTorch(str1);
	}
	_FUNCTION_X_;
}
