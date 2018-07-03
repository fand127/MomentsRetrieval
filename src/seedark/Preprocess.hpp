#pragma once
#ifndef _PREPROCESS_HPP_
#define _PREPROCESS_HPP_

#include "Common/DataProcessing.hpp"
#include "Common/DateType.hpp"

class PreprocessPara
{
public:
	bool use_auto_wb;
	bool use_default_substract_blk_level;
	bool tiff_out;
	PreprocessPara() {
		use_auto_wb = true;
		use_default_substract_blk_level = false;
		tiff_out = false;
	}
};

template <class dType>
class Preprocess :public DataProcessing {
public:
	Preprocess() {};
	virtual ~Preprocess() {
		RawProcessor.recycle(); delete pRaw; delete pImg;
	};
	ftensor*pRaw;
	ftensor*pImg;
	LibRaw RawProcessor;
public:
	int ReadRaw(const PreprocessPara&, string file = "../../../data/seedark/20018_00_0.1s.RAF");
	
	inline void raw2img(ftensor &raw, ftensor &img, uint32_t input_height, uint32_t input_width,
		uint32_t starti, uint32_t stepi, uint32_t startj, uint32_t stepj, uint32_t dim3,
		uint32_t startm, uint32_t stepm, uint32_t startn, uint32_t stepn)
	{
		for (uint32_t ini = starti, rawi = startm; ini < input_height; ini += stepi, rawi += stepm)
			for (uint32_t inj = startj, rawj = startn; inj < input_width; inj += stepj, rawj += stepn)
				img(ini, inj, dim3) = raw(rawi, rawj);
	}

	int Fujipreprocess();
	int Sonypreprocess();
};

template <class dType>
int Preprocess<dType>::Fujipreprocess()
{
	int ret = 0;
	uint32_t raw_width  = RawProcessor.imgdata.rawdata.sizes.raw_width;
	uint32_t raw_height = RawProcessor.imgdata.rawdata.sizes.raw_height;
	
	uint32_t input_width  = raw_width  / 6 * 2;
	uint32_t input_height = raw_height / 6 * 2;

	pRaw = new ftensor(raw_height, raw_width);
	pImg = new ftensor(input_height, input_width, 9);

	for (uint32_t hindx = 0; hindx < raw_height; ++hindx)
		for (uint32_t windx = 0; windx < raw_width; ++windx)
			(*pRaw)(hindx,windx) = fmaxf(0, RawProcessor.imgdata.rawdata.raw_image[hindx*raw_width + windx] - 1024) / (16383 - 1024);
	
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 0, 2, 0, 0, 6, 0, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 1, 2, 0, 0, 6, 4, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 0, 2, 0, 3, 6, 1, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 1, 2, 0, 3, 6, 3, 6);

	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 0, 2, 1, 0, 6, 2, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 1, 2, 1, 0, 6, 5, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 0, 2, 1, 3, 6, 2, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 1, 2, 1, 3, 6, 5, 6);

	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 0, 2, 2, 0, 6, 1, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 1, 2, 2, 0, 6, 3, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 0, 2, 2, 3, 6, 0, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 1, 2, 2, 3, 6, 4, 6);
	///
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 0, 2, 3, 1, 6, 2, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 1, 2, 3, 2, 6, 5, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 0, 2, 3, 5, 6, 2, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 1, 2, 3, 4, 6, 5, 6);

	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 0, 2, 4, 2, 6, 2, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 2, 1, 2, 4, 1, 6, 5, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 0, 2, 4, 4, 6, 2, 6);
	raw2img((*pRaw), (*pImg), input_height, input_width, 1, 2, 1, 2, 4, 5, 6, 5, 6);

	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 1, 0, 1, 5, 1, 3, 0, 3);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 1, 0, 1, 6, 1, 3, 1, 3);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 1, 0, 1, 7, 2, 3, 0, 3);
	raw2img((*pRaw), (*pImg), input_height, input_width, 0, 1, 0, 1, 8, 2, 3, 1, 3);
	return ret;
}

template <class dType>
int Preprocess<dType>::ReadRaw(const PreprocessPara& para, string file)
{
	_FUNCTION_E_;
	int ret = 0;
	RawProcessor.recycle();
	if (ret = RawProcessor.open_file(file.c_str()) != LIBRAW_SUCCESS)
	{
		RawProcessor.recycle();
		return -1;	
	}
	
	if (para.use_auto_wb)
	{
		RawProcessor.imgdata.params.use_auto_wb = 1;
	}

	if (para.use_default_substract_blk_level)
		if (ret = RawProcessor.subtract_black() != LIBRAW_SUCCESS)
		{
			RawProcessor.recycle();
			return -1;
		}
		
	if (ret = RawProcessor.unpack() != LIBRAW_SUCCESS)
	{
		RawProcessor.recycle();
		return -1;
	}

	if (ret = RawProcessor.dcraw_process() != LIBRAW_SUCCESS)
	{
		RawProcessor.recycle();
		return -1;
	}

	if (para.tiff_out)
		if (ret = RawProcessor.dcraw_ppm_tiff_writer("../../../data/seedark/test_6000.tiff") != LIBRAW_SUCCESS)
		{
			RawProcessor.recycle();
			return -1;
		}

	//RawProcessor.recycle();
	_FUNCTION_X_;
	return ret;
}

#endif