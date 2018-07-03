#ifndef _VISUAL_PROCESS_
#define _VISUAL_PROCESS_
#include "DataProcessing.hpp"


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
		pData_loc = NULL;
		pTemp_feature = NULL;
		mbatch_size = 0;
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
class VisualProcessing :public DataProcessing {
	unordered_map<string, vector<dType>> visual_feature;
public:
	VisualProcessing() { batch_size = 1; };
	virtual ~VisualProcessing() { closeh5file(file); };
protected:
	H5::H5File file;
	VisualFeature<dType> feature;
	unordered_set<string> visual_feature_name_vec;
	vector<string> visual_data_vec;
public:
	void setCombination(VisualFeature<dType> &);
	H5::H5File inith5file(string filename);
	void closeh5file(H5::H5File file) { file.close(); };
	void preProcessVisual();
	void ProcessVisual();
	//void ProcessVisual();
	void LoadDataFromFile(string datafile = "../../data/didemo/val_data.json", string featurefile = "../../data/didemo/average_fc7.h5");
	VisualFeature<dType> ExtractNextVisualFeature();
};


template <class dType>
void VisualProcessing<dType>::setCombination(VisualFeature<dType> &feature)
{
	uint32_t num_seg = feature.mtotal_seg;
	for (uint32_t start = 0; start < num_seg; ++start)
		feature.possible_segment.push_back(make_pair(start, start));
	for (uint32_t start = 0; start < num_seg; ++start)
		for (uint32_t end = start + 1; end < num_seg; ++end)
			feature.possible_segment.push_back(make_pair(start, end));

	if (feature.pData_feature == NULL)
	{
		feature.feature_size = feature.mbatch_size * feature.mdims[0] * feature.possible_segment.size() * feature.mdims[2] * 2;
		feature.pData_feature = new dType[feature.mbatch_size * feature.mdims[0] *
			feature.possible_segment.size() * feature.mdims[2] * 2];
		memset(feature.pData_feature, 0, sizeof(dType)*feature.mbatch_size * feature.mdims[0] *
			feature.possible_segment.size() * feature.mdims[2] * 2);
	}
	if (feature.pData_loc == NULL)
	{
		feature.loc_size = feature.mbatch_size * feature.mdims[0] * feature.possible_segment.size() * 2;
		feature.pData_loc = new dType[feature.mbatch_size * feature.mdims[0] *
			feature.possible_segment.size() * 2];
		memset(feature.pData_loc, 0, sizeof(dType) * feature.mbatch_size * feature.mdims[0] *
			feature.possible_segment.size() * 2);
	}
}
template <class dType>
H5::H5File VisualProcessing<dType>::inith5file(string filename)
{
	H5::H5File file(filename.c_str(), H5F_ACC_RDONLY);
	string dataset_name = file.getObjnameByIdx(0);
	H5::DataSet dataset = file.openDataSet(dataset_name.c_str());
	H5::DataSpace dataspace = dataset.getSpace();
	hsize_t dims_out[2];
	dataspace.getSimpleExtentDims(dims_out, NULL);
	feature.init(batch_size, 1, dims_out[0], dims_out[1]);

	uint32_t file_size = file.getNumObjs();
	for (uint32_t i = 0; i < file_size; ++i)
	{
		visual_feature_name_vec.insert(file.getObjnameByIdx(i));
	}
	return file;
}

