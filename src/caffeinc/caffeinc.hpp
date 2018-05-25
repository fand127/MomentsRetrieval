#ifndef _CAFFE_INC_HPP_
#define _CAFFE_INC_HPP_

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
#ifdef _WIN32
	REGISTER_LAYER_CLASS(ReLU);
#endif
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

#endif