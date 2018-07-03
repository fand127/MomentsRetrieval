#include <stdlib.h>
#include <stdio.h>
#include "filereadstream.h"
#include "document.h"
#include <string>  
using namespace rapidjson;
using namespace std;
#include <H5Cpp.h>

#include "VideoSearch.hpp"

#include "opencv2\opencv.hpp"
#include "opencv2\dnn.hpp"
#include "Common/Inference.hpp"

using namespace cv;
int main()
{

	utensor a(3, 4);
	//utensor b = a.Transpose(0,1,3,2);
	/*
	VideoSearch<float> vs;
	vs.Init(utensor(480, 640, 3));	
	vs.LoadVideo();		
	while(true)
	{ 
		VideoFrame frame = vs.getNextVideoFrame();
		vs.ShowVideo(frame, 30);
	}
	*/
	//vs.ShowVideo();
	/*
	VideoPreprocess<float> vp;
	//vp.LoadVideo(string("C:/Work/Project/C-code/data/videosearch/test1.wmv"));
	vp.LoadVideo();
	//vp.ShowVideo();
	 	
	Inference infe(Inference::Platform::Torch);
	infe.LoadModel("C:/Work/Project/C-code/data/videosearch/fast_neural_style_eccv16_starry_night.t7");
	//infe.LoadModel("C:/Work/Project/C-code/data/videosearch/fast_neural_style_instance_norm_feathers.t7");
	while(true)
	{
		VideoFrame input = vp.getNextVideoFrame();
		Mat frame;
		cvUI mUI;
		//mUI.Tensor2Mat(input.framedata, frame);
		//Mat blob = cv::dnn::blobFromImage(frame, 1.0, Size(300, 300), Scalar(103.939, 116.779, 123.68), false, false);		
		utensor outputt;
		infe.Run(input.framedata, outputt);
		Mat output;
		mUI.Tensor2Mat(outputt, output);
		int size[3] = {output.size[1],output.size[2], output.size[3]};
		Mat detection(3, size, CV_32FC1, output.ptr<float>());
		size[0] = output.size[2];
		size[1] = output.size[3];
		size[2] = output.size[1];
		Mat detectionMat(output.size[2], output.size[3], CV_32FC3);
		for(uint32_t i = 0; i < 300; ++ i)
			for (uint32_t j = 0; j < 300; ++j)
			{
				detectionMat.at<Vec3f>(i, j).val[0] = detection.at<float>(0, i, j);
				detectionMat.at<Vec3f>(i, j).val[1] = detection.at<float>(1, i, j);
				detectionMat.at<Vec3f>(i, j).val[2] = detection.at<float>(2, i, j);
			}
				
		//vp.ShowVideo(detectionMat,30);
	}
	*/
	return 0;
}

