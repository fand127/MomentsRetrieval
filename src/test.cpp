#include <stdlib.h>
#include <stdio.h>
#include "filereadstream.h"
#include "document.h"
#include <string>  
using namespace rapidjson;
using namespace std;
#include <H5Cpp.h>
#include "LanguageProcessing.hpp"
#include "VisualProcessing.hpp"
#include "caffeinc/caffeinc.hpp"

void evaluateVideoRecall()
{
	uint32_t top1count = 0, top5count = 0, querycount = 0, videocount = 0;
	uint32_t querymax = 5, videomax = 30;

	//Caffe::SetDevice(1);
	Caffe::set_mode(Caffe::CPU);
	string networkfile = "../../data/prototxts/deploy_clip_retrieval_rgb_iccv.prototxt";
	string weights = "../../data/snapshots/rgb_iccv_release_feature_process_iter_30000.caffemodel";
	Net<float> caffe_net(networkfile, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(weights);

	//visual
	VisualProcessing<float> vp;
	vp.LoadDataFromFile();
	//Load text
	LangProcessing<float> lp;
	lp.LoadDataFromFile();
	LanguageFeature<float> lf = lp.ExtractNextLanuguageData();
	while(lf.is_new && lf.video_id.size() > 0 && querycount < querymax)
	{
		boost::shared_ptr <Blob< float >> pBlobLData = caffe_net.blob_by_name("text_data");
		pBlobLData->set_cpu_data(lf.pData_vec);
		boost::shared_ptr <Blob< float >> pBlobLcont = caffe_net.blob_by_name("cont_data");
		pBlobLcont->set_cpu_data(lf.pData_cont);
		cout << "feed language data to gpu" << endl;

	
		VisualFeature<float> fp1 = vp.ExtractNextVisualFeature();
		
		//use map instead of mutlipla since same video will give same result
		map<float, string> val;
		videocount = 0;
		while (fp1.is_new && fp1.video_id.size() > 0)
		{
			boost::shared_ptr <Blob< float >> pBlobData = caffe_net.blob_by_name("image_data");
			pBlobData->set_cpu_data(fp1.pData_feature);
			boost::shared_ptr <Blob< float >> pBlobLoc = caffe_net.blob_by_name("loc_data");
			pBlobLoc->set_cpu_data(fp1.pData_loc);
			cout << "feed visual data to gpu" << endl;

			caffe_net.Forward();

			const boost::shared_ptr <Blob< float >> pBlobScore = caffe_net.blob_by_name("rank_score");
			const float *result = pBlobScore->cpu_data();
			vector<int> shape   = pBlobScore->shape();
			cout << "get scored dta to cpu" << endl;
			///////////////////////////
			uint32_t i = 0;
			float minimum = 100000;
			while (i < shape.at(0))
			{
				if (minimum > result[i]) minimum = result[i];
				++i;
			}			
			cout << "video_id : " << fp1.video_id.at(0) << endl;
			val.insert(pair<float, string>(minimum, fp1.video_id.at(0)));			
			videocount++;
			fp1 = vp.ExtractNextVisualFeature();
		}
		cout << "Max Size" << val.max_size() << endl;
		cout << "minimum size" << val.size() << endl;
		cout << "video count" << videocount << endl;
		cout << "query count " << querycount << endl;
		uint32_t count = 0;
		std::map<float, string>::iterator it = val.begin();
		std::ofstream ofs;
		ofs.open(to_string(querycount));
		ofs << "language video id" << lf.video_id.at(0) << endl;
		for (it = val.begin(); it != val.end(); it++)
			ofs << "video score " << it->first << "\t video id" << it->second << endl;
		ofs.close();				
		
		for (it = val.begin(); count < 10 && it != val.end(); ++it, ++count)
		{
			if (it == val.begin())
			{
				if (it->second.compare(lf.video_id.at(0)) == 0)
				{
					top1count++;
					top5count++;
				}
			}
			else if (it->second.compare(lf.video_id.at(0)) == 0)
			{
				cout << "top 5 hit " << it->second << "language video id" << lf.video_id.at(0) << endl;
				top5count++;
			}
		}
		cout << "query count " << querycount << endl;
		querycount++;
		lf = lp.ExtractNextLanuguageData();
	}
	std::cout << "total query " << querycount << endl;
	std::cout << "top1 count " << top1count << endl;
	std::cout << "top5 count " << top5count << endl;
}

void evaluateMomentsRecall()
{

	uint32_t top1count = 0, top5count = 0, querycount = 0, videocount = 0;
	uint32_t querymax = 100, videomax = 10;

	//Caffe::SetDevice(1);
	Caffe::set_mode(Caffe::CPU);
	string networkfile = "../../data/prototxts/deploy_clip_retrieval_rgb_iccv.prototxt";
	string weights = "../../data/snapshots/rgb_iccv_release_feature_process_iter_30000.caffemodel";
	Net<float> caffe_net(networkfile, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(weights);

	//visual
	VisualProcessing<float> vp;
	vp.LoadDataFromFile();
	//Load text
	LangProcessing<float> lp;
	lp.LoadDataFromFile();
	LanguageFeature<float> lf = lp.ExtractNextLanuguageData();
	while (lf.is_new && lf.video_id.size() > 0)
	{
		boost::shared_ptr <Blob< float >> pBlobLData = caffe_net.blob_by_name("text_data");
		pBlobLData->set_cpu_data(lf.pData_vec);
		boost::shared_ptr <Blob< float >> pBlobLcont = caffe_net.blob_by_name("cont_data");
		pBlobLcont->set_cpu_data(lf.pData_cont);
		cout << "feed language data to gpu" << endl;


		VisualFeature<float> fp1 = vp.ExtractNextVisualFeature();

		//use mulimap since we want every moment score out
		multimap<float, uint32_t> val;
		videocount = 0;
		if (fp1.is_new && fp1.video_id.size() > 0)
		{
			boost::shared_ptr <Blob< float >> pBlobData = caffe_net.blob_by_name("image_data");
			pBlobData->set_cpu_data(fp1.pData_feature);
			boost::shared_ptr <Blob< float >> pBlobLoc = caffe_net.blob_by_name("loc_data");
			pBlobLoc->set_cpu_data(fp1.pData_loc);
			cout << "feed visual data to gpu" << endl;

			caffe_net.Forward();

			const boost::shared_ptr <Blob< float >> pBlobScore = caffe_net.blob_by_name("rank_score");
			const float *result = pBlobScore->cpu_data();
			vector<int> shape   = pBlobScore->shape();
			cout << "get scored data to cpu" << endl;
			uint32_t i = 0;
			while (i < shape.at(0))
			{
				val.insert(pair<float, uint32_t>(result[i], i));
				++i;
			}
			videocount++;
			
			uint32_t index = 0;
			while ((fp1.possible_segment.at(index).first != lf.mstart_seg || fp1.possible_segment.at(index).second != lf.mend_seg) && index < fp1.possible_segment.size())
			{
				index++;
			}
			i = 0;
			std::map<float, uint32_t>::iterator it = val.begin();
			for (it = val.begin(); i < 10 && it != val.end(); ++it, ++i)
			{				
				if (it->second == index && i == 0)
				{
					top1count++;
					top5count++;
				}
				else if (it->second == index&& i < 5)
					top5count++;
			}
		}		
		cout << "query count " << querycount << endl;
		querycount++;
		lf = lp.ExtractNextLanuguageData();
	}
	std::cout << "total query " << querycount << endl;
	std::cout << "top1 count " << top1count << endl;
	std::cout << "top5 count " << top5count << endl;
}

int main()
{
	//evaluateMomentsRecall();
	evaluateVideoRecall();
	
	/*
	Caffe::set_mode(Caffe::CPU);
	string networkfile = "../../data/prototxts/deploy_clip_retrieval_rgb_iccv.prototxt";
	string weights = "../../data/snapshots/rgb_iccv_release_feature_process_iter_30000.caffemodel";
	Net<float> caffe_net(networkfile, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(weights);

	//Load Visual 
	VisualProcessing<float> vp;
	vp.LoadDataFromFile();
	VisualFeature<float> fp1 = vp.ExtractNextVisualFeature();

	boost::shared_ptr <Blob< float >> pBlobData = caffe_net.blob_by_name("image_data");
	pBlobData->set_cpu_data(fp1.pData_feature);
	boost::shared_ptr <Blob< float >> pBlobLoc = caffe_net.blob_by_name("loc_data");
	pBlobLoc->set_cpu_data(fp1.pData_loc);
	
	//Load text
	LangProcessing<float> lp;
	lp.LoadDataFromFile();
	LanguageFeature<float> lf = lp.ExtractNextLanuguageData();

	boost::shared_ptr <Blob< float >> pBlobLData = caffe_net.blob_by_name("text_data");
	pBlobLData->set_cpu_data(lf.pData_vec);
	boost::shared_ptr <Blob< float >> pBlobLcont = caffe_net.blob_by_name("cont_data");
	pBlobLcont->set_cpu_data(lf.pData_cont);
	
	caffe_net.Forward();
	
	const boost::shared_ptr <Blob< float >> pBlobScore = caffe_net.blob_by_name("rank_score");
	
	const float *result = pBlobScore->cpu_data();
	vector<int> shape = pBlobScore->shape();

	FILE *fp = fopen("rank.out", "w");
	uint32_t i = 0;
	while (i < shape.at(0))
	{
	fprintf(fp, "%1.5f\n", result[i]);
	++i;
	}
	fclose(fp);
	*/
	return 0;
}

