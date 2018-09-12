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

#include "get_field_ape.h"

GetField::GetField() {
	m_syslog = basicx::SysLog_S::GetInstance();

	m_map_get_field_func[120001] = &GetField::GetField_120001_204501;
	m_map_get_field_func[120002] = &GetField::GetField_120002_204502;
	m_map_get_field_func[120003] = &GetField::GetField_120003_204513;
	m_map_get_field_func[120004] = &GetField::GetField_120004_204511;
	m_map_get_field_func[120005] = &GetField::GetField_120005_204545;
	m_map_get_field_func[120006] = &GetField::GetField_120006_204546;
	m_map_get_field_func[130002] = &GetField::GetField_130002_303002;
	m_map_get_field_func[130004] = &GetField::GetField_130004_304101;
	m_map_get_field_func[130005] = &GetField::GetField_130005_304103;
	m_map_get_field_func[130006] = &GetField::GetField_130006_304110; // 应使用 304110 而非 304109 后者 security_type、trans_id 无值
	m_map_get_field_func[130008] = &GetField::GetField_130008_104105;
	m_map_get_field_func[130009] = &GetField::GetField_130009_104106;
}

GetField::~GetField() {
}

// 即使与 VIP 不同，业务执行出错时 ret_numb == 1，但不影响 FillHead() 中 ret_numb 填值，也不影响下单撤单函数中 int32_t i = ret_numb == 0 ? -1 : 0; 语句

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

