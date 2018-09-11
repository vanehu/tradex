/*
* Copyright (c) 2018-2018 the TradeX authors
* All rights reserved.
*
* The project sponsor and lead author is Xu Rendong.
* E-mail: xrd@ustc.edu, QQ: 277195007, WeChat: ustc_xrd
* See the contributors file for names of other contributors.
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

#include "set_field_vip.h"

SetField::SetField() {
	m_syslog = basicx::SysLog_S::GetInstance();

	m_map_set_field_func[120001] = &SetField::SetField_120001_620001;
	m_map_set_field_func[120002] = &SetField::SetField_120002_620021;
	m_map_set_field_func[120003] = &SetField::SetField_120003_620002;
	m_map_set_field_func[120004] = &SetField::SetField_120004_620022;
	m_map_set_field_func[130002] = &SetField::SetField_130002_630002;
	m_map_set_field_func[130004] = &SetField::SetField_130004_630004;
	m_map_set_field_func[130005] = &SetField::SetField_130005_630005;
	m_map_set_field_func[130006] = &SetField::SetField_130006_630006;
	m_map_set_field_func[130008] = &SetField::SetField_130008_601410;
	m_map_set_field_func[130009] = &SetField::SetField_130009_601411;
}

SetField::~SetField() {
}

bool SetField::SetField_120001_620001( int32_t api_session, Request* request ) { // 单个委托下单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetString( api_session, FID_GDH, request->m_req_json["holder"].asCString() ); // 股东号 Char 10
			Fix_SetString( api_session, FID_ZQDM, request->m_req_json["symbol"].asCString() ); // 证券代码 Char 6
			std::string exchange = request->m_req_json["exchange"].asCString();
			Fix_SetString( api_session, FID_JYS, exchange.c_str() ); // 交易所编码 Char 2
			int32_t entr_type = 0; // 1 限价、2 市价
			double price = 0.0;
			int32_t exch_side = request->m_req_json["exch_side"].asInt();
			if( 1 == exch_side || 2 == exch_side ) { // 1 买入、2 卖出、29 申购、30 赎回、37 质入、38 质出
				entr_type = request->m_req_json["entr_type"].asInt();
				if( 1 == entr_type ) { // 限价
					entr_type = 0; // 顶点上证和深证均为 0 限价
				}
				else if( 2 == entr_type && "SH" == exchange ) { // 市价
					entr_type = 1; // 顶点上证为 1 市价
				}
				else if( 2 == entr_type && "SZ" == exchange ) { // 市价
					entr_type = 104; // 顶点深证为 104 市价
				}
				else {
					return false;
				}
				price = request->m_req_json["price"].asDouble();
			}
			Fix_SetLong( api_session, FID_DDLX, entr_type ); // 订单类型 Int
			Fix_SetLong( api_session, FID_WTLB, exch_side ); // 委托类别 Int
			Fix_SetDouble( api_session, FID_WTJG, price ); // 委托价格 Numric 9,3
			Fix_SetLong( api_session, FID_WTSL, request->m_req_json["amount"].asInt() ); // 委托数量 Int
			// FID_WTPCH 委托批次号 Int
			// FID_DFXW 对方席位号 Char 6
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120002_620021( int32_t api_session, Request* request ) { // 单个委托撤单
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 原委托号 Int
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120003_620002( int32_t api_session, Request* request ) { // 批量委托下单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_COUNT, request->m_req_json["order_numb"].asInt() ); // 委托笔数 Int
			Fix_SetString( api_session, FID_FJXX, request->m_req_json["order_list"].asCString() ); // 委托详细信息 Char 15000
			// FID_WTPCH 委托批次号 Int
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120004_620022( int32_t api_session, Request* request ) { // 批量委托撤单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_WTPCH, request->m_req_json["batch_id"].asInt() ); // 委托批次号 Int
			Fix_SetString( api_session, FID_EN_WTH, request->m_req_json["batch_ht"].asCString() ); // 撤单委托号范围 Char 6000
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130002_630002( int32_t api_session, Request* request ) { // 查询客户资金
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130004_630004( int32_t api_session, Request* request ) { // 查询客户持仓
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充

			// FID_GDH 股东号 Char 10
			// FID_JYS 交易所编码 Char 2
			// FID_ZQDM 证券代码 Char 6
			// FID_EXFLG 查询扩展信息标志 Int
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130005_630005( int32_t api_session, Request* request ) { // 查询客户当日委托
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 委托号 Int
			Fix_SetString( api_session, FID_BROWINDEX, request->m_req_json["brow_index"].asCString() ); // 增量查询索引值 Char 128
			// FID_GDH 股东号 Char 10
			// FID_JYS 交易所编码 Char 2
			// FID_WTLB 委托类别 Int
			// FID_ZQDM 证券代码 Char 6
			// FID_WTPCH 委托批次号 Int
			// FID_FLAG 查询标志（0 所有委托，1 可撤单委托）Int
			// FID_CXBZ 撤销标志 Char 1
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130006_630006( int32_t api_session, Request* request ) { // 查询客户当日成交
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 委托号 Int
			Fix_SetString( api_session, FID_BROWINDEX, request->m_req_json["brow_index"].asCString() ); // 增量查询索引值 Char 128
			// FID_GDH 股东号 Char 10
			// FID_JYS 交易所编码 Char 2
			// FID_WTLB 委托类别 Int
			// FID_ZQDM 证券代码 Char 6
			// FID_WTPCH 委托批次号 Int
			// FID_FLAG 查询标志（0 所有委托，1 可撤单委托）Int
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130008_601410( int32_t api_session, Request* request ) { // 查询ETF基本信息
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Fix_SetString( api_session, FID_JJDM, request->m_req_json["fund_id_2"].asCString() ); // 基金代码 Char 6
			// FID_JYS 交易所 Char 2
			// FID_SGDM ETF申赎代码 Char 6
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130009_601411( int32_t api_session, Request* request ) { // 查询ETF成分股信息
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Fix_SetString( api_session, FID_JJDM, request->m_req_json["fund_id_2"].asCString() ); // 基金代码 Char 6
			// FID_JYS 交易所 Char 2
			// FID_TYPE 基金类型 Char 2
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}
