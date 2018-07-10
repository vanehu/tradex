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

#include "get_field_ape.h"

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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, 681, i ); // FID_WTH 委托号 Int
				// FID_WTPCH 委托批次号 Int
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, 681, i ); // FID_WTH 委托号(撤单) Int
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				ret_data_json["batch_id"] = Fix_GetLong( api_session, 1017, i ); // FID_WTPCH 委托批次号 Int
				ret_data_json["batch_ht"] = Fix_GetItem( api_session, 705, m_field_value_huge, FIELD_VALUE_HUGE, i ); // FID_EN_WTH 委托合同号 Char 6000
				// FID_COUNT 委托成功笔数 Int
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, 716, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZJZH 资金账号 Char 20
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, 511, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_BZ 币种 Char  3
				ret_data_json["available"] = Fix_GetDouble( api_session, 619, i ); // FID_KYZJ 可用资金 Numric 16,2
				ret_data_json["balance"] = Fix_GetDouble( api_session, 709, i ); // FID_ZHYE 账户余额 Numric 16,2
				ret_data_json["frozen"] = Fix_GetDouble( api_session, 826, i ); // FID_DJJE 冻结金额 Numric 16,2
				// FID_YJLX  预计利息  Numric  16,2
				// FID_ZHZT  账户状态  Int
				// FID_JGDM  机构代码  Char    4
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, 571, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_GDH 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JYS 交易所编码 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, 511, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_BZ 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQDM 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, 720, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQLB 证券类别 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = Fix_GetItem( api_session, 722, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQMC 证券名称 Char 8
				ret_data_json["security_qty"] = Fix_GetLong( api_session, 757, i ); // FID_JCCL 今持仓量 Long //不要用证券数量
				ret_data_json["can_sell"] = Fix_GetLong( api_session, 615, i ); // FID_KMCSL 可卖出数量 Long
				ret_data_json["can_sub"] = Fix_GetLong( api_session, 1179, i ); // FID_KSGSL 可申购数量 Long
				ret_data_json["can_red"] = Fix_GetLong( api_session, 1184, i ); // FID_KSHSL 可赎回数量 Long
				ret_data_json["non_tradable"] = Fix_GetLong( api_session, 568, i ); // FID_FLTSL 非流通数量 Long
				ret_data_json["frozen_qty"] = Fix_GetLong( api_session, 1235, i ); // FID_DJSL 冻结数量 Long
				ret_data_json["sell_qty"] = Fix_GetLong( api_session, 541, i ); // FID_DRMCCJSL 当日卖出成交数量 Long
				ret_data_json["sell_money"] = Fix_GetDouble( api_session, 540, i ); // FID_DRMCCJJE 当日卖出成交金额 Numric 16,2
				ret_data_json["buy_qty"] = Fix_GetLong( api_session, 545, i ); // FID_DRMRCJSL 当日买入成交数量 Long
				ret_data_json["buy_money"] = Fix_GetDouble( api_session, 544, i ); // FID_DRMRCJJE 当日买入成交金额 Numric 16,2
				ret_data_json["sub_qty"] = Fix_GetLong( api_session, 9299, i ); // FID_SGCJSL 申购成交数量 Long
				ret_data_json["red_qty"] = Fix_GetLong( api_session, 9300, i ); // FID_SHCJSL 赎回成交数量 Long
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
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, 571, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_GDH 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JYS 交易所编码 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, 511, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_BZ 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQDM 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, 720, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQLB 证券类别 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = Fix_GetItem( api_session, 722, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQMC 证券名称 Char 8
				ret_data_json["order_id"] = Fix_GetLong( api_session, 681, i ); // FID_WTH 委托号 Int
				int64_t entr_type = Fix_GetLong( api_session, 1013, i ); // FID_DDLX 订单类型 Int
				if( 0 == entr_type ) { // 限价
					ret_data_json["entr_type"] = 1; // 顶点上证和深证均为 0 限价
				}
				else if( 1 == entr_type || 104 == entr_type ) { //市价
					ret_data_json["entr_type"] = 2; // 顶点上证为 1 市价，顶点深证为 104 市价
				}
				else {
					return false;
				}
				ret_data_json["exch_side"] = Fix_GetLong( api_session, 683, i ); // FID_WTLB 委托类别 Int
				ret_data_json["price"] = Fix_GetDouble( api_session, 682, i ); // FID_WTJG 委托价格 Nurmic 9,3
				ret_data_json["amount"] = Fix_GetLong( api_session, 684, i ); // FID_WTSL 委托数量 Int
				ret_data_json["fill_price"] = Fix_GetDouble( api_session, 525, i ); // FID_CJJG 成交价格 Numric 9,3
				ret_data_json["fill_amount"] = Fix_GetLong( api_session, 528, i ); // FID_CJSL 成交数量 Int
				ret_data_json["fill_money"] = Fix_GetDouble( api_session, 524, i ); // FID_CJJE 成交金额 Numric 16,2
				ret_data_json["report_ret"] = Fix_GetLong( api_session, 753, i ); // FID_SBJG 申报结果 Int
				ret_data_json["cxl_qty"] = Fix_GetLong( api_session, 886, i ); // FID_CDSL 撤单数量 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["cxl_flag"] = Fix_GetItem( api_session, 755, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_CXBZ 撤销标志 Char 1
				ret_data_json["frozen"] = Fix_GetDouble( api_session, 764, i ); // FID_DJZJ 冻结资金 Numric 16,2
				ret_data_json["settlement"] = Fix_GetDouble( api_session, 647, i ); // FID_QSZJ 清算资金 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["brow_index"] = Fix_GetItem( api_session, 763, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_BROWINDEX 增量查询索引值 Char 128
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["report_time"] = Fix_GetItem( api_session, 751, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_SBSJ 申报时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["order_time"] = Fix_GetItem( api_session, 750, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_WTSJ 委托时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fill_time"] = Fix_GetItem( api_session, 527, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_CJSJ 成交时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, 716, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZJZH 资金账号 Char 20
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["message"] = Fix_GetItem( api_session, 830, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JGSM 结果说明 Char 64
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
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, 571, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_GDH 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JYS 交易所编码 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, 511, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_BZ 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQDM 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, 720, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQLB 证券类别 Char 2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = Fix_GetItem( api_session, 722, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQMC 证券名称 Char 8
				ret_data_json["order_id"] = Fix_GetLong( api_session, 681, i ); // FID_WTH 委托号 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["trans_id"] = Fix_GetItem( api_session, 522, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_CJBH 成交编号 Char 16
				ret_data_json["exch_side"] = Fix_GetLong( api_session, 683, i ); // FID_WTLB 委托类别 Int
				ret_data_json["fill_price"] = Fix_GetDouble( api_session, 525, i ); // FID_CJJG 成交价格 Numric 9,3
				ret_data_json["fill_amount"] = Fix_GetLong( api_session, 528, i ); // FID_CJSL 成交数量 Int
				ret_data_json["fill_money"] = Fix_GetDouble( api_session, 524, i ); // FID_CJJE 成交金额 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["cxl_flag"] = Fix_GetItem( api_session, 755, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_CXBZ 撤销标志 Char 1
				ret_data_json["settlement"] = Fix_GetDouble( api_session, 646, i ); // FID_QSJE 清算金额 Numric 16,2
				ret_data_json["commission"] = Fix_GetDouble( api_session, 766, i ); // FID_S1 佣金 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["brow_index"] = Fix_GetItem( api_session, 763, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_BROWINDEX 增量查询索引值 Char 128
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, 716, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZJZH 资金账号 Char 20
				// FID_HBXH   回报序号    Int
				// FID_LXBJ   利息报价    Numric  16,9
				// FID_SBWTH  申报委托号  Char    10
				// FID_LX     利息        Numric  16,2
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_name"] = Fix_GetItem( api_session, 9130, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JJMC ETF基金名称 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_id_1"] = Fix_GetItem( api_session, 1079, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_SGDM ETF申赎代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_id_2"] = Fix_GetItem( api_session, 9129, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JJDM ETF基金代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JYS 交易所 Char 2
				ret_data_json["count"] = Fix_GetLong( api_session, 788, i ); // FID_COUNT 股票记录数 Int
				ret_data_json["status"] = Fix_GetLong( api_session, 9133, i ); // FID_SGSHZT 申赎允许状态 Int
				ret_data_json["pub_iopv"] = Fix_GetLong( api_session, 739, i ); // FID_LOGICAL 是否发布IOPV Int
				ret_data_json["unit"] = Fix_GetLong( api_session, 1061, i ); // FID_TZDW 最小申购赎回单位 Int
				ret_data_json["cash_ratio"] = Fix_GetDouble( api_session, 9131, i ); // FID_XJTDBL 最大现金替代比例 Numeric 7,5
				ret_data_json["cash_diff"] = Fix_GetDouble( api_session, 9136, i ); // FID_XJCE T日现金差额 Numeric 11,2
				ret_data_json["iopv"] = Fix_GetDouble( api_session, 9138, i ); // FID_DWJZ T-1日基金单位净值 Numeric 8,4
				ret_data_json["trade_iopv"] = Fix_GetDouble( api_session, 9139, i ); // FID_SGSHDWJZ T-1日申赎单位净值 Numeric 12,2
				// FID_KSRQ    网下现金认购开始日期  Int
				// FID_RGDM    认购代码              Char 6
				// FID_RGQRDM  认购确认代码          Char 6
				// FID_RQ      ETF网上认购日期       Int
				// FID_XJDM    现金代码(资金代码)    Char 6
				// FID_JSRQ    网下现金认购结束日期  Int
				// FID_WJLJ    文件路径              Char 255
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
				ret_data_json["otc_code"] = Fix_GetLong( api_session, 507, i ); // FID_CODE 返回码 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = Fix_GetItem( api_session, 508, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_MESSAGE 返回说明 Char 255
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_name"] = Fix_GetItem( api_session, 9129, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JJDM ETF基金代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["stock_code"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQDM ETF成份股代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["stock_name"] = Fix_GetItem( api_session, 722, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_ZQMC ETF成份股名称 Char 8
				ret_data_json["stock_qty"] = Fix_GetLong( api_session, 724, i ); // FID_ZQSL ETF成份股数量 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT, i ); // FID_JYS 交易所 Char 2
				ret_data_json["replace_flag"] = Fix_GetLong( api_session, 9134, i ); // FID_TDBZ 替代标志 Int
				ret_data_json["replace_money"] = Fix_GetDouble( api_session, 9137, i ); // FID_TDJE 替代金额  Numeric 16,2
				ret_data_json["up_px_ratio"] = Fix_GetDouble( api_session, 1603, i ); // FID_YJBL 溢价比例  Numeric 16,2
				results_json["ret_data"].append( ret_data_json );
			}
			results = m_json_writer.write( results_json );
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
		results_json["order_id"] = Fix_GetLong( api_session, 681 ); // FID_WTH 委托号 Int
		results_json["exch_side"] = Fix_GetLong( api_session, 683 ); // FID_WTLB 委托类别 Int
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["symbol"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT ); // FID_ZQDM 证券代码 Char 6
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["security_type"] = Fix_GetItem( api_session, 720, m_field_value_short, FIELD_VALUE_SHORT ); // FID_ZQLB 证券类别 Char 2
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT ); // FID_JYS 交易所 Char 2
		results_json["cxl_qty"] = Fix_GetLong( api_session, 886 ); // FID_CDSL 撤单数量 Int
		results_json["commit_ret"] = Fix_GetLong( api_session, 753 ); // FID_SBJG 申报结果 Int
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["commit_msg"] = Fix_GetItem( api_session, 830, m_field_value_short, FIELD_VALUE_SHORT ); // FID_JGSM 结果说明 Char 64
		// FID_QRBZ   确认标志        Int
		// FID_GDH    股东号          Char     10
		// FID_BZ     币种            Char     3
		// FID_CXBZ   撤销标志        Char     1
		// FID_DJZJ   冻结资金        Numeric  16,2
		results = m_json_writer_sb.write( results_json );
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
		results_json["order_id"] = Fix_GetLong( api_session, 681 ); // FID_WTH 委托号 Int
		results_json["exch_side"] = Fix_GetLong( api_session, 683 ); // FID_WTLB 委托类别 Int
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["trans_id"] = Fix_GetItem( api_session, 522, m_field_value_short, FIELD_VALUE_SHORT ); // FID_CJBH 成交编号 Char 16
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["symbol"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT ); // FID_ZQDM 证券代码 Char 6
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["security_type"] = Fix_GetItem( api_session, 720, m_field_value_short, FIELD_VALUE_SHORT ); // FID_ZQLB 证券类别 Char 2
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT ); // FID_JYS 交易所 Char 2
		results_json["fill_qty"] = Fix_GetLong( api_session, 528 ); // FID_CJSL 本次成交数量 Int
		results_json["fill_price"] = Fix_GetDouble( api_session, 525 ); // FID_CJJG 本次成交价格 Numeric 9,3
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["fill_time"] = Fix_GetItem( api_session, 527, m_field_value_short, FIELD_VALUE_SHORT ); // FID_CJSJ 成交时间 Char 8
		results_json["cxl_qty"] = Fix_GetLong( api_session, 886 ); // FID_CDSL 撤单数量 Int
		// FID_GDH    股东号          Char     10
		// FID_BZ     币种            Char     3
		// FID_CXBZ   撤销标志        Char     1
		// FID_QSZJ   清算资金        Numeric  16,2
		// FID_ZCJSL  委托总成交数量  Int
		// FID_ZCJJE  委托总成交金额  Numeric  16,2
		// FID_CJJE   本次成交金额    Numeric  16,2
		results = m_json_writer_cj.write( results_json );
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
		results_json["order_id"] = Fix_GetLong( api_session, 681 ); // FID_WTH 委托号 Int
		results_json["exch_side"] = Fix_GetLong( api_session, 683 ); // FID_WTLB 委托类别 Int
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["symbol"] = Fix_GetItem( api_session, 719, m_field_value_short, FIELD_VALUE_SHORT ); // FID_ZQDM 证券代码 Char 6
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["security_type"] = Fix_GetItem( api_session, 720, m_field_value_short, FIELD_VALUE_SHORT ); // FID_ZQLB 证券类别 Char 2
		memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
		results_json["exchange"] = Fix_GetItem( api_session, 599, m_field_value_short, FIELD_VALUE_SHORT ); // FID_JYS 交易所 Char 2
		results_json["cxl_qty"] = Fix_GetLong( api_session, 886 ); // FID_CDSL 撤单数量 Int
		results_json["total_fill_qty"] = Fix_GetLong( api_session, 528 ); // FID_CJSL 成交数量 Int
		// FID_GDH    股东号          Char     10
		// FID_BZ     币种            Char     3
		// FID_CXBZ   撤销标志        Char     1
		// FID_DJZJ   冻结资金        Numeric  16,2
		results = m_json_writer_cd.write( results_json );
		return true;
	}
	catch( ... ) {
		return false;
	}
	return false;
}
