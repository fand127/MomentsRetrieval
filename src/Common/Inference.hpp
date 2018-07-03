#ifndef _INFERENCE_CPP_
#define _INFERENCE_CPP_
#include <stdlib.h>
#include <stdio.h>
#include "Utils.hpp"
#include "Common\DateType.hpp"
#include "UI\UI.hpp"

#ifdef OPENCVUI
#include "opencv2\dnn.hpp"
#endif

template <class dType>
class Inference {
public:
	enum class Platform {
		Caffe = 0,
		Tensorflow,
		Caffe2,
		Torch
	};

protected:
	Platform ptype;
#ifdef OPENCVUI
	cv::dnn::Net *pNet;
	cvUI mUI;
#endif
	std::vector<Otensor<dType>> data;
	std::vector<std::string> datanames;
public:
	Inference(Platform type) { ptype = type; pNet = NULL; };
	virtual ~Inference() { if (pNet) delete pNet; };
	
	void LoadModel(const std::string &, const std::string & str2 = ""); //graph and weight
	
	void Run(Otensor<dType> &output);
	
	void SetInput(Otensor<dType>, std::string name = "_");
};


template <class dType>
void Inference<dType>::SetInput(Otensor<dType> data,  std::string name = "_")
{

	data.push_back(data);
	datanames.push_back(name);

}

template <class dType>
void Inference<dType>::Run(Otensor<dType> &output)
{
	_FUNCTION_E_;
#ifdef OPENCVUI	
	for(uint32_t i = 0; i < data.size();++i)
	{
		cv::Mat image;
		mUI.Tensor2Mat(data.at(i), image);
		cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(300, 300), cv::Scalar(103.939, 116.779, 123.68), false, false);
		pNet->setInput(image);
	}

	cv::Mat result = pNet->forward();
	mUI.Mat2Tensor(result, output);
#endif
	return;
	_FUNCTION_X_;
}

#endif