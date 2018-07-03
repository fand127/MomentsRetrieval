#ifndef _DATATYPE_HPP_
#define _DATATYPE_HPP_

#include <stdint.h>

template <class dType>
class Otensor {
	bool extmem;
	bool extmempt;
public:
	dType *pData;
	dType *pData1;
	dType **pData2;
	dType ***pData3;
	dType ****pData4;
	uint32_t dims[4];
	uint32_t order = 0;
	uint32_t memsize = 0;
public:
	Otensor() 
	{ 
		extmem  = false;
		extmempt= false;
		order   = 0;
		memsize = 0;
		dims[0] = 0; 
		dims[1] = 0;
		dims[2] = 0;
		dims[3] = 0;
		pData  = NULL;
		pData1 = NULL;
		pData2 = NULL;
		pData3 = NULL;
		pData4 = NULL;
	};

	Otensor& operator = (Otensor &o)
	{
		if (&o == this) {
			return *this;
		}
		extmem   = true;
		extmempt = o.extmempt;
		pData  = o.pData;
		pData1 = o.pData1;
		pData2 = o.pData2;
		pData3 = o.pData3;
		pData4 = o.pData4;
		o.pData = NULL;
		o.pData1 = NULL;
		o.pData2 = NULL;
		o.pData3 = NULL;
		o.pData4 = NULL;
		dims[0] = o.dims[0];
		dims[1] = o.dims[1];
		dims[2] = o.dims[2];
		dims[3] = o.dims[3];
		order = o.order;
		memsize = o.memsize;
		return *this;
	}

	Otensor(Otensor&& o) {
		extmem   = true;
		extmempt = o.extmempt;
		pData  = o.pData;
		pData1 = o.pData1;
		pData2 = o.pData2;
		pData3 = o.pData3;
		pData4 = o.pData4;
		o.pData = NULL;
		o.pData1 = NULL;
		o.pData2 = NULL;
		o.pData3 = NULL;
		o.pData4 = NULL;
		dims[0] = o.dims[0];
		dims[1] = o.dims[1];
		dims[2] = o.dims[2];
		dims[3] = o.dims[3];
		order = o.order;
		memsize = o.memsize;
	};
	Otensor(uint32_t size1, dType *pdata = NULL);
	Otensor(uint32_t size1, uint32_t size2, dType *pdata = NULL);
	Otensor(uint32_t size1, uint32_t size2, uint32_t size3, dType *pdata = NULL);
	Otensor(uint32_t size1, uint32_t size2, uint32_t size3, uint32_t size4, dType *pdata = NULL);

	virtual ~Otensor()
	{
		if (!extmem && pData)			
		{
			delete[] pData;
		}
		pData = NULL;

		if (extmempt)
			return;

		if (0 == order)
		{
			return;
		}
		else if (1 == order && pData1)
			pData1 = NULL;
		else if (2 == order && pData2)
		{
			for (uint32_t i = 0; i < dims[2]; ++i)				
				pData2[i] = NULL;
			delete[]pData2;
			pData2 = NULL;
		}
		else if (3 == order && pData3)
		{
			for (uint32_t i = 0; i < dims[1]; ++i)
			{
				for (uint32_t j = 0; j < dims[2]; ++j)
					pData3[i][j] = NULL;
				delete[]pData3[i];
				pData3[i] = NULL;
			}
			delete[]pData3;
			pData3 = NULL;
		}
		else if (4 == order && pData4)
		{
			for (uint32_t o = 0; o < dims[0]; ++o)
			{
				for (uint32_t i = 0; i < dims[1]; ++i)
				{
					for (uint32_t j = 0; j < dims[2]; ++j)
						pData4[o][i][j] = NULL;
					delete[]pData4[o][i];
					pData4[o][i] = NULL;
				}
				delete[]pData4[o];
				pData4[o] = NULL;
			}
			delete[]pData4;
			pData4 = NULL;
		}
	}

	//Otensor<dType> Transpose(uint32_t, uint32_t, uint32_t, uint32_t);

	inline dType& operator()( uint32_t index1)
	{	
		if(order == 1)
			return pData1[index1];
		else return dType(data);
	}

	
	inline dType& operator()( uint32_t index1,  uint32_t index2) {
		if (order == 2)
			return pData2[index1][index2];
		else return dType(data);
	}

	
	inline dType& operator()( uint32_t index1,  uint32_t index2,  uint32_t index3)
	{
		if (order == 3)
			return pData3[index1][index2][index3];
		else return dType(data);
	}

	inline dType& operator()( uint32_t index1,  uint32_t index2,  uint32_t index3,  uint32_t index4)
	{
		if (order == 4)
			return pData4[index1][index2][index3][index4];
		else return dType(data);
	}	
};

template <class dType>
Otensor<dType>::Otensor(uint32_t size1, dType *pdata)
{
	order = 1;
	dims[0] = 1;
	dims[1] = 1;
	dims[2] = 1;
	dims[3] = size1;
	memsize = size1;
	if(pdata == NULL)
		pData = new dType[size1];
	else
	{
		pData = pdata;
		extmem = true;
	}
	pData1 = pData;

	extmempt = false;
}

