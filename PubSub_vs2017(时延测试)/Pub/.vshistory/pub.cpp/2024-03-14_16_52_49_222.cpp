/**********************************************************
*****************发布端程序publisher.cpp********************
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <chrono>

/* IDL_TypeSupport.h中包含所有依赖的头文件 */
#include "IDL_TypeSupport.h"


DataWriter* writer = nullptr;
int nums = 0;
int g_loop_count = 0; // 无限循环

/* UserDataTypeListener继承于DataReaderListener，
   需要重写其继承过来的方法on_data_available()，在其中进行数据监听读取操作 */
class UserDataTypeListener : public DataReaderListener {
public:
	virtual void on_data_available(DataReader* reader);
};

/* 重写继承过来的方法on_data_available()，在其中进行数据监听读取操作 */
void UserDataTypeListener::on_data_available(DataReader* reader)
{
	UserDataTypeDataWriter* UserDataType_writer = nullptr;
	InstanceHandle_t instance_handle = HANDLE_NIL;

	UserDataTypeDataReader* UserDataType_reader = NULL;
	UserDataTypeSeq data_seq;
	SampleInfoSeq info_seq;
	ReturnCode_t retcode;
	int i;


	// 创建UserDataType类型的writer
	UserDataType_writer = UserDataTypeDataWriter::narrow(writer);
	if (UserDataType_writer == NULL) {
		fprintf(stderr, "DataReader narrow error\n");
		return;
	}

	/* 利用reader，创建一个读取UserDataType类型的UserDataType_reader*/
	UserDataType_reader = UserDataTypeDataReader::narrow(reader);
	if (UserDataType_reader == NULL) {
		fprintf(stderr, "DataReader narrow error\n");
		return;
	}


	/* 获取数据，存放至data_seq，data_seq是一个队列 */
	retcode = UserDataType_reader->read(
		data_seq, info_seq, 10, 0, 0, 0);

	if (retcode == RETCODE_NO_DATA) {
		return;
	}
	else if (retcode != RETCODE_OK) {
		fprintf(stderr, "take error %d\n", retcode);
		return;
	}

	/* 打印数据 */
	/* 建议1：避免在此进行复杂数据处理 */
	/* 建议2：将数据传送到其他数据处理线程中进行处理 *
	/* 建议3：假如数据结构中有string类型，用完后需手动释放 */
	for (i = 0; i < data_seq.length() && (nums < g_loop_count || g_loop_count == 0); ++i) {
		UserDataType_writer->write(data_seq[i], instance_handle);
		nums++;
	}
}


/* 删除所有实体 */
static int subscriber_shutdown(
	DomainParticipant* participant)
{
	ReturnCode_t retcode;
	int status = 0;

	if (participant != NULL) {
		retcode = participant->delete_contained_entities();
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "delete_contained_entities error %d\n", retcode);
			status = -1;
		}

		retcode = DomainParticipantFactory::get_instance()->delete_participant(participant);
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "delete_participant error %d\n", retcode);
			status = -1;
		}
	}
	return status;
}


/* 删除所有实体 */
static int publisher_shutdown(DomainParticipant* participant)
{
	ReturnCode_t retcode;
	int status = 0;

	if (participant != NULL) {
		retcode = participant->delete_contained_entities();
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "delete_contained_entities error %d\n", retcode);
			status = -1;
		}

		retcode = DomainParticipantFactory::get_instance()->delete_participant(participant);
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "delete_participant error %d\n", retcode);
			status = -1;
		}
	}

	return status;
}

