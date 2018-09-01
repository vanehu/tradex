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

#include "get_field_vip.h"

GetField::GetField() {
	m_syslog = basicx::SysLog_S::GetInstance();

	m_map_get_field_func[120001] = &GetField::GetField_120001_620001;
	m_map_get_field_func[120002] = &GetField::GetField_120002_620021;
	m_map_get_field_func[120003] = &GetField::GetField_120003_620002;
	m_map_get_field_func[120004] = &GetField::GetField_120004_620022;
	m_map_get_field_func[130002] = &GetField::GetField_130002_630002;
	m_map_get_field_func[130004] = &GetField::GetField_130004_630004;
	m_map_get_field_func[130005] = &GetField::GetField_130005_630005;
	m_map_get_field_func[130006] = &GetField::GetField_130006_630006;
	m_map_get_field_func[130008] = &GetField::GetField_130008_601410;
	m_map_get_field_func[130009] = &GetField::GetField_130009_601411;
	m_map_get_field_func[190001] = &GetField::GetField_190001_100065;
	m_map_get_field_func[190002] = &GetField::GetField_190002_100064;
	m_map_get_field_func[190003] = &GetField::GetField_190003_100066;
}

GetField::~GetField() {
}

void GetField::FillHead( Json::Value& results_json, int32_t ret_func, int32_t ret_numb, Request* request ) {
	results_json["ret_func"] = ret_func;
	results_json["ret_code"] = 0;
	results_json["ret_info"] = basicx::StringToUTF8( "业务提交成功。" );
	results_json["ret_task"] = request->m_req_json["task_id"].asInt();
	results_json["ret_last"] = true;
	results_json["ret_numb"] = ret_numb == 0 ? 1 : ret_numb;
	//results_json["ret_data"] = "";
}

void GetField::FillHeadQuery( Json::Value& results_json, int32_t ret_func, int32_t ret_numb, Request* request ) {
	results_json["ret_func"] = ret_func;
	results_json["ret_code"] = 0;
	if( ret_numb > 0 ) {
		results_json["ret_info"] = basicx::StringToUTF8( "业务提交成功。" );
	}
	else {
		results_json["ret_info"] = basicx::StringToUTF8( "业务提交成功。无结果数据！" );
	}
	results_json["ret_task"] = request->m_req_json["task_id"].asInt();
	results_json["ret_last"] = true;
	results_json["ret_numb"] = ret_numb;
	//results_json["ret_data"] = "";
}

