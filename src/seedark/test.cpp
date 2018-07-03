#include <stdlib.h>
#include <stdio.h>
#include "filereadstream.h"
#include "document.h"
#include <string>  
using namespace rapidjson;
using namespace std;
#include <H5Cpp.h>

#include "libraw.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"

#include "Preprocess.hpp"

#include "opencv2\opencv.hpp"
#include "opencv2\dnn.hpp"

using namespace cv;
int main()
{
	/*
	Preprocess<float> imgprocess;
	PreprocessPara para;
	imgprocess.ReadRaw(para);
	imgprocess.Fujipreprocess();	
	//ftensor a(4, 5,5);
	//a(2, 1,1) = 5;
	*/
	//String weights = "../../../data/seedark/frozen_inference_graph.pb";
	String prototxt = "../../../data/seedark/ssd_mobilenet_v2_coco_2018_03_29.pbtxt";
	String weights = "../../../data/seedark/ssd_mobilenet_v2_coco_2018_03_29.pb";
	dnn::Net net = cv::dnn::readNetFromTensorflow(weights, prototxt);
	//dnn::Net net = cv::dnn::readNetFromTensorflow(weights);

	//Mat frame = imread("../../../data/seedark/download.jpg");
	
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	while (true)
	{
		Mat frame;
		cap >> frame;
		cv::resize(frame, frame, Size(300, 300));
	
		Size frame_size = frame.size();

		cv::Mat blob = cv::dnn::blobFromImage(frame, 1. / 255, Size(300, 300));
		net.setInput(blob);
		Mat output = net.forward();
		Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());

		string classNames[] = {"background", "person", "bicycle", "car", "motorcycle","airplane", "bus",
			"train", "truck", "boat", "traffic light", "fire hydrant",
			"stop sign", "parking meter", "bench",  "bird", "cat",
			"dog", "horse", "sheep", "cow", "elephant", "bear",
			"zebra", "giraffe", "backpack", "umbrella", "handbag",
			"tie", "suitcase", "frisbee", "skis", "snowboard",
			"sports ball", "kite",  "baseball bat",  "baseball glove",
			"skateboard", "surfboard","tennis racket", "bottle",
			"wine glass", "cup",  "fork", "knife", "spoon",
			"bowl", "banana", "apple", "sandwich", "orange",
			"broccoli", "carrot", "hot dog","pizza", "donut",
			"cake", "chair",  "couch", "potted plant",  "bed",
			"dining table", "toilet", "tv", "laptop","mouse",
			"remote", "keyboard", "cell phone", "microwave","oven",
			"toaster", "sink", "refrigerator",  "book", "clock",
			"vase", "scissors", "teddy bear", "hair drier", "toothbrush" };

		float confidenceThreshold = 0.20;
		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);

			if (confidence > confidenceThreshold)
			{
				size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));

				int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
				int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
				int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
				int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

				ostringstream ss;
				ss << confidence;
				String conf(ss.str());

				Rect object((int)xLeftBottom, (int)yLeftBottom,
					(int)(xRightTop - xLeftBottom),
					(int)(yRightTop - yLeftBottom));

				rectangle(frame, object, Scalar(0, 255, 0), 2);
				//String label = String(classNames[objectClass]) + ": " + conf;
				String label = "face : " + conf;
				int baseLine = 0;
				Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
				rectangle(frame, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height),
					Size(labelSize.width, labelSize.height + baseLine)),
					Scalar(0, 255, 0), CV_FILLED);
				putText(frame, label, Point(xLeftBottom, yLeftBottom),
					FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
			}
		}
		namedWindow("image", CV_WINDOW_NORMAL);
		imshow("image", frame);
		waitKey(30);

	}
 	return 0;
}

