#ifndef _LANGUAGE_PROCESS_
#define _LANGUAGE_PROCESS_
#include "DataProcessing.hpp"

template <class dType>
class LanguageFeature {
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
		pData_vec = NULL;
		pData_cont = NULL;
		mglove_length = 0;
		mloc_size = 0;
		msentence_length = 0;
		mbatch_size = 0;
		vec_total_size = 0;
		con_total_size = 0;
	};
	void init(uint32_t glove_length,
		uint32_t loc_size,
		uint32_t sent_length,
		uint32_t batch_size) {
		mglove_length = glove_length;
		mloc_size = loc_size; //default 21 
		msentence_length = sent_length;
		mbatch_size = batch_size;
		con_total_size = batch_size * sent_length * loc_size;
		vec_total_size = con_total_size * glove_length;
		pData_vec = NULL;
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
	vector<string> video_name_vec;
	vector<pair<uint32_t, uint32_t>> seg_vec;
	vector<string> sentencetoword(string);
	void preProcessLanguage();
	//Load model complete vocabulary
	void LoadVocabfile(string vocab_file = "../../data/didemo/vocab_glove_complete.txt");
	void LoadGloveEmbedding(string glove_file = "../../data/didemo/glove.6B.300d_test.txt");
	LanguageFeature<dType> ExtractNextLanuguageData(bool random = false);
	
	template <class K>
	void LoadData(vector<K>,
		string vocab_file = "../../data/didemo/vocab_glove_complete.txt",
		string embedding_file = "../../data/didemo/glove.6B.300d_test.txt");

	void LoadDataFromFile(string datafile = "../../data/didemo/val_data.json",
		string vocab_file = "../../data/didemo/vocab_glove_complete.txt",
		string embedding_file = "../../data/didemo/glove.6B.300d_test.txt");

	//correct moment times
	void GetTimes();
};

template <class dType>
vector<string> LangProcessing<dType>::sentencetoword(string sentence)
{
	//_FUNCTION_E_;
	vector<string> words;
	string word;
	char *data = new char[word_size];
	sentence = CommonRepresentation(sentence);
	istringstream linestream(sentence);

	while (linestream.getline(data, word_size, ' '))
	{
		word.assign(data);
		words.push_back(word);
	}
	//_FUNCTION_X_;
	return words;
}

template <class dType>
void LangProcessing<dType>::preProcessLanguage()
{
	_FUNCTION_E_;
	//Careful, Value assignment is one time move 
	for (int i = 0; i < doc.Size(); ++i)
	{
		if (doc[i].IsObject())
		{
			//in the order of data
			description_words_vec.push_back(sentencetoword(doc[i]["description"].GetString()));
			video_name_vec.push_back(doc[i]["video"].GetString());
		}
	}

	this->Data.mtotal_seg = doc[0]["num_segments"].GetInt();
	_FUNCTION_X_;
}

template <class dType>
void LangProcessing<dType>::LoadVocabfile(std::string vocab_file)
{
	_FUNCTION_E_;
	ifstream inFile;
	string word;
	vector<double> vec_holder;

	inFile.open(vocab_file.c_str());
	if (!inFile) {
		cerr << "Unable to open file " << vocab_file;
		exit(1);
	}
	//clear vacabulary if not empty 
	if (vocab_vec.size() > 0)
		vocab_vec.clear();

	while (getline(inFile, word))
	{
		if (word != "<unk>")
			vocab_vec.insert(make_pair(word, vec_holder));
	}
	inFile.close();
	_FUNCTION_X_;
}

template <class dType>
void LangProcessing<dType>::LoadGloveEmbedding(string glove_file)
{
	_FUNCTION_E_;
	ifstream inFile;
	string sentence, word;

	const int glove_size = 300;
	vector<double> vec;
	float val;
	char *data = new char[word_size];
	inFile.open(glove_file.c_str());
	if (!inFile) {
		cerr << "Unable to open file " << glove_file;
		exit(1);
	}
	istringstream linestream;

#ifdef didemo_temp
	ofstream oFile;
	oFile.open("../../data/glove.6B.300d_test.txt");
#endif

	while (getline(inFile, sentence))
	{

		linestream.clear();
		linestream.str(sentence);
		linestream.getline(data, word_size, ' ');
		word.assign(data);
		auto val = vocab_vec.find(word);
		if (val != vocab_vec.end())
		{
#ifdef didemo_temp
			oFile << sentence << endl;
#endif
			vec.clear();
			while (linestream.getline(data, glove_size, ' '))
			{
				vec.push_back((double)atof(data));
			}
			val->second = vec;
		}
	}
	inFile.close();
	delete data;

#ifdef didemo_temp
	oFile.close();
#endif
	_FUNCTION_X_;
}

