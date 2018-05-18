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

using caffe::Blob;
using caffe::Net;
using caffe::Caffe;
int main()
{
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
	
	return 0;
}