template <class dType>
void VisualProcessing<dType>::preProcessVisual()
{
	_FUNCTION_E_;
	for (int i = 0; i < doc.Size(); ++i)
	{
		if (doc[i].IsObject())
		{
			visual_data_vec.push_back((doc[i]["video"].GetString()));
		}
	}
	_FUNCTION_X_;
}
template <class dType>
void VisualProcessing<dType>::ProcessVisual()
{
	_FUNCTION_E_;
	uint32_t index = 0;
	uint32_t total_seg = feature.possible_segment.size();
	uint32_t start, end;
	uint32_t feature_data_base_, feature_data_base_local;


	double val_global = 0, norm_global_sum = 0;
	for (index = 0; index < total_seg; ++index)
	{
		feature_data_base_ = index * feature.mdims[2] * 2; //global
		feature_data_base_local = feature_data_base_ + feature.mdims[2]; //local
		double norm_sum = 0;
		start = feature.possible_segment.at(index).first;
		end = feature.possible_segment.at(index).second;
		for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
		{
			double val = 0;
			for (uint32_t _index = start; _index < end + 1; ++_index)
			{
				val += feature.pTemp_feature[_index * feature.mdims[2] + data_index];
			}
			val = (dType)(val / (end + 1 - start));
			norm_sum += val * val;

			feature.pData_feature[feature_data_base_local + data_index] = (dType)val;

			if (index == 0) //calculate global
			{
				val_global = 0;
				for (uint32_t _index = 0; _index < feature.mtotal_seg; ++_index)
				{
					val_global += feature.pTemp_feature[_index * feature.mdims[2] + data_index];
				}
				val_global /= feature.mtotal_seg;
				norm_global_sum += val_global*val_global;
				feature.pData_feature[feature_data_base_ + data_index] = val_global;
			}
		}

		norm_sum = sqrt(norm_sum);
		for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
			feature.pData_feature[feature_data_base_local + data_index] /= norm_sum + 0.00001;

		if (index == 0) //global
		{
			norm_global_sum = sqrt(norm_global_sum);
			for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
				feature.pData_feature[feature_data_base_ + data_index] /= norm_global_sum + 0.00001;//feature_data_base = 0
		}
		else
		{ 
			//copy global
			for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
				feature.pData_feature[feature_data_base_ + data_index] = feature.pData_feature[data_index];
		}
		feature.pData_loc[index * 2]     = (dType)start / feature.mtotal_seg;
		feature.pData_loc[index * 2 + 1] = (dType)end / feature.mtotal_seg;
	}

	_FUNCTION_X_;
}

template <class dType>
void VisualProcessing<dType>::LoadDataFromFile(string datafile, string featurefile)
{
	_FUNCTION_E_;
	file = inith5file(featurefile);
	readjsonfile(datafile);
	setCombination(feature);
	preProcessVisual();
	_FUNCTION_X_;
}
template <class dType>
VisualFeature<dType> VisualProcessing<dType>::ExtractNextVisualFeature()
{
	_FUNCTION_E_;
	static int batch_data_count = 0;
	string dataset_name;
	H5::DataSet dataset;
	uint32_t batch_data_index;
	uint32_t feature_indx_offset;
	feature.video_id.clear();
	feature.is_new  = false;
	feature.mloaded = 0;

	for (batch_data_index = batch_data_count, feature_indx_offset = 0; (feature_indx_offset < batch_size) &&
		(batch_data_index < visual_data_vec.size()); )
	{
		dataset_name = visual_data_vec.at(batch_data_index);
		if (visual_feature_name_vec.find(dataset_name) != visual_feature_name_vec.end())
		{
			uint32_t data_loc_offset = feature_indx_offset * feature.mdims[0] * feature.mdims[1] * feature.mdims[2];
			dataset = file.openDataSet(dataset_name.c_str());
			if (sizeof(dType) == sizeof(double))
				dataset.read(feature.pTemp_feature + data_loc_offset, H5::PredType::NATIVE_DOUBLE);
			else if (sizeof(dType) == sizeof(float))
				dataset.read(feature.pTemp_feature + data_loc_offset, H5::PredType::NATIVE_FLOAT);
			feature.video_id.push_back(dataset_name);
			++feature_indx_offset;
			feature.mloaded++;
		}
		else
		{
			cout << "Dataset not found " << dataset_name << endl;			
		}
		++batch_data_index;
	}

	if (batch_data_index == visual_data_vec.size())
		batch_data_count = 0;
	else
	{
		feature.is_new   = true;
		batch_data_count = batch_data_index;
	}
	ProcessVisual();
	_FUNCTION_X_;
	return feature;
}

#endif