template <class dType>
Otensor<dType>::Otensor(uint32_t size1, uint32_t size2, dType *pdata)
{
	order = 2;
	dims[0] = 1;
	dims[1] = 1;
	dims[2] = size1;
	dims[3] = size2;
	memsize = size1 * size2;
	if (pdata == NULL)
		pData = new dType[size1 * size2];
	else
	{
		pData = pdata;
		extmem = true;
	}
	pData2 = new dType*[size1];
	for (uint32_t indx = 0; indx < size1; ++indx)
		pData2[indx] = pData + size2 * indx;

	extmempt = false;
}

template <class dType>
Otensor<dType>::Otensor(uint32_t size1, uint32_t size2, uint32_t size3, dType *pdata)
{
	order   = 3;
	dims[0] = 1;
	dims[1] = size1;
	dims[2] = size2;
	dims[3] = size3;
	memsize = size1 * size2 * size3;
	if (pdata == NULL)
		pData = new dType[size1 * size2 * size3];
	else
	{
		pData = pdata;
		extmem = true;
	}

	pData3 = new dType**[size1];
	for (uint32_t indx1 = 0; indx1 < size1; ++indx1)
	{
		pData3[indx1] = new dType*[size2];
		for (uint32_t indx2 = 0; indx2 < size2; ++indx2)
		{
			pData3[indx1][indx2] = pData + indx1 * size2 * size3 + indx2 * size3;
		}
	}
	extmempt = false;
}
template <class dType>
Otensor<dType>::Otensor(uint32_t size1, uint32_t size2, uint32_t size3, uint32_t size4, dType *pdata)
{
	order = 4;
	dims[0] = size1;
	dims[1] = size2;
	dims[2] = size3;
	dims[3] = size4;
	memsize = size1 * size2 * size3 * size4;
	if (pdata == NULL)
		pData = new dType[size1 * size2 * size3 * size4];
	else
	{
		pData = pdata;
		extmem = true;
	}

	pData4 = new dType***[size1];
	for (uint32_t indx1 = 0; indx1 < size1; ++indx1)
	{
		pData4[indx1] = new dType**[size2];
		for (uint32_t indx2 = 0; indx2 < size2; ++indx2)
		{
			pData4[indx1][indx2] = new dType*[size3];
			for (uint32_t indx3 = 0; indx3 < size3; ++indx3)
			{
				pData4[indx1][indx2][indx3] = pData + indx1 * size2 * size3 * size4 + indx2 * size3 * size4  + indx3 * size4;
			}
		}
	}
	extmempt = false;
}

//axis input 0,1,2,3
/*
template <class dType>
Otensor<dType> Otensor<dType>::Transpose(uint32_t axis1, uint32_t axis2, uint32_t axis3, uint32_t axis4)
{	
	if (axis1 >= 4 || axis2 >= 4 || axis3 >= 4 || axis4 >= 4)
		return -1;

	if (axis1 == axis2 || axis1 == axis3 || axis1 == axis4 || axis2 == axis3 || axis2 == axis4 || axis3 == axis4)
		return -1;
	uint32_t axis[4] = { axis1,axis2,axis3, axis4 };

	Otensor<dType> data;
	if (order == 4)
	{
		data = Otensor<dType>(dims[axis1], dims[axis2], dims[axis3], dims[axis4]);
		for (uint32_t indx1 = 0; indx1 < dims[0]; ++indx1)
			for (uint32_t indx2 = 0; indx2 < dims[1]; ++indx2)
				for (uint32_t indx3 = 0; indx3 < dims[2]; ++indx3)
					for (uint32_t indx4 = 0; indx4 < dims[3]; ++indx4)
					{
						uint32_t indxs[4] = { indx1,indx2,indx3,indx4 };
						data(indxs[axis[0]], indxs[axis[1]], indxs[axis[2]], indxs[axis[3]]) = (*this)(indx1, indx2, indx3, indx4);
					}
	}
	else if (order == 3)
	{
		data = Otensor<dType>(dims[axis2], dims[axis3], dims[axis4]);
		for (uint32_t indx2 = 0; indx2 < dims[1]; ++indx2)
			for (uint32_t indx3 = 0; indx3 < dims[2]; ++indx3)
				for (uint32_t indx4 = 0; indx4 < dims[3]; ++indx4)
				{
					uint32_t indxs[3] = { indx2,indx3,indx4 };
					data(indxs[axis[0]], indxs[axis[1]], indxs[axis[2]]) = (*this)(indx2, indx3, indx4);
				}
	}
	else if (order == 2)
	{
		data = Otensor<dType>(dims[axis3], dims[axis4]);
		for (uint32_t indx3 = 0; indx3 < dims[2]; ++indx3)
			for (uint32_t indx4 = 0; indx4 < dims[3]; ++indx4)
			{
				uint32_t indxs[2] = {indx3,indx4 };
				data(indxs[axis[0]], indxs[axis[1]]) = (*this)(indx3, indx4);
			}
	}

	return data;
}
*/
#define ftensor Otensor<float>
#define utensor Otensor<uint8_t>

#endif