void generateRandomString(char* arg_c_color, int arg_i_length = 1024) {
	static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int charsetSize = sizeof(charset) - 1;
	if (arg_i_length > 1024)
	{
		arg_i_length = 1024;
	}
	int length = std::rand() % arg_i_length;
	try
	{
		for (int i = 0; i < length; ++i) {
			arg_c_color[i] = charset[std::rand() % charsetSize];
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	arg_c_color[length] = '\0';  // 添加字符串结尾的 null 字符


}

/* 发布者函数 */
extern "C" int publisher_main(int domainId, int sample_count)
{
	DomainParticipant* participant = NULL;
	Publisher* publisher = NULL;
	Topic* topic = NULL;
	DataWriter* writer = NULL;
	UserDataTypeDataWriter* UserDataType_writer = NULL;
	UserDataType* instance = NULL;
	ReturnCode_t retcode;
	InstanceHandle_t instance_handle = HANDLE_NIL;
	const char* type_name = NULL;
	int count = 0;

	UserDataTypeListener* reader_listener = NULL;
	DataReader* reader = NULL;
	Subscriber* subscriber = NULL;

	/* 1. 创建一个participant，可以在此处定制participant的QoS */
	/* 建议1：在程序启动后优先创建participant，进行资源初始化*/
	/* 建议2：相同的domainId只创建一次participant，重复创建会占用大量资源 */
	participant = DomainParticipantFactory::get_instance()->create_participant(
		domainId, PARTICIPANT_QOS_DEFAULT/* participant默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (participant == NULL) {
		fprintf(stderr, "create_participant error\n");
		publisher_shutdown(participant);
		return -1;
	}

	/* 2. 创建一个publisher，可以在创建publisher的同时定制其QoS  */
	/* 建议1：在程序启动后优先创建publisher */
	/* 建议2：一个participant下创建一个publisher即可，无需重复创建 */
	publisher = participant->create_publisher(
		PUBLISHER_QOS_DEFAULT /* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (publisher == NULL) {
		fprintf(stderr, "create_publisher error\n");
		publisher_shutdown(participant);
		return -1;
	}
	{
		/* 2. 创建一个subscriber，可以在创建subscriber的同时定制其QoS  */
/* 建议1：在程序启动后优先创建subscriber*/
/* 建议2：一个participant下创建一个subscriber即可，无需重复创建 */
		subscriber = participant->create_subscriber(
			SUBSCRIBER_QOS_DEFAULT/* 默认QoS */,
			NULL /* listener */, STATUS_MASK_NONE);
		if (subscriber == NULL) {
			fprintf(stderr, "create_subscriber error\n");
			subscriber_shutdown(participant);
			return -1;
		}
	}

	/* 3. 在创建主题之前注册数据类型 */
	/* 建议1：在程序启动后优先进行注册 */
	/* 建议2：一个数据类型注册一次即可 */
	type_name = UserDataTypeTypeSupport::get_type_name();
	retcode = UserDataTypeTypeSupport::register_type(
		participant, type_name);
	if (retcode != RETCODE_OK) {
		fprintf(stderr, "register_type error %d\n", retcode);
		publisher_shutdown(participant);
		return -1;
	}

	/* 4. 创建主题，并定制主题的QoS  */
	/* 建议1：在程序启动后优先创建Topic */
	/* 建议2：一种主题名创建一次即可，无需重复创建 */
	topic = participant->create_topic(
		"Example UserDataType"/* 主题名 */,
		type_name /* 类型名 */, TOPIC_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (topic == NULL) {
		fprintf(stderr, "create_topic error\n");
		publisher_shutdown(participant);
		return -1;
	}

	/* 5. 创建datawriter，并定制datawriter的QoS  */
	/* 建议1：在程序启动后优先创建datawriter */
	/* 建议2：创建一次即可，无需重复创建 */
	/* 建议3：在程序退出时再进行释放 */
	/* 建议4：避免打算发送数据时创建datawriter，发送数据后删除，该做法消耗资源，影响性能 */
	writer = publisher->create_datawriter(
		topic, DATAWRITER_QOS_DEFAULT/* 默认QoS */,
		NULL /* listener */, STATUS_MASK_NONE);
	if (writer == NULL) {
		fprintf(stderr, "create_datawriter error\n");
		publisher_shutdown(participant);
		return -1;
	}
	UserDataType_writer = UserDataTypeDataWriter::narrow(writer);
	if (UserDataType_writer == NULL) {
		fprintf(stderr, "DataWriter narrow error\n");
		publisher_shutdown(participant);
		return -1;
	}

	{
		/* 5. 创建一个监听器 */
		reader_listener = new UserDataTypeListener();

		/* 6. 创建datareader，并定制datareader的QoS */
		/* 建议1：在程序启动后优先创建datareader*/
		/* 建议2：创建一次即可，无需重复创建 */
		/* 建议3：在程序退出时再进行释放 */
		/* 建议4：避免打算接收数据时创建datareader，接收数据后删除，该做法消耗资源，影响性能 */
		reader = subscriber->create_datareader(
			topic, DATAREADER_QOS_DEFAULT/* 默认QoS */,
			reader_listener/* listener */, STATUS_MASK_ALL);
		if (reader == NULL) {
			fprintf(stderr, "create_datareader error\n");
			subscriber_shutdown(participant);
			delete reader_listener;
			return -1;
		}
	}

	/* 6. 创建一个数据样本 */
	/* 建议：该数据为new出来的，使用后用户需要调用delete_data进行释放内存*/
	instance = UserDataTypeTypeSupport::create_data();
	if (instance == NULL) {
		fprintf(stderr, "UserDataTypeTypeSupport::create_data error\n");
		publisher_shutdown(participant);
		return -1;
	}

	auto start = std::chrono::high_resolution_clock::now();
	UserDataType_writer->write(*instance, instance_handle);

	/* 7. 主循环 ，发送数据 */
	for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {

		generateRandomString(instance->color);//默认长度1024,可自定义,想要更长更大,换动态数组或容器
		retcode = UserDataType_writer->write(*instance, instance_handle);
		if (retcode != RETCODE_OK) {
			fprintf(stderr, "write error %d\n", retcode);
		}
		else
		{
			fprintf(stderr, "%d : write  successfully . . \n", count);
			UserDataTypeTypeSupport::print_data(instance);
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();
	auto elapsed = stop - start;
	std::cout << "时延: " <<std::to_string(elapsed / sample_count / 2) << std::endl;

	/* 8. 删除数据样本 */
	retcode = UserDataTypeTypeSupport::delete_data(instance);
	if (retcode != RETCODE_OK) {
		fprintf(stderr, "UserDataTypeTypeSupport::delete_data error %d\n", retcode);
	}

	/* 9. 删除所有实例 */
	delete reader_listener;
	return publisher_shutdown(participant);
}

int main(int argc, char* argv[])
{
	int domain_id = 0;
	int sample_count = 0; /* 无限循环 */

	if (argc >= 2) {
		domain_id = atoi(argv[1]);  /* 发送至域domain_id */
	}
	if (argc >= 3) {
		sample_count = atoi(argv[2]); /* 发送sample_count次 */
	}

	return publisher_main(domain_id, sample_count);
}
