#include "DataProcessing.hpp"
#include "Utils.hpp"

void DataProcessing::loadh5file(string filename)
{
	_FUNCTION_E_;
	_FUNCTION_X_;
}

void DataProcessing::readjsonfile(string filename)
{
	_FUNCTION_E_;
	if (data_loaded)
		return;

	FILE *fp = fopen(filename.c_str(), "rb");
	char *readbuffer = new char[1024 * 1024 * 50];

	rapidjson::FileReadStream Inputstream(fp, readbuffer, sizeof(readbuffer));
	doc.ParseStream(Inputstream);
	rapidjson::Value val;
	bool result = false;
	
	if (!doc.IsArray() || !doc[0].IsObject())
	{
		fclose(fp);
		delete readbuffer;
		return;
	}
	fclose(fp);
	data_loaded = true;
	delete readbuffer;
	_FUNCTION_X_;
}
