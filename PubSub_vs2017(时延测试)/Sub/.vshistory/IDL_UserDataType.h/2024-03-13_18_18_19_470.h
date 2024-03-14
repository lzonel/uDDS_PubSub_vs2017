// Don't modify this file as it will be overwritten.
//
#include "CDR/CDR.h"
#include "ReturnCode.h"
#include "BasicTypes.h"
#include "md5.h"
#include <sstream>


#ifndef IDL_UserDataType_hh
#define IDL_UserDataType_hh

#ifndef UserDataType_defined
#define UserDataType_defined

struct UserDataType {
	UserDataType()
	{
		x = 7;
		y = 7;
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
	std::string s_md5;

	//private:
	//	std::string _post_data;
	//	std::string _data_md5;
	//public:
	//	/// <summary>
	//	/// 设置请求数据
	//	/// </summary>
	//	/// <param name="data">数据</param>
	//	/// <returns></returns>
	//	bool set_data(const std::string& data)
	//	{
	//		//设置数据,并且计算md5
	//		this->_post_data = data;
	//		this->_data_md5 = MD5(data).toStr();
	//		if ("" != this->_data_md5)
	//			return true;
	//		return false;
	//	};


public:
	/// <summary>
	/// 计算md5,有条件的话最好加个time和salt
	/// </summary>
	/// <param name="arg_i_type">存储方式,0=存储到this,1=存储到</param>
	/// <param name="arg_s_premd5"></param>
	/// <returns></returns>
	bool calc_md5(int arg_i_type, std::string* arg_s_premd5 = nullptr);
	/// <summary>
	/// 校验本地md5数值
	/// </summary>
	/// <returns>true==正确</returns>
	bool check_data_md5();
	/// <summary>
	/// 获取md5
	/// </summary>
	/// <returns></returns>
	std::string get_md5()const;
	//	std::string get_data() const
	//	{
	//		return this->_post_data;
	//	}
};

typedef sequence<UserDataType> UserDataTypeSeq;

#endif




#endif /*IDL_UserDataType_hh */
