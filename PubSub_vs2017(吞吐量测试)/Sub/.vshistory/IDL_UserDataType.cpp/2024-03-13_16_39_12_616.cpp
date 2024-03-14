// Don't modify this file as it will be overwritten.
//
#include "IDL_UserDataType.h"

UserDataType::UserDataType(const UserDataType& IDL_s) {
	x = IDL_s.x;
	y = IDL_s.y;
	color = IDL_s.color;
}

UserDataType& UserDataType::operator= (const UserDataType& IDL_s) {
	if (this == &IDL_s) return *this;
	x = IDL_s.x;
	y = IDL_s.y;
	color = IDL_s.color;
	return *this;
}

void UserDataType::Marshal(CDR* cdr) const {
	cdr->PutShort(x);
	cdr->PutLong(y);
	cdr->PutString(color);
}

void UserDataType::UnMarshal(CDR* cdr) {
	cdr->GetShort(x);
	cdr->GetLong(y);
	{
		char* IDL_str;
		cdr->GetString(IDL_str);
		if (color != NULL)
		{
			delete color;
			color = NULL;
		}
		color = IDL_str;
	}
	//计算存储md5
	this->calc_md5(this->s_md5);
}


/// <summary>
/// 计算md5,有条件的话最好加个time和salt
/// </summary>
/// <returns></returns>
bool UserDataType::calc_md5(std::string& arg_s_md5)
{
	std::ostringstream oss;
	oss << x << y << color;
	std::string combinedData = oss.str();
	arg_s_md5 = MD5(combinedData).toStr();
	if ("" == arg_s_md5)
		return false;
	return true;
}
/// <summary>
/// 校验本地md5数值
/// </summary>
/// <returns></returns>
bool UserDataType::check_data_md5()
{
	std::string calced_md5;
	if (calc_md5(calced_md5))
		return calced_md5 == this->s_md5 ? true : false;
	return false;

}
