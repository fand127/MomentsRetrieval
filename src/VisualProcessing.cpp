#include "DataProcessing.hpp"


template <class dType>
void VisualProcessing<dType>::setCombination(VisualFeature<dType> &feature)
{
	uint32_t num_seg = feature.mtotal_seg;
	for(uint32_t start = 0; start < num_seg; ++start)
		for (uint32_t end = start; end < num_seg; ++end)
			feature.possible_segment.push_back(make_pair(start, end));

	if (feature.pData_feature == NULL)
	{
		feature.pData_feature = new dType[feature.mbatch_size * feature.mdims[0] *
			feature.possible_segment.size() * feature.mdims[2] * 2];
		memset(feature.pData_feature, 0, sizeof(dType)*feature.mbatch_size * feature.mdims[0] *
			feature.possible_segment.size() * feature.mdims[2] * 2);
	}
	if (feature.pData_loc == NULL)
	{
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
			visual_data_vec.insert((doc[i]["video"].GetString()));
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
	uint32_t feature_date_base_, feature_date_base_local;
	double val_global = 0,norm_global_sum = 0;
	for (index = 0; index < total_seg; ++index)
	{
		feature_date_base_      = index * feature.mdims[2] * 2; //global
		feature_date_base_local = feature_date_base_ + feature.mdims[2]; //local
		double norm_sum = 0;
		start = feature.possible_segment.at(index).first;
		end   = feature.possible_segment.at(index).second;
		for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
		{
			double val = 0;
			for (uint32_t _index = start; _index < end + 1; ++_index)
			{
				val += feature.pTemp_feature[data_index * feature.mdims[1] + _index];
			}
			norm_sum += val * val;

			feature.pData_feature[feature_date_base_local + data_index] = (dType)(val / (end + 1 - start));
			
			if (index == 0) //calculate global
			{
				for (uint32_t _index = 0; _index < feature.mtotal_seg; ++_index)
				{
					val_global += feature.pTemp_feature[data_index * feature.mdims[1] + _index];
				}
				norm_global_sum += val_global*val_global;
				feature.pData_feature[feature_date_base_ + data_index] = (dType)(val_global / feature.mtotal_seg);
			}			
		}

		norm_sum = sqrt(norm_sum);
		for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
			feature.pData_feature[feature_date_base_local + data_index] /= norm_sum + 0.00001;

		if(index ==0 ) //global
		{
			norm_global_sum = sqrt(norm_global_sum);
			for (uint32_t data_index = 0; data_index < feature.mdims[2]; ++data_index)
				feature.pData_feature[feature_date_base_ + data_index] /= norm_global_sum + 0.00001;
		}
		feature.pData_loc[index * 2]     = (class dType)start / feature.mtotal_seg;
		feature.pData_loc[index * 2 + 1] = (class dType)end / feature.mtotal_seg;
	}

	_FUNCTION_X_;
}

template <class dType>
void VisualProcessing<dType>::LoadDataFromFile(string datafile, string featurefile)
{
	_FUNCTION_E_;
	file  = inith5file(featurefile);
	readjsonfile(datafile);
	setCombination(feature);
	preProcessVisual();
	_FUNCTION_X_;
}
template <class dType>
VisualFeature<dType> VisualProcessing<dType>::ExtractNextVisualFeature()
{
	_FUNCTION_E_;
	static int batch_count = 0;
	string dataset_name;
	H5::DataSet dataset;
	uint32_t batch_data_index;
	uint32_t feature_indx_offset;
	feature.video_id.clear();

	for (batch_data_index = batch_count * batch_size, feature_indx_offset = 0;
		(feature_indx_offset < batch_size) && (batch_data_index < file.getNumObjs()); )
	{
		dataset_name = file.getObjnameByIdx(batch_data_index);
		if (visual_data_vec.find(dataset_name) != visual_data_vec.end())
		{
			uint32_t data_loc_offset = feature_indx_offset * feature.mdims[0] * feature.mdims[1] * feature.mdims[2];
			dataset = file.openDataSet(dataset_name.c_str());
			if(sizeof(dType) == sizeof(double))
				dataset.read(feature.pTemp_feature + data_loc_offset, H5::PredType::NATIVE_DOUBLE);
			else if (sizeof(dType) == sizeof(float))
				dataset.read(feature.pTemp_feature + data_loc_offset, H5::PredType::NATIVE_FLOAT);
			feature.video_id.push_back(dataset_name);
			++feature_indx_offset;
		}
		++batch_data_index;
	}
	if (batch_data_index == file.getNumObjs())
		batch_count = 0;
	else
		batch_count++;

	ProcessVisual();
	_FUNCTION_X_;
	return feature;
}
