#include <stdlib.h>
#include <stdio.h>
#include "filereadstream.h"
#include "document.h"
#include <string>  
using namespace rapidjson;
using namespace std;
#include <H5Cpp.h>
#include "DataProcessing.hpp"

#include "caffe.hpp"
#include "caffe/caffe.hpp"
#include "caffe/util/signal_handler.h"
#include "caffe/common.hpp"
#include "caffe/layers/dummy_data_layer.hpp"
#include "caffe/layers/concat_layer.hpp"
#include "caffe/layers/inner_product_layer.hpp"
#include "caffe/layers/relu_layer.hpp"
#include "caffe/layers/dropout_layer.hpp"
#include "caffe/layers/lstm_layer.hpp"
#include "caffe/layers/input_layer.hpp"
#include "caffe/layers/split_layer.hpp"
#include "caffe/layers/scale_layer.hpp"
#include "caffe/layers/slice_layer.hpp"
#include "caffe/layers/silence_layer.hpp"
#include "caffe/layers/reshape_layer.hpp"
#include "caffe/layers/power_layer.hpp"
#include "caffe/layers/eltwise_layer.hpp"
#include "caffe/layers/reduction_layer.hpp"

namespace caffe
{
	extern INSTANTIATE_CLASS(DummyDataLayer);
	extern INSTANTIATE_CLASS(ConcatLayer);
	extern INSTANTIATE_CLASS(InnerProductLayer);
	extern INSTANTIATE_CLASS(ReLULayer);
	REGISTER_LAYER_CLASS(ReLU);
	extern INSTANTIATE_CLASS(DropoutLayer);
	extern INSTANTIATE_CLASS(LSTMLayer);
	extern INSTANTIATE_CLASS(LSTMUnitLayer);
	extern INSTANTIATE_CLASS(InputLayer);
	extern INSTANTIATE_CLASS(SplitLayer);
	extern INSTANTIATE_CLASS(ScaleLayer);
	extern INSTANTIATE_CLASS(SliceLayer);
	extern INSTANTIATE_CLASS(SilenceLayer);
	extern INSTANTIATE_CLASS(ReshapeLayer);
	extern INSTANTIATE_CLASS(PowerLayer);
	extern INSTANTIATE_CLASS(EltwiseLayer);
	extern INSTANTIATE_CLASS(ReductionLayer);
}



using caffe::Blob;
using caffe::Net;
using caffe::Caffe;
int main()
{
	Caffe::set_mode(Caffe::CPU);
	string networkfile = "C:/Work/Project/VideoContentsRetrieval/didemo/LocalizingMoments-master/prototxts/deploy_clip_retrieval_rgb_iccv.prototxt";
	string weights = "C:/Work/Project/VideoContentsRetrieval/didemo/LocalizingMoments-master/snapshots/rgb_iccv_release_feature_process_iter_30000.caffemodel";
	Net<float> caffe_net(networkfile, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(weights);


	VisualProcessing<float> vp;
	vp.LoadDataFromFile();
	VisualFeature<float> fp1 = vp.ExtractNextVisualFeature();

	boost::shared_ptr <Blob< float >> pBlobData = caffe_net.blob_by_name("image_data");
	pBlobData->set_cpu_data(fp1.pData_feature);
	boost::shared_ptr <Blob< float >> pBlobLoc = caffe_net.blob_by_name("loc_data");
	pBlobLoc->set_cpu_data(fp1.pData_loc);


	LangProcessing<float> lp;
	lp.LoadDataFromFile();
	LanguageFeature<float> lf = lp.ExtractNextLanuguageData();

	boost::shared_ptr <Blob< float >> pBlobLData = caffe_net.blob_by_name("test_data");
	pBlobLData->set_cpu_data(lf.pData_vec);
	boost::shared_ptr <Blob< float >> pBlobLcont = caffe_net.blob_by_name("cont_data");
	pBlobLcont->set_cpu_data(lf.pData_cont);
	/*
	VisualProcessing vp;
	vp.LoadDataFromFile();
	VisualFeature fp1 = vp.ExtractNextVisualFeature();
	vp.ProcessVisual(fp1);
	
	LangProcessing lp;
	lp.LoadDataFromFile();
	LanguageFeature lf = lp.ExtractNextLanuguageData();
	VisualProcessing vp;
	vp.LoadDataFromFile();
	VisualFeature fp1 = vp.ExtractNextVisualFeature();
	//vp.ProcessVisual(fp1);
	VisualFeature fp2 = vp.ExtractNextVisualFeature();
	*/

	return 0;
}

