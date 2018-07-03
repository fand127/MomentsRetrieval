#pragma once
#ifndef _VIDEOPREPROCESS_HPP_
#define _VIDEOPREPROCESS_HPP_

#include "DataProcessing.hpp"
#include "DateType.hpp"

#ifndef __ANDROID__
#include "UI/UI.hpp"
#endif 

using namespace std;

typedef struct frame {
	utensor framedata;
	string video_name;
	uint32_t frame_indx;
	uint32_t video_id;
	void *metadata;
}VideoFrame;

typedef struct video {
	//frame id and actual frame
	map<uint32_t, VideoFrame> frames;
	uint32_t video_id;
	string video_name;
	void *metadata;
}Video;

template <class dType>
class VideoPreprocess :public DataProcessing {
public:
	VideoPreprocess() {};
	virtual ~VideoPreprocess() {};

	uint32_t frameidcount;
	string videoname;
	bool inited = false;
#ifdef OPENCVUI
	cvUI mUI;
#endif 

public:
	//For use in frame to frame
	virtual Otensor<dType> ProcessFrame(const Otensor<dType>& image) { Otensor<dType> data; return data; };
	utensor buffer[2]; // for display
	uint32_t cur_disp = 0;
public:
	int Init(utensor &&tensor);
	
	int LoadVideo(string& videoname);
	int LoadVideo();
	VideoFrame getNextVideoFrame();
	int ShowVideo();
	int ShowVideo(VideoFrame&, int waitmm = 0);
	int Deinit();
};

template <class dType>
int VideoPreprocess<dType>::Init(utensor &&tensor)
{
	int ret = 0;
	frameidcount = 0;
	buffer[0] = utensor(tensor.dims[1], tensor.dims[2], tensor.dims[3]);
	buffer[1] = tensor;
	inited = true;
	return ret;
}

template <class dType>
int VideoPreprocess<dType>::Deinit()
{
	int ret = 0;
	inited = false;
	return ret;
}

template <class dType>
int VideoPreprocess<dType>::LoadVideo(string& videoname)
{
	int ret = 0;
	if (!inited)
		return -1;
#ifdef OPENCVUI
	mUI.LoadVideo(videoname);
#endif 
	return ret;
}
template <class dType>
int VideoPreprocess<dType>::LoadVideo()
{
	int ret = 0;
	if (!inited)
		return -1;
#ifdef OPENCVUI
	mUI.LoadVideo();
#endif 
	return ret;
}
template <class dType>
int VideoPreprocess<dType>::ShowVideo()
{
	int ret = 0;
#ifdef OPENCVUI
	mUI.ShowVideo();
#endif
	return ret;
}

template <class dType>
int VideoPreprocess<dType>::ShowVideo(VideoFrame& frame, int waitmm = 0)
{
	int ret = 0;
#ifdef OPENCVUI
	cv::Mat mat;
	mUI.Tensor2Mat(frame.framedata, mat);
	mUI.ShowVideo(mat, waitmm);
#endif
	return ret;
}

template <class dType>
VideoFrame VideoPreprocess<dType>::getNextVideoFrame()
{
	VideoFrame frame;
#ifdef OPENCVUI	
	cv::Mat mat(buffer[0].dims[1], buffer[0].dims[2], CV_8UC3, buffer[cur_disp].pData);
	mUI.getVideoFrame(mat);
	cur_disp = (cur_disp + 1) % 2;   //ping-pong buffer
	utensor tensor;
	mUI.Mat2Tensor(mat, tensor);	
	frame.frame_indx = frameidcount;
	frame.framedata  = tensor;
	frame.metadata   = NULL;
	frame.video_id   = 0;
	frame.video_name = " ";
	frameidcount++;
#endif
	return frame;
}

#endif