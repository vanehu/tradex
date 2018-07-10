/*
* Copyright (c) 2018-2018 the TradeX authors
* All rights reserved.
*
* The project sponsor and lead author is Xu Rendong.
* E-mail: xrd@ustc.edu, QQ: 277195007, WeChat: ustc_xrd
* You can get more information at https://xurendong.github.io
* For names of other contributors see the contributors file.
*
* Commercial use of this code in source and binary forms is
* governed by a LGPL v3 license. You may get a copy from the
* root directory. Or else you should get a specific written
* permission from the project author.
*
* Individual and educational use of this code in source and
* binary forms is governed by a 3-clause BSD license. You may
* get a copy from the root directory. Certainly welcome you
* to contribute code of all sorts.
*
* Be sure to retain the above copyright notice and conditions.
*/

#ifndef TRADER_APE_GET_FIELD_APE_H
#define TRADER_APE_GET_FIELD_APE_H

#include "struct_ape.h"

#define FIELD_VALUE_SHORT 256
#define FIELD_VALUE_LONG 2048
#define FIELD_VALUE_HUGE 10240

class GetField
{
public:
	GetField();
	~GetField();

public:
	typedef bool (GetField::*GetFieldFunc)( int32_t api_session, Request* request, std::string& results );

	void FillHead( Json::Value& results_json, int32_t ret_func, int32_t ret_numb, Request* request );
	void FillHeadQuery( Json::Value& results_json, int32_t ret_func, int32_t ret_numb, Request* request );
	bool GetField_120001_620001( int32_t api_session, Request* request, std::string& results ); // 单个委托下单
	bool GetField_120002_620021( int32_t api_session, Request* request, std::string& results ); // 单个委托撤单
	bool GetField_120003_620002( int32_t api_session, Request* request, std::string& results ); // 批量委托下单
	bool GetField_120004_620022( int32_t api_session, Request* request, std::string& results ); // 批量委托撤单
	bool GetField_130002_630002( int32_t api_session, Request* request, std::string& results ); // 查询客户资金
	bool GetField_130004_630004( int32_t api_session, Request* request, std::string& results ); // 查询客户持仓
	bool GetField_130005_630005( int32_t api_session, Request* request, std::string& results ); // 查询客户当日委托
	bool GetField_130006_630006( int32_t api_session, Request* request, std::string& results ); // 查询客户当日成交
	bool GetField_130008_601410( int32_t api_session, Request* request, std::string& results ); // 查询ETF基本信息
	bool GetField_130009_601411( int32_t api_session, Request* request, std::string& results ); // 查询ETF成分股信息
	bool GetField_190001_100065( int32_t api_session, Request* request, std::string& results ); // 申报回报
	bool GetField_190002_100064( int32_t api_session, Request* request, std::string& results ); // 成交回报
	bool GetField_190003_100066( int32_t api_session, Request* request, std::string& results ); // 撤单回报

public:
	basicx::SysLog_S* m_syslog;
	Json::StreamWriterBuilder m_json_writer;
	Json::StreamWriterBuilder m_json_writer_sb; // 预防多线程
	Json::StreamWriterBuilder m_json_writer_cj; // 预防多线程
	Json::StreamWriterBuilder m_json_writer_cd; // 预防多线程
	char m_field_value_short[FIELD_VALUE_SHORT];
	char m_field_value_long[FIELD_VALUE_LONG];
	char m_field_value_huge[FIELD_VALUE_HUGE];
	std::unordered_map<int32_t, GetFieldFunc> m_map_get_field_func;
};

#endif // TRADER_APE_GET_FIELD_APE_H
