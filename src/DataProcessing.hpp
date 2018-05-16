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
		mloc_size        = loc_size;
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
class LangProcessing: public DataProcessing
{
	int word_size         = 256;
    int sentence_length   = 50;
	int loc_size          = 21;
	int glove_length      = 300;
	LanguageFeature<dType> Data;
protected:
	static int isNotAlphaNum(char c) { return !isalnum(c); }
	string CommonRepresentation(string str) {
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		replace_if(str.begin(), str.end(), isNotAlphaNum, ' ');
		return str;
	};
public:
	LangProcessing() {
		batch_size = 1;
	};
	virtual ~LangProcessing() { Data.deinit(); };

	enum class LangType {
		zero_lanuage = 0,
		recurrent_embedding
	};
	unordered_map<string, vector<double>> vocab_vec;
	unordered_map<string, vector<double>> glove_vec;
	vector<vector<string>> description_words_vec;	

	vector<string> sentencetoword(string);
	void preProcessLanguage();
	//Load model complete vocabulary
	void LoadVocabfile(string vocab_file = "../../data/vocab_glove_complete.txt");
	void LoadGloveEmbedding(string glove_file = "../../data/glove.6B.300d_test.txt");
	LanguageFeature<dType> ExtractNextLanuguageData();
	
	template <class K>
	void LoadData(vector<K>,
		string vocab_file = "../../data/vocab_glove_complete.txt",
		string embedding_file = "../../data/glove.6B.300d_test.txt");

	void LoadDataFromFile(string datafile = "../../data/val_data.json",
		string vocab_file = "../../data/vocab_glove_complete.txt",
		string embedding_file = "../../data/glove.6B.300d_test.txt");
};

template <class dType>
class VisualFeature {
public:
	uint32_t mbatch_size;
	uint32_t mdims[3];
	uint32_t mtotal_seg;
public:
	dType* pData_feature;
	dType* pData_loc;
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

template <class dType>
class VisualProcessing:public DataProcessing {
	unordered_map<string, vector<dType>> visual_feature;
public:
	VisualProcessing() { batch_size = 1; };
	virtual ~VisualProcessing() { closeh5file(file);};
protected: 
	H5::H5File file;
	VisualFeature<dType> feature;
	unordered_set<string> visual_data_vec;
public:
	void setCombination(VisualFeature<dType> &);
	H5::H5File inith5file(string filename);
	void closeh5file(H5::H5File file) { file.close(); };
	void preProcessVisual();
	void ProcessVisual();
	//void ProcessVisual();
	void LoadDataFromFile(string datafile = "../../data/val_data.json", string featurefile = "../../data/average_fc7.h5");
		//,string flowfile = "../../data/average_global_flow.h5");
	VisualFeature<dType> ExtractNextVisualFeature();
};