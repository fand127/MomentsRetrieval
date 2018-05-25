#ifndef _DATA_PROCESSING_
#define _DATA_PROCESSING_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include <string> 
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "Utils.hpp"

#include <H5Cpp.h>

#include "filereadstream.h"
#include "document.h"
using namespace rapidjson;
using namespace std;

#include "Utils.hpp"


class DataProcessing
{
	
public:
	DataProcessing() { data_loaded = false; };
	virtual ~DataProcessing() {};
protected:
	int batch_size = 0;
	bool data_loaded;
	rapidjson::Document doc;
public:
	void readjsonfile(std::string filename = "../../data/val_data.json");
	virtual void loadh5file(string filename);
};

template <class dType>
class LanguageFeature{
	uint32_t mglove_length;
	uint32_t mloc_size;
	uint32_t msentence_length;
	uint32_t mbatch_size;	
	uint32_t vec_total_size;
	uint32_t con_total_size;
	
public:
	bool is_new;
	uint32_t mloaded;
	uint32_t mtotal_seg;
	uint32_t mstart_seg;
	uint32_t mend_seg;
	vector<string> video_id;
	dType* pData_vec;
	dType* pData_cont;
public:
	LanguageFeature() { init(); };
	~LanguageFeature() {  };
	void init() {
		pData_vec        = NULL;
		pData_cont       = NULL;
		mglove_length    = 0;
		mloc_size        = 0; 
		msentence_length = 0; 
		mbatch_size      = 0;
		vec_total_size   = 0;
		con_total_size   = 0;
	};
	void init(uint32_t glove_length,
		uint32_t loc_size,
		uint32_t sent_length,
		uint32_t batch_size) {
		mglove_length    = glove_length;
		mloc_size        = loc_size; //default 21 
		msentence_length = sent_length;
		mbatch_size      = batch_size;
		con_total_size   = batch_size * sent_length * loc_size;
		vec_total_size   = con_total_size * glove_length;
		pData_vec  = NULL; 
		pData_cont = NULL; 
		pData_vec = new dType[glove_length * loc_size * sent_length * batch_size];
		pData_cont = new dType[loc_size * sent_length * batch_size];
		
		memset(pData_vec, 0, sizeof(dType) * glove_length * loc_size * sent_length * batch_size);
		memset(pData_cont, 0, sizeof(dType) * loc_size * sent_length * batch_size);
	};
	void deinit() {
		if (pData_vec) { delete pData_vec; pData_vec = NULL; }
		if (pData_cont) { delete pData_cont; pData_cont = NULL; }
	};
public:
	void dumpdata()
	{
		ofstream of("../../bin/pVec.txt");
		uint32_t count = 0;
		while (count < vec_total_size)
		{
			of << pData_vec[count] << endl;
			count++;
		}
		of.close();
		ofstream of2("../../bin/pCon.txt");
		count = 0;
		while (count < con_total_size)
		{
			of2 << pData_cont[count] << endl;
			count++;
		}
		of2.close();
	}
};

template <class dType>
class VisualFeature {
public:
	bool is_new;
	uint32_t mloaded;
	uint32_t mbatch_size;
	uint32_t mdims[3];
	uint32_t mtotal_seg;
public:
	dType* pData_feature;
	uint32_t feature_size;
	dType* pData_loc;
	uint32_t loc_size;
	dType* pTemp_feature;
	vector<string> video_id;
	vector<pair<uint32_t, uint32_t>> possible_segment;
public: 
	VisualFeature() { init(); };
	~VisualFeature() {};
public:
	void init() {
		pData_feature = NULL;
		pData_loc     = NULL;
		pTemp_feature = NULL;
		mbatch_size   = 0;
		memset(mdims, 0, sizeof(uint32_t) * 3);
	};
	void init(uint32_t batch_size, uint32_t channels,
		uint32_t height, uint32_t width)
	{
		mbatch_size = batch_size;
		mdims[0] = channels;
		mdims[1] = height;
		mdims[2] = width;
		mtotal_seg = height;
		video_id.clear();
		pTemp_feature = new dType[batch_size * channels * height * width];
		memset(pTemp_feature, 0, sizeof(dType) * batch_size * channels* height * width);
	}
	void deinit() {
		video_id.clear();
		if (pData_feature) { delete pData_feature; pData_feature = NULL; }
		if (pData_loc) { delete pData_loc; pData_loc = NULL; }
		if (pTemp_feature) { delete pTemp_feature; pTemp_feature = NULL; }
	};
};

#endif