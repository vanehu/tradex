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

#include "set_field_ape.h"

SetField::SetField() {
	m_syslog = basicx::SysLog_S::GetInstance();

	m_map_set_field_func[120001] = &SetField::SetField_120001_204501;
	m_map_set_field_func[120002] = &SetField::SetField_120002_204502;
	m_map_set_field_func[120003] = &SetField::SetField_120003_204513;
	m_map_set_field_func[120004] = &SetField::SetField_120004_204511;
	m_map_set_field_func[120005] = &SetField::SetField_120005_204545;
	m_map_set_field_func[120006] = &SetField::SetField_120006_204546;
	m_map_set_field_func[130002] = &SetField::SetField_130002_303002;
	m_map_set_field_func[130004] = &SetField::SetField_130004_304101;
	m_map_set_field_func[130005] = &SetField::SetField_130005_304103;
	m_map_set_field_func[130006] = &SetField::SetField_130006_304110; // 应使用 304110 而非 304109 后者 security_type、trans_id 无值
	m_map_set_field_func[130008] = &SetField::SetField_130008_104105;
	m_map_set_field_func[130009] = &SetField::SetField_130009_104106;
}

SetField::~SetField() {
}

bool SetField::SetField_120001_204501( int32_t api_session, Request* request ) { // 单个委托下单 // 入参同 VIP 接口
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			// FID_JMLX 加密类型 Char // 已填充
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
			Fix_SetLong( api_session, FID_JYLB, exch_side ); // 交易类别 Int
			Fix_SetDouble( api_session, FID_WTJG, price ); // 委托价格 Numric 9,3
			Fix_SetLong( api_session, FID_WTSL, request->m_req_json["amount"].asInt() ); // 委托数量 Int
			// FID_DDYXRQ 1536 N 订单有效日期
			// FID_HGRQ 1251 N 报价回购日期
			// FID_CJBH 522 C12 成交编号
			// FID_HYH 1616 C12 自动约定号
			// FID_DFXW 1935 C6 对方席位
			// FID_DFGDH 1936 C10 对方股东号
			// FID_WTPCH 1017 N 委托批次号
			// FID_DDJYXZ 1538 N 订单交易指令限制
			// FID_DDSXXZ 1537 N 订单时效限制
			// FID_PDDM 4279 N 配对代码
			// FID_TDBZ 9134 C 特定标志
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120002_204502( int32_t api_session, Request* request ) { // 单个委托撤单 // 入参相比 VIP 接口多了 FID_GDH、FID_JYS
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
//			// FID_JYMM 交易密码 Char 16 // 已填充
//			Fix_SetString( api_session, FID_GDH, request->m_req_json["holder"].asCString() ); // 股东号 Char 10 // 接口 VIP 不需 // 测试：不填即可正常撤单
//			Fix_SetString( api_session, FID_JYS, request->m_req_json["exchange"].asCString() ); // 交易所编码 Char 2 // 接口 VIP 不需 // 测试：不填即可正常撤单
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 原委托号 Int
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120003_204513( int32_t api_session, Request* request ) { // 批量委托下单 // 入参同 VIP 接口，但目前 FID_WTFS 需验证
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			// FID_JMLX 加密类型 Char // 已填充
			Fix_SetLong( api_session, FID_COUNT, request->m_req_json["order_numb"].asInt() ); // 委托笔数 Int // 不超过 60 笔
			Fix_SetString( api_session, FID_FJXX, request->m_req_json["order_list"].asCString() ); // 委托详细信息 Char // 30 字节的倍数，最长 6000 字节：JYS(2字节) ZQDM(6字节) WTLB(1字节,1或2) WTJG(9字节,5.3) WTSL(9字节) DDLX(3字节)
			// FID_WTPCH 1017 C 整数型委托批次号
			// FID_WTFS 680 N 委托方式 // 必填？
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120004_204511( int32_t api_session, Request* request ) { // 批量委托撤单 // 入参相比 VIP 接口少了 FID_EN_WTH
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			// FID_JMLX 加密类型 Char // 已填充
			Fix_SetLong( api_session, FID_WTPCH, request->m_req_json["batch_id"].asInt() ); // 委托批次号 Int
//			Fix_SetString( api_session, FID_EN_WTH, request->m_req_json["batch_ht"].asCString() ); // 撤单委托号范围 Char 6000
			// FID_ZQDM 719 C6 证券代码
			// FID_WTLB 683 N 交易类别
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120005_204545( int32_t api_session, Request* request ) { // 港股通买卖委托 // 入参相比 VIP 接口 FID_DDLX 改用 FID_DDJYXZ
	try {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
			// FID_JYMM 交易密码 Char 16 // 已填充
			// FID_JMLX 加密类型 Char // 已填充
			Fix_SetString( api_session, FID_GDH, request->m_req_json["holder"].asCString() ); // 股东号 Char 10
			Fix_SetString( api_session, FID_ZQDM, request->m_req_json["symbol"].asCString() ); // 证券代码 Char 6
			std::string exchange = request->m_req_json["exchange"].asCString();
			Fix_SetString( api_session, FID_JYS, exchange.c_str() ); // 交易所编码 Char 2
			int32_t entr_type = 0; // 1 竞价限价盘、2 增强限价盘、3 零股买卖
			double price = 0.0;
			int32_t exch_side = request->m_req_json["exch_side"].asInt();
			if( 1 == exch_side || 2 == exch_side ) { // 1 买入、2 卖出
				entr_type = request->m_req_json["entr_type"].asInt();
				price = request->m_req_json["price"].asDouble();
			}
			Fix_SetLong( api_session, FID_DDJYXZ, entr_type ); // 订单交易指令限制 Int
			Fix_SetLong( api_session, FID_JYLB, exch_side ); // 交易类别 Int
			Fix_SetDouble( api_session, FID_WTJG, price ); // 委托价格 Numric 9,3
			Fix_SetLong( api_session, FID_WTSL, request->m_req_json["amount"].asInt() ); // 委托数量 Int
			// FID_DDLX 1013 N 订单类型
			// FID_DDYXRQ 1536 N 订单有效日期
			// FID_HGRQ 1251 N 报价回购日期
			// FID_CJBH 522 C12 成交编号
			// FID_HYH 1616 C12 自动约定号
			// FID_DFXW 1935 C6 对方席位
			// FID_DFGDH 1936 C10 对方股东号
			// FID_WTPCH 1017 N 委托批次号
			// FID_DDSXXZ 1537 N 订单时效限制
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_120006_204546( int32_t api_session, Request* request ) { // 港股通委托撤单 // 入参相比 VIP 接口多了 FID_GDH、FID_JYS
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
//			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetString( api_session, FID_GDH, request->m_req_json["holder"].asCString() ); // 股东号 Char 10 // 接口 VIP 不需 // 测试：不填会报输入参数有误
			Fix_SetString( api_session, FID_JYS, request->m_req_json["exchange"].asCString() ); // 交易所编码 Char 2 // 接口 VIP 不需 // 测试：不填会报输入参数有误
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 原委托号 Int
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130002_303002( int32_t api_session, Request* request ) { // 查询客户资金 // 入参同 VIP 接口
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
//			// FID_JYMM 交易密码 Char 16 // 已填充

			// FID_EXFLG 988 N 扩展标志 // 0 查询基本信息，1 查询扩展信息
			// FID_BZ 511 C3 币种 // RMB USD HKD
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130004_304101( int32_t api_session, Request* request ) { // 查询客户持仓 // 入参同 VIP 接口
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
//			// FID_JYMM 交易密码 Char 16 // 已填充

			// FID_GDH 股东号 Char 10
			// FID_JYS 交易所编码 Char 2
			// FID_ZQDM 证券代码 Char 6
			// FID_EXFLG 查询扩展信息标志 Int // 0 不扩展，1 查询扩展信息
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130005_304103( int32_t api_session, Request* request ) { // 查询客户当日委托 // 入参相比 VIP 接口少了 FID_WTH // 无法通过填写 order_id 由柜台过滤
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
//			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 委托号 Int // 测试：填了也不会过滤
			Fix_SetString( api_session, FID_BROWINDEX, request->m_req_json["brow_index"].asCString() ); // 增量查询索引值 Char 128
			Fix_SetString( api_session, FID_CXBZ, "O" ); // 撤销标志 Char 1 // O 委托，W 撤单，A 所有(委托和撤单) // 结果不含撤单委托
			// FID_GDH 股东号 Char 10
			// FID_JYS 交易所编码 Char 2
			// FID_WTLB 委托类别 Int
			// FID_ZQDM 证券代码 Char 6
			// FID_WTPCH 委托批次号 Int
			// FID_FLAG 查询标志（0 所有委托，1 可撤单委托）Int
			// FID_SORTTYPE 9109 N 排序方式 // 0 顺序，1 逆序
            // FID_ROWCOUNT 9110 N 查询记录数
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130006_304110( int32_t api_session, Request* request ) { // 查询客户当日成交 // 入参相比 VIP 接口少了 FID_WTH // 无法通过填写 order_id 由柜台过滤 // 也没有 FID_CXBZ 字段，客户端需根据有无成交编号做过滤
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			// FID_KHH 客户号 Char 20 // 已填充
//			// FID_JYMM 交易密码 Char 16 // 已填充
			Fix_SetLong( api_session, FID_WTH, request->m_req_json["order_id"].asInt() ); // 委托号 Int // 测试：填了也不会过滤
			Fix_SetString( api_session, FID_BROWINDEX, request->m_req_json["brow_index"].asCString() ); // 增量查询索引值 Char 128
			// FID_EN_WTH 705 C255 委托号范围，多个委托号可以用逗号分隔 // 必填？
			// FID_ROWCOUNT 9110 N 查询记录数
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}

bool SetField::SetField_130008_104105( int32_t api_session, Request* request ) { // 查询ETF基本信息 // 入参同 VIP 接口
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

bool SetField::SetField_130009_104106( int32_t api_session, Request* request ) { // 查询ETF成分股信息 // 入参同 VIP 接口
	try {
		std::string field_value = "";
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Fix_SetString( api_session, FID_JJDM, request->m_req_json["fund_id_2"].asCString() ); // 基金代码 Char 6
			return true;
		}
	}
	catch( ... ) {
		return false;
	}
	return false;
}
