#include "DataProcessing.hpp"
#include "Utils.hpp"

template <class dType>
vector<string> LangProcessing<dType>::sentencetoword(string sentence)
{
	//_FUNCTION_E_;
	vector<string> words;
	string word;
	char *data = new char[word_size];
	sentence = CommonRepresentation(sentence);
	istringstream linestream(sentence);
	
	while(linestream.getline(data, word_size, ' '))
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
	for (int i = 0; i < doc.Size(); ++i)
	{
		if (doc[i].IsObject())
		{
			description_words_vec.push_back(sentencetoword(doc[i]["description"].GetString()));
		}
	}
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
		if(word != "<unk>")
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
	string sentence , word;
	
	
	const int glove_size = 300;
	vector<double> vec;
	float val;
	char *data = new char[word_size];
	inFile.open(glove_file.c_str());
	if (!inFile) {
		cerr << "Unable to open file " << glove_file ;
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
LanguageFeature<dType> LangProcessing<dType>::ExtractNextLanuguageData()
{
	_FUNCTION_E_;
	static int batch_count = 0;

	Data.deinit();
	Data.init(glove_length, loc_size, sentence_length, batch_size);
	
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

	for (batch_sendata_index = 0, sendata_index = index_count_begin; batch_sendata_index < batch_size; ++batch_sendata_index, ++sendata_index)
	{
		//Get sentence index not exceedding end of total description data 
		if (sendata_index == description_words_vec.size())
			break;
		//Get sentence vector string		
		uint32_t word_con_index_start  = loc_size * sentence_length * batch_sendata_index;
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
				
				DPRINT("\n %s ",val->first.c_str());
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
	}

	//Last sentence is reached
	if (description_words_vec.size() == sendata_index)
		batch_count = 0;
	else
		batch_count++;
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
	_FUNCTION_X_;
}