bool GetField::GetField_120001_620001( int32_t api_session, Request* request, std::string& results ) { // 单个委托下单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHead( results_json, 120001, ret_numb, request ); // 120001 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号 Int
				// FID_WTPCH 委托批次号 Int
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_120002_620021( int32_t api_session, Request* request, std::string& results ) { // 单个委托撤单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHead( results_json, 120002, ret_numb, request ); // 120002 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号(撤单) Int
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_120003_620002( int32_t api_session, Request* request, std::string& results ) { // 批量委托下单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHead( results_json, 120003, ret_numb, request ); // 120003 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["batch_id"] = Fix_GetLong( api_session, FID_WTPCH, i ); // 委托批次号 Int
				ret_data_json["batch_ht"] = Fix_GetItem( api_session, FID_EN_WTH, m_field_value_huge, FIELD_VALUE_HUGE, i ); // 委托合同号 Char 6000
				// FID_COUNT 委托成功笔数 Int
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_120004_620022( int32_t api_session, Request* request, std::string& results ) { // 批量委托撤单
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHead( results_json, 120004, ret_numb, request ); // 120004 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_130002_630002( int32_t api_session, Request* request, std::string& results ) { // 查询客户资金
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130002, ret_numb, request ); // 130002 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, FID_ZJZH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 资金账号 Char 20
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char  3
				ret_data_json["available"] = Fix_GetDouble( api_session, FID_KYZJ, i ); // 可用资金 Numric 16,2
				ret_data_json["balance"] = Fix_GetDouble( api_session, FID_ZHYE, i ); // 账户余额 Numric 16,2
				ret_data_json["frozen"] = Fix_GetDouble( api_session, FID_DJJE, i ); // 冻结金额 Numric 16,2
				// FID_YJLX  预计利息  Numric  16,2
				// FID_ZHZT  账户状态  Int
				// FID_JGDM  机构代码  Char    4
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_130004_630004( int32_t api_session, Request* request, std::string& results ) { // 查询客户持仓
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130004, ret_numb, request ); // 130004 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, FID_GDH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所编码 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券类别 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 证券名称 Char 8
				ret_data_json["security_qty"] = Fix_GetLong( api_session, FID_JCCL, i ); // 今持仓量 Long //不要用证券数量
				ret_data_json["can_sell"] = Fix_GetLong( api_session, FID_KMCSL, i ); // 可卖出数量 Long
				ret_data_json["can_sub"] = Fix_GetLong( api_session, FID_KSGSL, i ); // 可申购数量 Long
				ret_data_json["can_red"] = Fix_GetLong( api_session, FID_KSHSL, i ); // 可赎回数量 Long
				ret_data_json["non_tradable"] = Fix_GetLong( api_session, FID_FLTSL, i ); // 非流通数量 Long
				ret_data_json["frozen_qty"] = Fix_GetLong( api_session, FID_DJSL, i ); // 冻结数量 Long
				ret_data_json["sell_qty"] = Fix_GetLong( api_session, FID_DRMCCJSL, i ); // 当日卖出成交数量 Long
				ret_data_json["sell_money"] = Fix_GetDouble( api_session, FID_DRMCCJJE, i ); // 当日卖出成交金额 Numric 16,2
				ret_data_json["buy_qty"] = Fix_GetLong( api_session, FID_DRMRCJSL, i ); // 当日买入成交数量 Long
				ret_data_json["buy_money"] = Fix_GetDouble( api_session, FID_DRMRCJJE, i ); // 当日买入成交金额 Numric 16,2
				ret_data_json["sub_qty"] = Fix_GetLong( api_session, FID_SGCJSL, i ); // 申购成交数量 Long
				ret_data_json["red_qty"] = Fix_GetLong( api_session, FID_SHCJSL, i ); // 赎回成交数量 Long
				// FID_DRMCWTSL  当日卖出委托数量  Long
				// FID_DRMRWTSL  当日买入委托数量  Long
				// FID_KCRQ      开仓日期          Int
				// FID_ZQSL      证券数量          Long
				// FID_WJSSL     未交收数量        Long
				// FID_BDRQ      变动日期          Int
				// FID_MCDXSL    卖出抵消数量      Long
				// FID_MRDXSL    买入抵消数量      Long
				// FID_JYDW      交易单位          Int
				// FID_MCSL      累计卖出数量      Long
				// FID_MRSL      累计买入数量      Long
				// FID_PGSL      配股数量          Long
				// FID_SGSL      申购数量          Long
				// FID_TBFDYK    摊薄浮动盈亏      Numric  16,2
				// FID_TBBBJ     摊薄保本价        Numric  9,3
				// FID_TBCBJ     摊薄成本价        Numric  9,3
				// FID_CCJJ      持仓均价          Numric  9,3
				// FID_FDYK      浮动盈亏          Numric  16,2
				// FID_HLJE      红利金额          Numric  16,2
				// FID_LJYK      累计盈亏          Numric  16,2
				// FID_MCJE      卖出金额          Numric  16,2
				// FID_MRJE      买入金额          Numric  16,2
				// FID_MRJJ      买入均价          Numric  9,3
				// FID_PGJE      配股金额          Numric  16,2
				// FID_ZXSZ      最新市值          Numric  16,2
				// FID_BBJ       保本价            Numric  9,3
				// FID_ZXJ       最新价            Numric  9,3
				// FID_GPSZ      非流通市值        Numric  16,2
				// FID_LXBJ      利息报价          Numric  16,9
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_130005_630005( int32_t api_session, Request* request, std::string& results ) { // 查询客户当日委托
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130005, ret_numb, request ); // 130005 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, FID_GDH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所编码 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券类别 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 证券名称 Char 8
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号 Int
				int64_t entr_type = Fix_GetLong( api_session, FID_DDLX, i ); // 订单类型 Int
				if( 0 == entr_type ) { // 限价
					ret_data_json["entr_type"] = 1; // 顶点上证和深证均为 0 限价
				}
				else if( 1 == entr_type || 104 == entr_type ) { // 市价
					ret_data_json["entr_type"] = 2; // 顶点上证为 1 市价，顶点深证为 104 市价
				}
				else {
					return false;
				}
				ret_data_json["exch_side"] = Fix_GetLong( api_session, FID_WTLB, i ); // 委托类别 Int
				ret_data_json["price"] = Fix_GetDouble( api_session, FID_WTJG, i ); // 委托价格 Nurmic 9,3
				ret_data_json["amount"] = Fix_GetLong( api_session, FID_WTSL, i ); // 委托数量 Int
				ret_data_json["fill_price"] = Fix_GetDouble( api_session, FID_CJJG, i ); // 成交价格 Numric 9,3
				ret_data_json["fill_amount"] = Fix_GetLong( api_session, FID_CJSL, i ); // 成交数量 Int
				ret_data_json["fill_money"] = Fix_GetDouble( api_session, FID_CJJE, i ); // 成交金额 Numric 16,2
				ret_data_json["report_ret"] = Fix_GetLong( api_session, FID_SBJG, i ); // 申报结果 Int
				ret_data_json["cxl_qty"] = Fix_GetLong( api_session, FID_CDSL, i ); // 撤单数量 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["cxl_flag"] = Fix_GetItem( api_session, FID_CXBZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 撤销标志 Char 1
				ret_data_json["frozen"] = Fix_GetDouble( api_session, FID_DJZJ, i ); // 冻结资金 Numric 16,2
				ret_data_json["settlement"] = Fix_GetDouble( api_session, FID_QSZJ, i ); // 清算资金 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["brow_index"] = Fix_GetItem( api_session, FID_BROWINDEX, m_field_value_short, FIELD_VALUE_SHORT, i ); // 增量查询索引值 Char 128
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["report_time"] = Fix_GetItem( api_session, FID_SBSJ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 申报时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["order_time"] = Fix_GetItem( api_session, FID_WTSJ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 委托时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fill_time"] = Fix_GetItem( api_session, FID_CJSJ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 成交时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, FID_ZJZH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 资金账号 Char 20
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["message"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_JGSM, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 结果说明 Char 64
				// FID_BPGDH     报盘股东号      Char  10
				// FID_WTFS      委托方式        Int
				// FID_SBWTH     申报委托号      Char  10
				// FID_WTPCH     委托批次号      Int
				// FID_ZJDJLSH   资金冻结流水号  Int
				// FID_ZQDJLSH   证券冻结流水号  Int
				// FID_SBRQ      委托申报日期    Int
				// FID_SBJLH     申报记录号      Int
				// FID_WTRQ      委托日期        Int
				// FID_ADDR_IP   IP 地址         Char  16
				// FID_ADDR_MAC  MAC 地址        Char  12
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_130006_630006( int32_t api_session, Request* request, std::string& results ) { // 查询客户当日成交
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130006, ret_numb, request ); // 130006 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, FID_GDH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所编码 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券类别 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 证券名称 Char 8
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["trans_id"] = Fix_GetItem( api_session, FID_CJBH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 成交编号 Char 16
				ret_data_json["exch_side"] = Fix_GetLong( api_session, FID_WTLB, i ); // 委托类别 Int
				ret_data_json["fill_price"] = Fix_GetDouble( api_session, FID_CJJG, i ); // 成交价格 Numric 9,3
				ret_data_json["fill_amount"] = Fix_GetLong( api_session, FID_CJSL, i ); // 成交数量 Int
				ret_data_json["fill_money"] = Fix_GetDouble( api_session, FID_CJJE, i ); // 成交金额 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["cxl_flag"] = Fix_GetItem( api_session, FID_CXBZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 撤销标志 Char 1
				ret_data_json["settlement"] = Fix_GetDouble( api_session, FID_QSJE, i ); // 清算金额 Numric 16,2
				ret_data_json["commission"] = Fix_GetDouble( api_session, FID_S1, i ); // 佣金 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["brow_index"] = Fix_GetItem( api_session, FID_BROWINDEX, m_field_value_short, FIELD_VALUE_SHORT, i ); // 增量查询索引值 Char 128
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, FID_ZJZH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 资金账号 Char 20
				// FID_HBXH   回报序号    Int
				// FID_LXBJ   利息报价    Numric  16,9
				// FID_SBWTH  申报委托号  Char    10
				// FID_LX     利息        Numric  16,2
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_130008_601410( int32_t api_session, Request* request, std::string& results ) { // 查询ETF基本信息
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130008, ret_numb, request ); // 130008 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_JJMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // ETF基金名称 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_id_1"] = Fix_GetItem( api_session, FID_SGDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF申赎代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_id_2"] = Fix_GetItem( api_session, FID_JJDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF基金代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所 Char 2
				ret_data_json["count"] = Fix_GetLong( api_session, FID_COUNT, i ); // 股票记录数 Int
				ret_data_json["status"] = Fix_GetLong( api_session, FID_SGSHZT, i ); // 申赎允许状态 Int
				ret_data_json["pub_iopv"] = Fix_GetLong( api_session, FID_LOGICAL, i ); // 是否发布IOPV Int
				ret_data_json["unit"] = Fix_GetLong( api_session, FID_TZDW, i ); // 最小申购赎回单位 Int
				ret_data_json["cash_ratio"] = Fix_GetDouble( api_session, FID_XJTDBL, i ); // 最大现金替代比例 Numeric 7,5
				ret_data_json["cash_diff"] = Fix_GetDouble( api_session, FID_XJCE, i ); // T日现金差额 Numeric 11,2
				ret_data_json["iopv"] = Fix_GetDouble( api_session, FID_DWJZ, i ); // T-1日基金单位净值 Numeric 8,4
				ret_data_json["trade_iopv"] = Fix_GetDouble( api_session, FID_SGSHDWJZ, i ); // T-1日申赎单位净值 Numeric 12,2
				// FID_KSRQ    网下现金认购开始日期  Int
				// FID_RGDM    认购代码              Char 6
				// FID_RGQRDM  认购确认代码          Char 6
				// FID_RQ      ETF网上认购日期       Int
				// FID_XJDM    现金代码(资金代码)    Char 6
				// FID_JSRQ    网下现金认购结束日期  Int
				// FID_WJLJ    文件路径              Char 255
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_130009_601411( int32_t api_session, Request* request, std::string& results ) { // 查询ETF成分股信息
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130009, ret_numb, request ); // 130009 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_name"] = Fix_GetItem( api_session, FID_JJDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF基金代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["stock_code"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF成份股代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["stock_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // ETF成份股名称 Char 8
				ret_data_json["stock_qty"] = Fix_GetLong( api_session, FID_ZQSL, i ); // ETF成份股数量 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所 Char 2
				ret_data_json["replace_flag"] = Fix_GetLong( api_session, FID_TDBZ, i ); // 替代标志 Int
				ret_data_json["replace_money"] = Fix_GetDouble( api_session, FID_TDJE, i ); // 替代金额  Numeric 16,2
				ret_data_json["up_px_ratio"] = Fix_GetDouble( api_session, FID_YJBL, i ); // 溢价比例  Numeric 16,2
				results_json["ret_data"].append( ret_data_json );
			}
			results = Json::writeString( m_json_writer, results_json );
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_190001_100065( int32_t api_session, Request* request, std::string& results ) { // 申报回报
	try {
		Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
		results_json["ret_func"] = 190001;
		results_json["task_id"] = request->m_req_json["task_id"].asInt();
		results_json["order_id"] = Fix_GetLong( api_session, FID_WTH ); // 委托号 Int
		results_json["exch_side"] = Fix_GetLong( api_session, FID_WTLB ); // 委托类别 Int
		memset( m_field_value_short_sb, 0, FIELD_VALUE_SHORT );
		results_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short_sb, FIELD_VALUE_SHORT ); // 证券代码 Char 6
		memset( m_field_value_short_sb, 0, FIELD_VALUE_SHORT );
		results_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short_sb, FIELD_VALUE_SHORT ); // 证券类别 Char 2
		memset( m_field_value_short_sb, 0, FIELD_VALUE_SHORT );
		results_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short_sb, FIELD_VALUE_SHORT ); // 交易所 Char 2
		results_json["cxl_qty"] = Fix_GetLong( api_session, FID_CDSL ); // 撤单数量 Int
		results_json["commit_ret"] = Fix_GetLong( api_session, FID_SBJG ); // 申报结果 Int
		memset( m_field_value_short_sb, 0, FIELD_VALUE_SHORT );
		results_json["commit_msg"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_JGSM, m_field_value_short_sb, FIELD_VALUE_SHORT ) ); // 结果说明 Char 64
		// FID_QRBZ   确认标志        Int
		// FID_GDH    股东号          Char     10
		// FID_BZ     币种            Char     3
		// FID_CXBZ   撤销标志        Char     1
		// FID_DJZJ   冻结资金        Numeric  16,2
		results = Json::writeString( m_json_writer_sb, results_json );
		return true;
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_190002_100064( int32_t api_session, Request* request, std::string& results ) { // 成交回报
	try {
		Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
		results_json["ret_func"] = 190002;
		results_json["task_id"] = request->m_req_json["task_id"].asInt();
		results_json["order_id"] = Fix_GetLong( api_session, FID_WTH ); // 委托号 Int
		results_json["exch_side"] = Fix_GetLong( api_session, FID_WTLB ); // 委托类别 Int
		memset( m_field_value_short_cj, 0, FIELD_VALUE_SHORT );
		results_json["trans_id"] = Fix_GetItem( api_session, FID_CJBH, m_field_value_short_cj, FIELD_VALUE_SHORT ); // 成交编号 Char 16
		memset( m_field_value_short_cj, 0, FIELD_VALUE_SHORT );
		results_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short_cj, FIELD_VALUE_SHORT ); // 证券代码 Char 6
		memset( m_field_value_short_cj, 0, FIELD_VALUE_SHORT );
		results_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short_cj, FIELD_VALUE_SHORT ); // 证券类别 Char 2
		memset( m_field_value_short_cj, 0, FIELD_VALUE_SHORT );
		results_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short_cj, FIELD_VALUE_SHORT ); // 交易所 Char 2
		results_json["fill_qty"] = Fix_GetLong( api_session, FID_CJSL ); // 本次成交数量 Int
		results_json["fill_price"] = Fix_GetDouble( api_session, FID_CJJG ); // 本次成交价格 Numeric 9,3
		memset( m_field_value_short_cj, 0, FIELD_VALUE_SHORT );
		results_json["fill_time"] = Fix_GetItem( api_session, FID_CJSJ, m_field_value_short_cj, FIELD_VALUE_SHORT ); // 成交时间 Char 8
		results_json["cxl_qty"] = Fix_GetLong( api_session, FID_CDSL ); // 撤单数量 Int
		// FID_GDH    股东号          Char     10
		// FID_BZ     币种            Char     3
		// FID_CXBZ   撤销标志        Char     1
		// FID_QSZJ   清算资金        Numeric  16,2
		// FID_ZCJSL  委托总成交数量  Int
		// FID_ZCJJE  委托总成交金额  Numeric  16,2
		// FID_CJJE   本次成交金额    Numeric  16,2
		results = Json::writeString( m_json_writer_cj, results_json );
		return true;
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool GetField::GetField_190003_100066( int32_t api_session, Request* request, std::string& results ) { // 撤单回报
	try {
		Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
		results_json["ret_func"] = 190003;
		results_json["task_id"] = request->m_req_json["task_id"].asInt();
		results_json["order_id"] = Fix_GetLong( api_session, FID_WTH ); // 委托号 Int
		results_json["exch_side"] = Fix_GetLong( api_session, FID_WTLB ); // 委托类别 Int
		memset( m_field_value_short_cd, 0, FIELD_VALUE_SHORT );
		results_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short_cd, FIELD_VALUE_SHORT ); // 证券代码 Char 6
		memset( m_field_value_short_cd, 0, FIELD_VALUE_SHORT );
		results_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short_cd, FIELD_VALUE_SHORT ); // 证券类别 Char 2
		memset( m_field_value_short_cd, 0, FIELD_VALUE_SHORT );
		results_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short_cd, FIELD_VALUE_SHORT ); // 交易所 Char 2
		results_json["cxl_qty"] = Fix_GetLong( api_session, FID_CDSL ); // 撤单数量 Int
		results_json["total_fill_qty"] = Fix_GetLong( api_session, FID_CJSL ); // 成交数量 Int
		// FID_GDH    股东号          Char     10
		// FID_BZ     币种            Char     3
		// FID_CXBZ   撤销标志        Char     1
		// FID_DJZJ   冻结资金        Numeric  16,2
		results = Json::writeString( m_json_writer_cd, results_json );
		return true;
	}
	catch( ... ) {
		return false;
	}
	return false;
}
