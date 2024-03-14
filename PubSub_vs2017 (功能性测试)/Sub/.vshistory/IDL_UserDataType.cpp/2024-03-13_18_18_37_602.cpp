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
}


/// <summary>
/// ����md5,�������Ļ���üӸ�time��salt
/// </summary>
/// <param name="arg_i_type">�洢��ʽ,0=�洢��this,1=�洢�� *arg_s_premd5</param>
/// <param name="arg_s_premd5"></param>
/// <returns></returns>
bool calc_md5(int arg_i_type, std::string* arg_s_premd5 = nullptr);
{
	std::ostringstream oss;
	oss << x << y << color;
	std::string combinedData = oss.str();
	this->s_md5 = MD5(combinedData).toStr();
	if ("" == this->s_md5)
		return false;
	return true;
}
/// <summary>
/// У�鱾��md5��ֵ
/// </summary>
/// <returns></returns>
bool UserDataType::check_data_md5()
{
	std::string calced_md5;
	if (calc_md5(calced_md5))
		return calced_md5 == this->s_md5 ? true : false;
	return false;

}

/// <summary>
/// ��ȡmd5
/// </summary>
/// <returns></returns>
std::string UserDataType::get_md5() const
{
	return this->s_md5;
}
