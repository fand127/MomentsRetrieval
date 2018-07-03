#pragma once
#ifndef _VIDEO_SEARCH_HPP_
#define _VIDEO_SEARCH_HPP_

#include "Common/DataProcessing.hpp"
#include "Common/DateType.hpp"
#include "Common/VideoPreprocess.hpp"

#ifndef __ANDROID__
#include "UI/UI.hpp"
#endif 

#define MAX_FRAME_DATA 5
using namespace std;

typedef struct singledetclassdata {
	string classname;
	float confidence;
	uint16_t bbox[4];
}SData;

typedef struct rawframedata {
	SData data[MAX_FRAME_DATA];
	uint32_t frame_indx;
	uint32_t video_id;
	string video_name;
}RawFrameData;

typedef struct framedata {
	vector<SData> data;
	uint32_t frame_indx;
	uint32_t video_id;
	string video_name;
}FrameData;

typedef struct catdata {
	SData data;
	uint32_t frameindx_s;
	uint32_t frameindx_e;
}CatData;

typedef struct videometadata {
	multimap<string, CatData> rawobjlist; //class names. start and end 
	uint32_t video_id;
	string video_name;
}VideoData;

typedef struct postvideometadata {
	multimap<string, uint32_t> objlist; //<classname, startframeindx>
	uint32_t video_id;
	string video_name;
}PostVideoData;



template <class dType>
class VideoSearch :public VideoPreprocess<dType> {
public:
	VideoSearch() {};
	virtual ~VideoSearch() {};
/*
	unordered_map<uint32_t, string> videos;
	PostVideoData mPVD;
public:
	int Init();
	int LoadVideo(string& videoname);
	//For use in frame to frame
	Otensor<dType> ProcessFrame(const Otensor<dType>& image);
	//For use in generating Meta Data
	FrameData ProcessFrame(const Otensor<dType>& image);
	Otensor<dType> getCurrentVideoFrame();

	int Deinit();
	*/
};


#endif