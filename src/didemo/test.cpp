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
	uint32_t querymax = 100, videomax = 1;
	vector<uint32_t> rank_vec;

	//Caffe::SetDevice(1);
	Caffe::set_mode(Caffe::CPU);
	string networkfile = "../../data/didemo/prototxts/deploy_clip_retrieval_rgb_iccv.prototxt";
	string weights = "../../data/didemo/snapshots/rgb_iccv_release_feature_process.caffemodel";
	
	Net<float> caffe_net(networkfile, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(weights);
	
	//visual
	VisualProcessing<float> vp;
	vp.LoadDataFromFile("../../data/didemo/val_data.json", "../../data/didemo/average_fc7.h5");
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
		vector<float> processtime;
		while (fp1.is_new && fp1.video_id.size() > 0)
		{
			boost::shared_ptr <Blob< float >> pBlobData = caffe_net.blob_by_name("image_data");
			pBlobData->set_cpu_data(fp1.pData_feature);
			boost::shared_ptr <Blob< float >> pBlobLoc = caffe_net.blob_by_name("loc_data");
			pBlobLoc->set_cpu_data(fp1.pData_loc);
			cout << "feed visual data to gpu" << endl;
			 
			//Evalutate time 
			for (uint32_t i = 0; i < caffe_net.layers().size() - 1; ++i)
			{
				boost::shared_ptr <caffe::Layer<float>>pLayer = caffe_net.layers().at(i);
				cout << "Layer name : " << pLayer->layer_param().name() << endl;
				clock_t start = clock();
				caffe_net.ForwardFromTo(i, i + 1);
				clock_t end = clock();
				cout << " processing time" << (double)(end - start) / CLOCKS_PER_SEC << endl;
				if (processtime.size() <= i)
				{
					processtime.push_back((float)(end - start) / CLOCKS_PER_SEC);
				}
				else
					processtime.at(i) += (float)(end - start) / CLOCKS_PER_SEC;
			}
			
			//caffe_net.Forward();
			const boost::shared_ptr <Blob< float >> pBlobScore = caffe_net.blob_by_name("rank_score");
			const float *result = pBlobScore->cpu_data();
			vector<int> shape   = pBlobScore->shape();
			cout << "get scored dta to cpu" << endl;
/*
			FILE *fp = fopen("rank.out", "w");
			uint32_t j = 0;
			while (j < shape.at(0))
			{
				fprintf(fp, "%1.5f\n", result[j]);
				++j;
			}
			fclose(fp);
*/
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
		for (uint32_t i = 0; i < processtime.size(); ++i)
			cout << i << "layer processing time" << processtime.at(i) << endl;
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
				
		for (it = val.begin(); it != val.end(); ++it, ++count)
		{
			if (it->second.compare(lf.video_id.at(0)) == 0)
			{
				rank_vec.push_back(count);
				if (it == val.begin())
				{
					top1count++;
					top5count++;
				}
				else if(count < 10)
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
	uint32_t sum = 0;
	for (uint32_t i = 0; i < rank_vec.size(); ++i)
	{
		sum += rank_vec.at(i);
	}
	cout << "average ranking " << (float)sum / rank_vec.size() << endl;
}

void evaluateMomentsRecall()
{

	uint32_t top1count = 0, top5count = 0, querycount = 0, videocount = 0;
	uint32_t querymax = 100, videomax = 10;

	//Caffe::SetDevice(1);
	Caffe::set_mode(Caffe::CPU);
	//string networkfile = "../../data/prototxts/deploy_clip_retrieval_rgb_iccv.prototxt";
	//string weights = "../../data/snapshots/rgb_iccv_release_feature_process_iter_30000.caffemodel";
	string networkfile = "../../data/didemo/prototxts/deploy_clip_retrieval_flow_iccv.prototxt";
	string weights = "../../data/didemo/snapshots/flow_iccv_release_feature_process.caffemodel";
	Net<float> caffe_net(networkfile, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(weights);

	//visual
	VisualProcessing<float> vp;
	vp.LoadDataFromFile("../../data/didemo/val_data.json", "../../data/didemo/average_global_flow.h5");
	//Load text
	LangProcessing<float> lp;
	lp.LoadDataFromFile();
	LanguageFeature<float> lf = lp.ExtractNextLanuguageData();
	
	vector<uint32_t> rank_vec;
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
			for (it = val.begin(); it != val.end(); ++it, ++i)
			{		
				if (it->second == index)
				{
					rank_vec.push_back(i);
					if (i == 0)
					{
						top1count++;
						top5count++;
					}
					else if (i < 5)
						top5count++;
				}			
			}
		}		
		cout << "query count " << querycount << endl;
		querycount++;
		lf = lp.ExtractNextLanuguageData();
	}
	std::cout << "total query " << querycount << endl;
	std::cout << "top1 count " << top1count << endl;
	std::cout << "top5 count " << top5count << endl;
	uint32_t sum = 0;
	for (uint32_t i = 0; i < rank_vec.size(); ++i)
	{
		sum += rank_vec.at(i);
	}
	cout << "average ranking " << (float)sum / rank_vec.size() << endl;
}

void evaluateDataProcessing()
{
	VisualProcessing<float> vp;
	vp.LoadDataFromFile("../../data/didemo/val_data.json", "../../data/didemo/average_global_flow.h5");
	//Load text
	LangProcessing<float> lp;
	lp.LoadDataFromFile();

	double lantime = 0, vistime = 0;
	for (uint32_t count = 0; count < 100; ++count)
	{
		clock_t start = clock();
		LanguageFeature<float> lf = lp.ExtractNextLanuguageData();
		clock_t end = clock();
		lantime += (double)(end - start) / CLOCKS_PER_SEC;
		cout << "Language Processing time" << (double)(end - start) / CLOCKS_PER_SEC << endl;

		start = clock();
		VisualFeature<float> fp1 = vp.ExtractNextVisualFeature();
		end = clock();
		vistime += (double)(end - start) / CLOCKS_PER_SEC;
		cout << "Visual Processing time" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	}
	cout << "average language time" << lantime / 100 << endl;
	cout << "average visual time" << vistime / 100 << endl;
}

int main()
{
	//evaluateMomentsRecall();
	evaluateVideoRecall();
	//evaluateDataProcessing();
 	return 0;
}

