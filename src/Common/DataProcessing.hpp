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


#endif