// Don't modify this file as it will be overwritten.
//
#include "CDR/CDR.h"
#include "ReturnCode.h"
#include "BasicTypes.h"
#include "md5.h"

#ifndef IDL_UserDataType_hh
#define IDL_UserDataType_hh

#ifndef UserDataType_defined
#define UserDataType_defined

struct UserDataType {
	UserDataType()
	{
		x = 0;
		y = 0;
		color = new char[255];
		color[0] = '\0';

	}

	UserDataType(const UserDataType& IDL_s);

	~UserDataType() {
		delete color;
		color = NULL;
	}

	int StructSize()
	{
		int strSize = 0;
		strSize += sizeof(UserDataType);
		strSize += strlen(color);
		return strSize;
	}
	UserDataType& operator= (const UserDataType& IDL_s);

	void Marshal(CDR* cdr) const;
	void UnMarshal(CDR* cdr);

	short x;
	long y;
	char* color;

private:
	std::string _post_data;
	std::string _data_md5;
public:
	bool set_data(const std::string& data)
	{
		//设置数据,并且计算md5
		this->_post_data = data;
		this->_data_md5 = MD5(data).toStr();
		if (NULL != this->_data_md5)
		{

		}
	};
	bool check_data_md5()
	{
		//校验本地md5
		/*if (MD5(this->_post_data).toStr() == this->_data_md5)
			return true;
		return false;*/
		return MD5(this->_post_data).toStr() == this->_data_md5 ? true : false;
	};
};

typedef sequence<UserDataType> UserDataTypeSeq;

#endif




#endif /*IDL_UserDataType_hh */