bool GetField::GetField_120001_204501( int32_t api_session, Request* request, std::string& results ) { // 单个委托下单 // 出参同 VIP 接口
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 与 VIP 不同，业务执行出错时 ret_numb == 1
			FillHead( results_json, 120001, ret_numb, request ); // 120001 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int // 在风控 CheckTradeResultForRisk() 中检查
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号 Int
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

bool GetField::GetField_120002_204502( int32_t api_session, Request* request, std::string& results ) { // 单个委托撤单 // 出参相比 VIP 接口少了 FID_WTH
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 与 VIP 不同，业务执行出错时 ret_numb == 1
			FillHead( results_json, 120002, ret_numb, request ); // 120002 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int // 在风控 CheckTradeResultForRisk() 中检查
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号(撤单) Int // 测试：0
//				ret_data_json["order_id"] = 0;
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

bool GetField::GetField_120003_204513( int32_t api_session, Request* request, std::string& results ) { // 批量委托下单 // 出参同 VIP 接口
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 与 VIP 不同，业务执行出错时 ret_numb == 1
			FillHead( results_json, 120003, ret_numb, request ); // 120003 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int // 在风控 CheckTradeResultForRisk() 中检查
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

bool GetField::GetField_120004_204511( int32_t api_session, Request* request, std::string& results ) { // 批量委托撤单 // 出参同 VIP 接口
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 与 VIP 不同，业务执行出错时 ret_numb == 1
			FillHead( results_json, 120004, ret_numb, request ); // 120004 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int // 在风控 CheckTradeResultForRisk() 中检查
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				// FID_SBBS 1297 N 成功申请撤单的笔数
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

bool GetField::GetField_120005_204545( int32_t api_session, Request* request, std::string& results ) { // 港股通买卖委托 // 与 GetField_120005_204545 单个委托下单 一致
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 与 VIP 不同，业务执行出错时 ret_numb == 1
			FillHead( results_json, 120005, ret_numb, request ); // 120005 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int // 在风控 CheckTradeResultForRisk() 中检查
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号 Int
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

bool GetField::GetField_120006_204546( int32_t api_session, Request* request, std::string& results ) { // 港股通委托撤单 // 与 GetField_120006_204546 单个委托撤单 一致
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 与 VIP 不同，业务执行出错时 ret_numb == 1
			FillHead( results_json, 120006, ret_numb, request ); // 120006 // FillHead
			int32_t i = ret_numb == 0 ? -1 : 0; // 交易，业务失败则取第一行
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int // 在风控 CheckTradeResultForRisk() 中检查
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号(撤单) Int // 测试：0
//				ret_data_json["order_id"] = 0;
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

bool GetField::GetField_130002_303002( int32_t api_session, Request* request, std::string& results ) { // 查询客户资金 // 出参相比 VIP 接口少了 FID_CODE、FID_MESSAGE
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130002, ret_numb, request ); // 130002 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
//				ret_data_json["otc_code"] = 0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
//				ret_data_json["otc_info"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, FID_ZJZH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 资金账号 Char 20
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char  3
				ret_data_json["available"] = Fix_GetDouble( api_session, FID_KYZJ, i ); // 可用资金 Numric 16,2 // FID_EXFLG = 1 时有效
				ret_data_json["balance"] = Fix_GetDouble( api_session, FID_ZHYE, i ); // 账户余额 Numric 16,2
				ret_data_json["frozen"] = Fix_GetDouble( api_session, FID_DJJE, i ); // 冻结金额 Numric 16,2
				// FID_ZZHBZ 727 N 主账户标志 // 1 表示主账户
				// FID_ZHLB 929 N 账户类别 // 1 普通帐户，2 融资融券帐户
				// FID_ZHZT 710 N 账户状态 // 0 正常，其它为异常
				// FID_KQZJ 617 R 可取资金 // FID_EXFLG = 1 时有效
				// FID_OFSS_JZ 2100 R 基金市值 // FID_EXFLG = 1 时有效
				// FID_ZXSZ 760 R	 证券市值 // FID_EXFLG = 1 时有效
				// FID_QTZC 1254 R 其它资产 // FID_EXFLG = 1 时有效
				// FID_ZZC 1255 R 总资产 // FID_EXFLG = 1 时有效
				// FID_KYZJ2 1656 R 港股可用资金 // FID_EXFLG = 1 时有效
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

bool GetField::GetField_130004_304101( int32_t api_session, Request* request, std::string& results ) { // 查询客户持仓 // 出参相比 VIP 接口少了 FID_CODE、FID_MESSAGE、FID_ZQLB、FID_DRMCCJJE、FID_DRMRCJJE、FID_SGCJSL、FID_SHCJSL
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130004, ret_numb, request ); // 130004 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
//				ret_data_json["otc_code"] = 0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
//				ret_data_json["otc_info"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, FID_GDH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所编码 Char 2 // "SH"，"SZ"，"HK"
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券类别 Char 2 // 测试：可正常获取
//				ret_data_json["security_type"] = "A0";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 证券名称 Char 8
				ret_data_json["security_qty"] = Fix_GetLong( api_session, FID_JCCL, i ); // 今持仓量 Long //不要用证券数量
				ret_data_json["can_sell"] = Fix_GetLong( api_session, FID_KMCSL, i ); // 可卖出数量 Long
				ret_data_json["can_sub"] = Fix_GetLong( api_session, FID_KSGSL, i ); // 可申购数量 Long
				ret_data_json["can_red"] = Fix_GetLong( api_session, FID_KSHSL, i ); // 可赎回数量 Long
				ret_data_json["non_tradable"] = Fix_GetLong( api_session, FID_FLTSL, i ); // 非流通数量 Long
				ret_data_json["frozen_qty"] = Fix_GetLong( api_session, FID_DJSL, i ); // 冻结数量 Long
				ret_data_json["sell_qty"] = Fix_GetLong( api_session, FID_DRMCCJSL, i ); // 当日卖出成交数量 Long
				ret_data_json["sell_money"] = Fix_GetDouble( api_session, FID_DRMCCJJE, i ); // 当日卖出成交金额 Numric 16,2 // 测试：可正常获取
//				ret_data_json["sell_money"] = 0.0;
				ret_data_json["buy_qty"] = Fix_GetLong( api_session, FID_DRMRCJSL, i ); // 当日买入成交数量 Long
				ret_data_json["buy_money"] = Fix_GetDouble( api_session, FID_DRMRCJJE, i ); // 当日买入成交金额 Numric 16,2 // 测试：可正常获取
//				ret_data_json["buy_money"] = 0.0;
				ret_data_json["sub_qty"] = Fix_GetLong( api_session, FID_SGCJSL, i ); // 申购成交数量 Long // 测试：尚未测试
//				ret_data_json["sub_qty"] = 0;
				ret_data_json["red_qty"] = Fix_GetLong( api_session, FID_SHCJSL, i ); // 赎回成交数量 Long // 测试：尚未测试
//				ret_data_json["red_qty"] = 0;
				// FID_ZQSL 724 N 数量
				// FID_DRMCWTSL 543 N 当天卖出冻结
				// FID_WJSSL 758 N 非交收数量
				// FID_JGSL 1722 N 已交收数量
				// FID_KCRQ 603 N 开仓日期
				// FID_ZXSZ 760 R 最新市值 // FID_EXFLG = 1 时有效
				// FID_JYDW 591 N 数量单位，对债券有效，1 表示张，10 表示手 // FID_EXFLG = 1 时有效
				// FID_ZXJ 729 R 最新价 // FID_EXFLG = 1 时有效
				// FID_LXBJ 961 R 利息报价，对债券有效 // FID_EXFLG = 1 时有效
				// FID_MRJJ 636 R 买入均价 // FID_EXFLG = 1 时有效
				// FID_CCJJ 1209 R 成本价(卖出不影响成本价的算法)，成本算法一 // FID_EXFLG = 1 时有效
				// FID_BBJ 1098 R 保本价(考虑了按当前渠道卖出的费用) // FID_EXFLG = 1 时有效
				// FID_FDYK 761 R 浮动盈亏(市价与保本价的计算结果) // FID_EXFLG = 1 时有效
				// FID_LJYK 1210 R 累计盈亏(未卖出的浮动盈亏+卖出部分实现的盈亏) // FID_EXFLG = 1 时有效
				// FID_TBCBJ 671 R 摊薄成本价(卖出要摊薄成本的算法)，成本算法二 // FID_EXFLG = 1 时有效
				// FID_TBBBJ 748 R 摊薄保本价(考虑了当前渠道卖出的费用) // FID_EXFLG = 1 时有效
				// FID_TBFDYK 749 R 摊薄浮动盈亏 // FID_EXFLG = 1 时有效
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

bool GetField::GetField_130005_304103( int32_t api_session, Request* request, std::string& results ) { // 查询客户当日委托 // 出参相比 VIP 接口少了 FID_CODE、FID_MESSAGE、FID_ZQLB、FID_DJZJ、FID_CJSJ、FID_ZJZH
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130005, ret_numb, request ); // 130005 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
//				ret_data_json["otc_code"] = 0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
//				ret_data_json["otc_info"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, FID_GDH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所编码 Char 2 // "SH"，"SZ"，"HK"
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券类别 Char 2 // 测试：可正常获取
//				ret_data_json["security_type"] = "A0";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 证券名称 Char 8
				ret_data_json["order_id"] = Fix_GetLong( api_session, FID_WTH, i ); // 委托号 Int
				// 这里没有 FID_DDJYXZ 订单交易指令限制 字段，港股委托查询得到的 FID_DDLX 均为零，所以港股统一定为 限价 订单
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
				ret_data_json["frozen"] = Fix_GetDouble( api_session, FID_DJZJ, i ); // 冻结资金 Numric 16,2 // 测试：可正常获取
//				ret_data_json["frozen"] = 0.0;
				ret_data_json["settlement"] = Fix_GetDouble( api_session, FID_QSZJ, i ); // 清算资金 Numric 16,2
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["brow_index"] = Fix_GetItem( api_session, FID_BROWINDEX, m_field_value_short, FIELD_VALUE_SHORT, i ); // 增量查询索引值 Char 128
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["report_time"] = Fix_GetItem( api_session, FID_SBSJ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 申报时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["order_time"] = Fix_GetItem( api_session, FID_WTSJ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 委托时间 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fill_time"] = Fix_GetItem( api_session, FID_CJSJ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 成交时间 Char 8 // 测试：可正常获取
//				ret_data_json["fill_time"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, FID_ZJZH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 资金账号 Char 20 // 测试：""
//				ret_data_json["account"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["message"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_JGSM, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 结果说明 Char 64
				// FID_NODE 641 C 委托站点信息
				// FID_WTPCH 1017 C 批次号
				// FID_MRSL1 843 N 投票赞成数量
				// FID_MRSL2 845 N 投票反对数量
				// FID_MRSL3 847 N 投票弃权数量
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

bool GetField::GetField_130006_304110( int32_t api_session, Request* request, std::string& results ) { // 查询客户当日成交 // 出参相比 VIP 接口少了 FID_CODE、FID_MESSAGE、FID_ZQLB、FID_S1、FID_ZJZH
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130006, ret_numb, request ); // 130006 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
//				ret_data_json["otc_code"] = 0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
//				ret_data_json["otc_info"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["holder"] = Fix_GetItem( api_session, FID_GDH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 股东号 Char 10
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所编码 Char 2 // "SH"，"SZ"，"HK"
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["currency"] = Fix_GetItem( api_session, FID_BZ, m_field_value_short, FIELD_VALUE_SHORT, i ); // 币种 Char 3
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["symbol"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["security_type"] = Fix_GetItem( api_session, FID_ZQLB, m_field_value_short, FIELD_VALUE_SHORT, i ); // 证券类别 Char 2
//				ret_data_json["security_type"] = "A0"; // 测试：可正常获取
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
				ret_data_json["commission"] = Fix_GetDouble( api_session, FID_S1, i ); // 佣金 Numric 16,2 // 测试：0.0
//				ret_data_json["commission"] = 0.0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["brow_index"] = Fix_GetItem( api_session, FID_BROWINDEX, m_field_value_short, FIELD_VALUE_SHORT, i ); // 增量查询索引值 Char 128
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["account"] = Fix_GetItem( api_session, FID_ZJZH, m_field_value_short, FIELD_VALUE_SHORT, i ); // 资金账号 Char 20 // 测试：""
//				ret_data_json["account"] = "";
				// FID_SBWTH 1011 C 申报委托号
				// FID_CJSJ 527 C 成交时间
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

bool GetField::GetField_130008_104105( int32_t api_session, Request* request, std::string& results ) { // 查询ETF基本信息 // 出参相比 VIP 接口少了 FID_CODE、FID_MESSAGE、FID_LOGICAL、FID_DWJZ
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130008, ret_numb, request ); // 130008 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
//				ret_data_json["otc_code"] = 0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
//				ret_data_json["otc_info"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_JJMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // ETF基金名称 Char 8
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_id_1"] = Fix_GetItem( api_session, FID_SGDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF申赎代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_id_2"] = Fix_GetItem( api_session, FID_JJDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF基金代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所 Char 2
				ret_data_json["count"] = Fix_GetLong( api_session, FID_COUNT, i ); // 股票记录数 Int
				ret_data_json["status"] = Fix_GetLong( api_session, FID_SGSHZT, i ); // 申赎允许状态 Int // 0 禁止申赎，1 允许申赎
				ret_data_json["pub_iopv"] = Fix_GetLong( api_session, FID_LOGICAL, i ); // 是否发布IOPV Int // 测试：可正常获取
//				ret_data_json["pub_iopv"] = 0;
				ret_data_json["unit"] = Fix_GetLong( api_session, FID_TZDW, i ); // 最小申购赎回单位 Int
				ret_data_json["cash_ratio"] = Fix_GetDouble( api_session, FID_XJTDBL, i ); // 最大现金替代比例 Numeric 7,5
				ret_data_json["cash_diff"] = Fix_GetDouble( api_session, FID_XJCE, i ); // T日现金差额 Numeric 11,2
				ret_data_json["iopv"] = Fix_GetDouble( api_session, FID_DWJZ, i ); // T-1日基金单位净值 Numeric 8,4 // 测试：0.0
//				ret_data_json["iopv"] = 0.0;
				ret_data_json["trade_iopv"] = Fix_GetDouble( api_session, FID_SGSHDWJZ, i ); // T-1日申赎单位净值 Numeric 12,2
				// FID_RGDM 1394 C 认购代码
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

bool GetField::GetField_130009_104106( int32_t api_session, Request* request, std::string& results ) { // 查询ETF成分股信息 // 出参相比 VIP 接口少了 FID_CODE、FID_MESSAGE
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			int32_t ret_numb = Fix_GetCount( api_session ); // 业务执行出错时 ret_numb == 0
			FillHeadQuery( results_json, 130009, ret_numb, request ); // 130009 // FillHeadQuery
			int32_t i = 0; // 查询，业务失败则不取结果数据
			for( ; i < ret_numb; i++ ) {
				Json::Value ret_data_json;
				ret_data_json["otc_code"] = Fix_GetLong( api_session, FID_CODE, i ); // 返回码 Int
//				ret_data_json["otc_code"] = 0;
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["otc_info"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_MESSAGE, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // 返回说明 Char 255
//				ret_data_json["otc_info"] = "";
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["fund_name"] = Fix_GetItem( api_session, FID_JJDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF基金代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["stock_code"] = Fix_GetItem( api_session, FID_ZQDM, m_field_value_short, FIELD_VALUE_SHORT, i ); // ETF成份股代码 Char 6
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["stock_name"] = basicx::StringToUTF8( Fix_GetItem( api_session, FID_ZQMC, m_field_value_short, FIELD_VALUE_SHORT, i ) ); // ETF成份股名称 Char 8
				ret_data_json["stock_qty"] = Fix_GetLong( api_session, FID_ZQSL, i ); // ETF成份股数量 Int
				memset( m_field_value_short, 0, FIELD_VALUE_SHORT );
				ret_data_json["exchange"] = Fix_GetItem( api_session, FID_JYS, m_field_value_short, FIELD_VALUE_SHORT, i ); // 交易所 Char 2
				ret_data_json["replace_flag"] = Fix_GetLong( api_session, FID_TDBZ, i ); // 替代标志 Int // 0 股票，1 可用现金，2 必须现金，3 现金溢价
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