template <class dType>
LanguageFeature<dType> LangProcessing<dType>::ExtractNextLanuguageData(bool random)
{
	_FUNCTION_E_;
	static int batch_count = 0;

	Data.deinit();
	Data.init(glove_length, loc_size, sentence_length, batch_size);
	Data.is_new  = false;
	Data.mloaded = 0;

	//current word index with float data memory
	uint32_t word_data_index = 0;
	//current data index of total description sentence data
	uint32_t sendata_index = 0;
	//current data index relative to start of batch
	uint32_t batch_sendata_index = 0;
	//total batch number for current batch size
	uint32_t total_batch_num = description_words_vec.size() / batch_size;
	//Current batch start data index of total description sentence data 
	uint32_t index_count_begin = batch_count * batch_size;
	//Current batch end data index of total description sentence data
	uint32_t index_count_end = index_count_begin + batch_size;
	//Rotation Index 1-21
	uint32_t loc_index = 0;
	//Cont_data index
	uint32_t word_cont_index = 0;
	//Clear Video name
	Data.video_id.clear();

	if (random)
		batch_count = rand() % description_words_vec.size();

	for (batch_sendata_index = 0, sendata_index = index_count_begin; batch_sendata_index < batch_size; ++batch_sendata_index, ++sendata_index)
	{
		//Get sentence index not exceedding end of total description data 
		if (sendata_index == description_words_vec.size())
			break;
		//Get sentence vector string		
		uint32_t word_con_index_start = loc_size * sentence_length * batch_sendata_index;
		uint32_t word_data_index_start = glove_length * word_con_index_start;
		vector<string> &description_words = description_words_vec.at(sendata_index);
		for (uint32_t word_indx = 0; word_indx < description_words.size(); ++word_indx)
		{
			//Actual Vector placement is end aligned 
			uint32_t word_placement_indx = sentence_length - description_words.size() + word_indx;
			//word cont data start index 
			word_cont_index = word_con_index_start + word_placement_indx * loc_size;
			//word float data start index 
			word_data_index = word_data_index_start + word_placement_indx * loc_size * glove_length;
			//Find each word embedding vector in the sentence
			auto val = vocab_vec.find(description_words.at(word_indx));
			if (val != vocab_vec.end())
			{
				//If word embedding vector is found, locate data
				DPRINT("\n %s ", val->first.c_str());
				for (uint32_t vec_index = 0; vec_index < glove_length; ++vec_index)
				{
					loc_index = 0;
					while (loc_index < loc_size)
					{
						Data.pData_vec[word_data_index + loc_index * glove_length + vec_index] = (dType)(val->second.at(vec_index));
						loc_index++;
					}
					DPRINT("%f ", val->second.at(vec_index));
				}

				loc_index = 0;
				while (loc_index < loc_size)
				{
					Data.pData_cont[word_cont_index + loc_index] = 1;
					loc_index++;
				}				
			}
		}
		Data.mstart_seg = seg_vec.at(sendata_index).first;
		Data.mend_seg   = seg_vec.at(sendata_index).second;
		Data.video_id.push_back(video_name_vec.at(sendata_index));
		Data.mloaded++;
	}

	//Last sentence is reached
	if (description_words_vec.size() == sendata_index)
		batch_count = 0;
	else
	{
		batch_count++;
		Data.is_new = true;
	}	
	_FUNCTION_X_;
	return Data;
}

template <class dType>
void LangProcessing<dType>::LoadDataFromFile(string datafile,
	string vocab_file,
	string embedding_file)
{
	_FUNCTION_E_;
	LoadVocabfile(vocab_file);
	LoadGloveEmbedding(embedding_file);
	readjsonfile(datafile);
	preProcessLanguage();
	GetTimes();
	_FUNCTION_X_;
}

template <class dType>
void LangProcessing<dType>::GetTimes()
{	
	for (int i = 0; i < doc.Size(); ++i)
	{
		if (doc[i].IsObject())
		{		
			uint32_t seg_count = 0;
			float seg_start = 0, seg_end = 0;
			Value timearray = doc[i]["times"].GetArray();
			for (uint32_t j = 0; j< timearray.Size(); ++j)
			{
				const Value& val = timearray[j];		
				seg_start += val[0].GetInt();
				seg_end   += val[1].GetInt();
				seg_count++;
			}
			if(seg_count != 0)
			{
				seg_start /= seg_count;
				seg_end /= seg_count;
			}
		
			seg_vec.push_back(make_pair(uint32_t(seg_start + 0.5f), uint32_t(seg_end + 0.5f)));
		}
	}
}

#endif