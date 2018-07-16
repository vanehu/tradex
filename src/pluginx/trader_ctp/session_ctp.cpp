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

#include "session_ctp.h"
#include "trader_ctp_.h"

Session::Session( TraderCTP_P* trader_ctp_p )
	: m_session( 0 )
	, m_username( "" )
	, m_password( "" )
	, m_broker_id( "" )
	, m_query_only( false )
	, m_service_user_running( false )
	, m_user_api( nullptr )
	, m_last_rsp_is_error( false )
	, m_last_error_msg( "无错误信息。" )
	, m_request_id( 0 )
	, m_connect_ok( false )
	, m_login_ok( false )
	, m_logout_ok( false )
	, m_settle_ok( false )
	, m_front_id( 0 )
	, m_session_id( 0 )
	, m_log_cate( "<TRADER_CTP>" ) {
	m_syscfg = basicx::SysCfg_S::GetInstance();
	m_trader_ctp_p = trader_ctp_p; // 不检测是否空指针
}

Session::~Session() {
	StopServiceUser();
}

void Session::OnFrontConnected() {
	m_connect_ok = true; // 只用于首次连接成功标记
	std::string log_info = "交易前置连接成功。";
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );
}

void Session::OnFrontDisconnected( int nReason ) { // CTP 的 API 会自动重新连接
	m_connect_ok = false;
	m_login_ok = false;
	m_settle_ok = false;

	std::string log_info;
	FormatLibrary::StandardLibrary::FormatTo( log_info, "交易前置连接中断！{0}", nReason );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_warn, log_info );
}

void Session::OnHeartBeatWarning( int nTimeLapse ) {
	std::string log_info;
	FormatLibrary::StandardLibrary::FormatTo( log_info, "心跳超时警告！{0}", nTimeLapse );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_warn, log_info );
}

int32_t Session::ReqUserLogin( std::string broker_id, std::string user_id, std::string password ) {
	CThostFtdcReqUserLoginField req;
	memset( &req, 0, sizeof( req ) );
	strcpy_s( req.BrokerID, const_cast<char*>( broker_id.c_str()) ); // 经纪公司代码 char 11
	strcpy_s( req.UserID, const_cast<char*>( user_id.c_str()) ); // 用户代码 char 16
	strcpy_s( req.Password, const_cast<char*>( password.c_str()) ); // 密码 char 41
	// req.TradingDay; // 交易日 char 9
	// req.UserProductInfo; // 用户端产品信息 char 11
	// req.InterfaceProductInfo; // 接口端产品信息 char 11
	// req.ProtocolInfo; // 协议信息 char 11
	// req.MacAddress; // Mac地址 char 21
	// req.OneTimePassword; // 动态密码 char 41
	// req.ClientIPAddress; // 终端IP地址 char 16
	m_login_ok = false; // 只用于首次登录成功标记
	m_last_rsp_is_error = false;
	int32_t request_id = GetRequestID();
	return m_user_api->ReqUserLogin( &req, request_id );
}

void Session::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pRspUserLogin ) {
		m_last_error_msg = pRspInfo->ErrorMsg;
		m_last_rsp_is_error = true;
		FormatLibrary::StandardLibrary::FormatTo( log_info, "用户登录失败！{0}", m_last_error_msg );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
	else {
		m_login_ok = true; // 只用于首次登录成功标记
		m_front_id = pRspUserLogin->FrontID; // 前置编号 int
		m_session_id = pRspUserLogin->SessionID; // 会话编号 int
		// m_trader_ctp_p->m_order_ref = atoi( pRspUserLogin->MaxOrderRef ); // 最大报单引用 char 13 // 每次新建会话 CTP 都会重置 OrderRef 为 1 所以这里以自己计数为准
		// pRspUserLogin->TradingDay; // 交易日 char 9
		// pRspUserLogin->LoginTime; // 登录成功时间 char 9
		// pRspUserLogin->BrokerID; // 经纪公司代码 char 11
		// pRspUserLogin->UserID; // 用户代码 char 16
		// pRspUserLogin->SystemName; // 交易系统名称 char 41
		// pRspUserLogin->SHFETime; // 上期所时间 char 9
		// pRspUserLogin->DCETime; // 大商所时间 char 9
		// pRspUserLogin->CZCETime; // 郑商所时间 char 9
		// pRspUserLogin->FFEXTime; // 中金所时间 char 9
		FormatLibrary::StandardLibrary::FormatTo( log_info, "用户登录成功。当前交易日：{0}", pRspUserLogin->TradingDay );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );
	}
}

int32_t Session::ReqUserLogout() {
	CThostFtdcUserLogoutField req;
	memset( &req, 0, sizeof( req ) );
	strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
	strcpy_s( req.UserID, const_cast<char*>(m_username.c_str()) ); // 用户代码 char 16
	m_logout_ok = false; // 只用于末次登出成功标记
	m_last_rsp_is_error = false;
	int32_t request_id = GetRequestID();
	return m_user_api->ReqUserLogout( &req, request_id );
}

void Session::OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pUserLogout ) {
		m_last_error_msg = pRspInfo->ErrorMsg;
		m_last_rsp_is_error = true;
		FormatLibrary::StandardLibrary::FormatTo( log_info, "用户登出失败！{0}", m_last_error_msg );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
	else {
		m_logout_ok = true; // 只用于末次登出成功标记
		// pUserLogout->BrokerID; // 经纪公司代码 char 11
		// pUserLogout->UserID; // 用户代码 char 16
		FormatLibrary::StandardLibrary::FormatTo( log_info, "用户登出成功。{0} {1}", pUserLogout->BrokerID, pUserLogout->UserID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );
	}
}

int32_t Session::ReqSettlementInfoConfirm() {
	CThostFtdcSettlementInfoConfirmField req;
	memset( &req, 0, sizeof( req ) );
	strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
	strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
	// req.ConfirmDate; // 确认日期 char 9
	// req.ConfirmTime; // 确认时间 char 9
	m_settle_ok = false; // 只用于首次结算确认成功标记
	m_last_rsp_is_error = false;
	int32_t request_id = GetRequestID();
	return m_user_api->ReqSettlementInfoConfirm( &req, request_id );
}

void Session::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pSettlementInfoConfirm ) {
		m_last_error_msg = pRspInfo->ErrorMsg;
		m_last_rsp_is_error = true;
		FormatLibrary::StandardLibrary::FormatTo( log_info, "结算单确认失败！{0}", m_last_error_msg );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
	else {
		m_settle_ok = true; // 只用于首次结算确认成功标记
		// pSettlementInfoConfirm->BrokerID; // 经纪公司代码 char 11
		// pSettlementInfoConfirm->InvestorID; // 投资者代码 char 13
		// pSettlementInfoConfirm->ConfirmDate; // 确认日期 char 9
		// pSettlementInfoConfirm->ConfirmTime; // 确认时间 char 9
		FormatLibrary::StandardLibrary::FormatTo( log_info, "结算单确认成功。{0} {1} {2} {3}", 
			pSettlementInfoConfirm->BrokerID, pSettlementInfoConfirm->InvestorID, pSettlementInfoConfirm->ConfirmDate, pSettlementInfoConfirm->ConfirmTime );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );
	}
}

// 目前不实现触发单功能，一般通过策略条件执行，需要时再加吧
// 触发单：ContingentCondition(用户设定触发条件)、StopPrice(用户设定止损价)、OrderPriceType(THOST_FTDC_OPT_Li mitPrice)、LimitPrice(用户设定价格)、TimeCondition(THOST_FTDC_TC_GFD)
// 触发单的止赢是根据哪个价格变量？StopPrice？触发单是否通过 ReqParkedOrderInsert 和 ReqParkedOrderAction 进行？
// 测试时发现 Q7 客户端下的委托其 OrderRef 直接是长度 12 的字符串，前面无数字的部分补空格的
int32_t Session::ReqOrderInsert( Request* request, bool is_arbitrage ) {
	try {
		std::string instrument = "";
		double price = 0.0;
		int32_t amount = 0;
		int32_t entr_type = 0;
		int32_t exch_side = 0;
		int32_t offset = 0;
		int32_t hedge = 0;

		if( NW_MSG_CODE_JSON == request->m_code ) {
			instrument = request->m_req_json["instrument"].asCString();
			price = request->m_req_json["price"].asDouble();
			amount = request->m_req_json["amount"].asInt();
			entr_type = request->m_req_json["entr_type"].asInt();
			exch_side = request->m_req_json["exch_side"].asInt();
			offset = request->m_req_json["offset"].asInt();
			hedge = request->m_req_json["hedge"].asInt();
		}

		CThostFtdcInputOrderField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
		strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
		std::string order_ref = "";
		FormatLibrary::StandardLibrary::FormatTo( order_ref, "{0}", m_trader_ctp_p->GetOrderRef() );
		strcpy_s( req.OrderRef, const_cast<char*>( order_ref.c_str()) ); // 报单引用 char 13 // 不填则由 CTP 自动生成
		strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31
		req.VolumeTotalOriginal = amount; // 数量 int
		if( false == is_arbitrage ) { // 单个合约
			if( 1 == entr_type ) { // 委托方式，限价
				req.LimitPrice = price; // 委托价格 double
				req.OrderPriceType = THOST_FTDC_OPT_LimitPrice; // 报单价格条件 char // 限价
				req.TimeCondition = THOST_FTDC_TC_GFD; // 有效期类型 char // 当日有效
			}
			else if( 2 == entr_type ) { // 委托方式，市价
				req.LimitPrice = 0.0; // 委托价格 double
				req.OrderPriceType = THOST_FTDC_OPT_AnyPrice; // 报单价格条件 char // 市价
				req.TimeCondition = THOST_FTDC_TC_IOC; // 有效期类型 char // 立即完成，否则撤销
			}
			req.IsSwapOrder = 0; // 互换单标志 int
		}
		else { // 组合合约 // 只能限价
			req.LimitPrice = price; // 委托价格 double
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice; // 报单价格条件 char // 限价
			req.TimeCondition = THOST_FTDC_TC_GFD; // 有效期类型 char // 当日有效
			req.IsSwapOrder = ( 7 == entr_type ? 1 : 0 ); // 互换单标志 int // entr_type：郑商所：8 跨期套利，9 跨品种套利，大商所：2 套利订单，7 互换订单
		}
		// 交易类型：买入、卖出
		char c_direct[] = { THOST_FTDC_D_Buy, THOST_FTDC_D_Sell };
		if( exch_side >= 1 && exch_side <= 2 ) {
			req.Direction = c_direct[exch_side - 1]; // 买卖方向 char
		}
		// 在 CTP 层面，SR505&SR509 或 SPC y1505&p1505 这样的合约开仓平仓，CombOffsetFlag 字段也是当做单腿的填写，只是互换时需要指明互换标志，套利时跟单腿完全一样
		// 开平方向：开仓、平仓、强平、平今、平昨、强减、本地强平，上期所平今仓须为：THOST_FTDC_OF_CloseToday，其他均为：THOST_FTDC_OF_Close
		char c_offset[] = { THOST_FTDC_OF_Open, THOST_FTDC_OF_Close, THOST_FTDC_OF_ForceClose, THOST_FTDC_OF_CloseToday, THOST_FTDC_OF_CloseYesterday, THOST_FTDC_OF_ForceOff, THOST_FTDC_OF_LocalForceClose };
		if( offset >= 1 && offset <= 7 ) {
			req.CombOffsetFlag[0] = c_offset[offset - 1]; // 组合开平标志 char 5
		}
		// 投机套保：投机、套利、套保
		char c_hedge[] = { THOST_FTDC_HF_Speculation, THOST_FTDC_HF_Arbitrage, THOST_FTDC_HF_Hedge };
		if( hedge >= 1 && hedge <= 3 ) {
			req.CombHedgeFlag[0] = c_hedge[hedge - 1]; // 组合投机套保标志 char 5
		}
		req.VolumeCondition = THOST_FTDC_VC_AV; // 成交量类型 char // 任何数量
		req.MinVolume = 1; // 最小成交量 int // 1
		req.ContingentCondition = THOST_FTDC_CC_Immediately; // 触发条件 char // 立即
		req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; // 强平原因 char // 非强平
		req.IsAutoSuspend = 0; // 自动挂起标志 int // 否
		req.UserForceClose = 0; // 用户强平标志 int // 否
		// req.UserID; // 用户代码 char 16
		// req.StopPrice; // 止损价 double
		// req.GTDDate; // GTD日期 char 9
		// req.BusinessUnit; // 业务单元 char 21
		int32_t request_id = GetRequestID();
		req.RequestID = request_id; // 请求编号 int // 填一下吧，确保报单回报中存在此值
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqOrderInsert( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

// 用户报单时，如果 CTP 参数校验正确，则不会收到 OnRspOrderInsert 消息，而是直接 OnRtnOrder 回报，只有报单被 CTP 拒绝才会收到 OnRspOrderInsert 消息
// 如果交易所认为报单错误，用户就会收到 OnErrRtnOrderInsert 消息
void Session::OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			results_json["ret_func"] = request->m_req_json["function"].asInt();
			results_json["ret_task"] = request->m_req_json["task_id"].asInt();
			results_json["ret_last"] = bIsLast;
			//results_json["ret_data"] = "";

			if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pInputOrder ) {
				if( pRspInfo ) {
					FormatLibrary::StandardLibrary::FormatTo( log_info, "报单提交失败！{0}", pRspInfo->ErrorMsg );
				}
				else {
					log_info = "报单提交失败！原因未知！";
				}
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
			}
			else {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "报单提交成功。{0}", pInputOrder->OrderRef );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 1;

				Json::Value ret_data_json;
				ret_data_json["otc_code"] = 0;
				ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
				ret_data_json["order_id"] = pInputOrder->OrderRef; // 报单引用 char 13
				// pInputOrder->BrokerID; // 经纪公司代码 char 11
				// pInputOrder->InvestorID; // 投资者代码 char 13
				// pInputOrder->InstrumentID; // 合约代码 char 31
				// pInputOrder->UserID; // 用户代码 char 16
				// pInputOrder->OrderPriceType; // 报单价格条件 char
				// pInputOrder->Direction; // 买卖方向 char
				// pInputOrder->CombOffsetFlag; // 组合开平标志 char 5
				// pInputOrder->CombHedgeFlag; // 组合投机套保标志 char 5
				// pInputOrder->LimitPrice; // 价格 double
				// pInputOrder->VolumeTotalOriginal; // 数量 int
				// pInputOrder->TimeCondition; // 有效期类型 char
				// pInputOrder->GTDDate; // GTD日期 char 9
				// pInputOrder->VolumeCondition; // 成交量类型 char
				// pInputOrder->MinVolume; // 最小成交量 int
				// pInputOrder->ContingentCondition; // 触发条件 char
				// pInputOrder->StopPrice; // 止损价 double
				// pInputOrder->ForceCloseReason; // 强平原因 char
				// pInputOrder->IsAutoSuspend; // 自动挂起标志 int
				// pInputOrder->BusinessUnit; // 业务单元 char 21
				// pInputOrder->RequestID; // 请求编号 int
				// pInputOrder->UserForceClose; // 用户强评标志 int
				// pInputOrder->IsSwapOrder; // 互换单标志 int
				results_json["ret_data"].append( ret_data_json );
			}

			result_data = Json::writeString( m_json_writer, results_json );
		}

		m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "报单回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

void Session::OnErrRtnOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo ) { // 报单到交易所后发生校验错误应该极少发生
	std::string log_info;

	if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pInputOrder ) {
		if( pRspInfo ) {
			FormatLibrary::StandardLibrary::FormatTo( log_info, "报单提交交易所失败！{0}", pRspInfo->ErrorMsg );
		}
		else {
			log_info = "报单提交交易所失败！原因未知！";
		}
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
	else {
		std::string result_data = "";
		Request* request = GetRequestByID( pInputOrder->RequestID );

		if( request != nullptr ) {
			if( NW_MSG_CODE_JSON == request->m_code ) {
				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( "报单提交交易所失败！" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";

				if( pRspInfo && pRspInfo->ErrorID != 0 ) {
					if( pRspInfo ) {
						FormatLibrary::StandardLibrary::FormatTo( log_info, "报单提交交易所失败！{0}", pRspInfo->ErrorMsg );
					}
					else {
						log_info = "报单提交交易所失败！原因未知！";
					}
					m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

					results_json["ret_info"] = basicx::StringToUTF8( log_info );
				}

				result_data = Json::writeString( m_json_writer, results_json );
			}

			m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
		}
		else {
			FormatLibrary::StandardLibrary::FormatTo( log_info, "交易所报单错误回调查找失败！{0}", pInputOrder->RequestID );
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
		}
	}
}

// 下单以后，在 CTP 将委托报到交易所以前，可以通过 FrontID、SessionID、OrderRef 三者从 CTP 撤单
// 在 CTP 将委托报到交易所之后，则要根据 ExchangeID、OrderSysID 两个信息撤单，否则会报找不到要撤单委托的错误
// 如果要完成 CTP 撤单，因为客户端尚未收到报单回报故无 OrderRef 信息，则需要撤单请求给出下单时的 task_id 即下单时的 RequestID 信息，服务端据此找出缓存的对应 OrderRef 去撤单
// 考虑委托报到交易所前就撤单的情况罕见，下单后报单回报到来前就撤单的情况也极少，故目前不实现这种撤单功能，需要时再加吧
int32_t Session::ReqOrderAction( Request* request ) {
	try {
		std::string order_ref = "";

		if( NW_MSG_CODE_JSON == request->m_code ) {
			order_ref = request->m_req_json["order_id"].asCString();
		}

		CThostFtdcOrderField* order_item = GetOrderItemByID( atoi( order_ref.c_str() ) );

		CThostFtdcInputOrderActionField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
		strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
		req.OrderActionRef = m_trader_ctp_p->GetOrderRef(); // 报单操作引用 int // 不填则由 CTP 自动生成
		strcpy_s( req.OrderRef, const_cast<char*>( order_ref.c_str()) ); // 报单引用 char 13
		req.FrontID = m_front_id; // 前置编号 int
		req.SessionID = m_session_id; // 会话编号 int
		if( order_item != nullptr ) { // 如果 order_item == nullptr 就让 CTP 报找不到的错误
			strcpy_s( req.ExchangeID, order_item->ExchangeID ); // 交易所代码 char 9
			strcpy_s( req.OrderSysID, order_item->OrderSysID ); // 报单编号 char 21
		}
		req.ActionFlag = THOST_FTDC_AF_Delete; // 操作标志 char
		// req.LimitPrice; // 价格 double
		// req.VolumeChange; // 数量变化 int
		// req.UserID; // 用户代码 char 16
		// req.InstrumentID; // 合约代码 char 31
		int32_t request_id = GetRequestID();
		req.RequestID = request_id; // 请求编号 int // 填一下吧，确保报单回报中存在此值
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqOrderAction( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

// 用户撤单时，如果 CTP 参数校验正确，则不会收到 OnRspOrderAction 消息，而是直接 OnRtnOrder 回报，只有撤单被 CTP 拒绝才会收到 OnRspOrderAction 消息
// 如果交易所认为撤单错误，用户就会收到 OnErrRtnOrderAction 消息
void Session::OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			results_json["ret_func"] = request->m_req_json["function"].asInt();
			results_json["ret_task"] = request->m_req_json["task_id"].asInt();
			results_json["ret_last"] = bIsLast;
			//results_json["ret_data"] = "";

			if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pInputOrderAction ) {
				if( pRspInfo ) {
					FormatLibrary::StandardLibrary::FormatTo( log_info, "撤单提交失败！{0}", pRspInfo->ErrorMsg );
				}
				else {
					log_info = "撤单提交失败！原因未知！";
				}
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
			}
			else {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "撤单提交成功。{0}", pInputOrderAction->OrderActionRef );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 1;

				Json::Value ret_data_json;
				ret_data_json["otc_code"] = 0;
				ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
				std::string order_action_ref;
				FormatLibrary::StandardLibrary::FormatTo( order_action_ref, "{0}", pInputOrderAction->OrderActionRef ); // 报单操作引用 int
				ret_data_json["order_id"] = order_action_ref; // 用字符串形式的
				// pInputOrderAction->BrokerID; // 经纪公司代码 char 11
				// pInputOrderAction->InvestorID; // 投资者代码 char 13
				// pInputOrderAction->OrderRef; // 报单引用 char 13
				// pInputOrderAction->RequestID; // 请求编号 int
				// pInputOrderAction->FrontID; // 前置编号 int
				// pInputOrderAction->SessionID; // 会话编号 int
				// pInputOrderAction->ExchangeID; // 交易所代码 char 9
				// pInputOrderAction->OrderSysID; // 报单编号 char 21
				// pInputOrderAction->ActionFlag; // 操作标志 char
				// pInputOrderAction->LimitPrice; // 价格 double
				// pInputOrderAction->VolumeChange; // 数量变化 int
				// pInputOrderAction->UserID; // 用户代码 char 16
				// pInputOrderAction->InstrumentID; // 合约代码 char 31
				results_json["ret_data"].append( ret_data_json );
			}

			result_data = Json::writeString( m_json_writer, results_json );
		}

		m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "撤单回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

void Session::OnErrRtnOrderAction( CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo ) { // 撤单到交易所后发生校验错误应该极少发生
	std::string log_info;

	if( ( pRspInfo && pRspInfo->ErrorID != 0 ) || !pOrderAction ) {
		if( pRspInfo ) {
			FormatLibrary::StandardLibrary::FormatTo( log_info, "撤单提交交易所失败！{0}", pRspInfo->ErrorMsg );
		}
		else {
			log_info = "撤单提交交易所失败！原因未知！";
		}
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
	else {
		std::string result_data = "";
		Request* request = GetRequestByID( pOrderAction->RequestID );

		if( request != nullptr ) {
			if( NW_MSG_CODE_JSON == request->m_code ) {
				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( "撤单提交交易所失败！" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";

				if( pRspInfo && pRspInfo->ErrorID != 0 ) {
					if( pRspInfo ) {
						FormatLibrary::StandardLibrary::FormatTo( log_info, "撤单提交交易所失败！{0}", pRspInfo->ErrorMsg );
					}
					else {
						log_info = "撤单提交交易所失败！原因未知！";
					}
					m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

					results_json["ret_info"] = basicx::StringToUTF8( log_info );
				}

				result_data = Json::writeString( m_json_writer, results_json );
			}

			m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
		}
		else {
			FormatLibrary::StandardLibrary::FormatTo( log_info, "交易所撤单错误回调查找失败！{0}", pOrderAction->RequestID );
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
		}
	}
}

int32_t Session::ReqQryTradingAccount( Request* request ) {
	try {
		CThostFtdcQryTradingAccountField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
		strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
		int32_t request_id = GetRequestID();
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqQryTradingAccount( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

void Session::OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		bool no_error_and_is_last = false; // 标记是否需要结束标志消息

		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			results_json["ret_func"] = request->m_req_json["function"].asInt();
			results_json["ret_task"] = request->m_req_json["task_id"].asInt();
			//results_json["ret_data"] = "";

			if( pRspInfo && pRspInfo->ErrorID != 0 ) {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "客户资金查询提交失败！{0}", pRspInfo->ErrorMsg );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
			}
			else if( !pTradingAccount ) { // 查询无结果数据
				log_info = "客户资金查询提交成功。无结果数据。";
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "客户资金查询应答结束标记。" );
				results_json["ret_numb"] = 0;
			}
			else {
				if( true == bIsLast ) { // 需要补发结束标志消息
					no_error_and_is_last = true;
				}

				results_json["ret_last"] = false;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 1;

				Json::Value ret_data_json;
				ret_data_json["otc_code"] = 0;
				ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
				ret_data_json["account"] = pTradingAccount->AccountID; // 投资者帐号 char 13
				ret_data_json["currency"] = "RMB"; // 币种
				ret_data_json["available"] = pTradingAccount->Available; // 可用资金 double
				ret_data_json["profit"] = pTradingAccount->CloseProfit; // 平仓盈亏 double
				ret_data_json["float_profit"] = pTradingAccount->PositionProfit; // 持仓盈亏 double
				ret_data_json["margin"] = pTradingAccount->CurrMargin; // 当前保证金总额 double
				ret_data_json["frozen_margin"] = pTradingAccount->FrozenMargin; // 冻结的保证金 double
				ret_data_json["fee"] = pTradingAccount->Commission; // 手续费 double
				ret_data_json["frozen_fee"] = pTradingAccount->FrozenCommission; // 冻结的手续费 double
				// pTradingAccount->BrokerID; // 经纪公司代码 char 11
				// pTradingAccount->PreMortgage; // 上次质押金额 double
				// pTradingAccount->PreCredit; // 上次信用额度 double
				// pTradingAccount->PreDeposit; // 上次存款额 double
				// pTradingAccount->PreBalance; // 上次结算准备金 double
				// pTradingAccount->PreMargin; // 上次占用的保证金 double
				// pTradingAccount->InterestBase; // 利息基数 double
				// pTradingAccount->Interest; // 利息收入 double
				// pTradingAccount->Deposit; // 入金金额 double
				// pTradingAccount->Withdraw; // 出金金额 double
				// pTradingAccount->FrozenCash; // 冻结的资金 double
				// pTradingAccount->CashIn; // 资金差额 double
				// pTradingAccount->Balance; // 期货结算准备金 double
				// pTradingAccount->WithdrawQuota; // 可取资金 double
				// pTradingAccount->Reserve; // 基本准备金 double
				// pTradingAccount->TradingDay; // 交易日 char 9
				// pTradingAccount->SettlementID; // 结算编号 int
				// pTradingAccount->Credit; // 信用额度 double
				// pTradingAccount->Mortgage; // 质押金额 double
				// pTradingAccount->ExchangeMargin; // 交易所保证金 double
				// pTradingAccount->DeliveryMargin; // 投资者交割保证金 double
				// pTradingAccount->ExchangeDeliveryMargin; // 交易所交割保证金 double
				results_json["ret_data"].append( ret_data_json );
			}

			result_data = Json::writeString( m_json_writer, results_json );
		}

		m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );

		if( true == no_error_and_is_last ) { // 补发结束标志消息
			FormatLibrary::StandardLibrary::FormatTo( log_info, "客户资金查询提交成功。{0}", pTradingAccount->AccountID );
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

			if( NW_MSG_CODE_JSON == request->m_code ) {
				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "客户资金查询应答结束标记。" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";
				result_data = Json::writeString( m_json_writer, results_json );
			}

			m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
		}
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "客户资金查询回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

int32_t Session::ReqQryInvestorPosition( Request* request ) {
	try {
		std::string instrument = "";

		if( NW_MSG_CODE_JSON == request->m_code ) {
			instrument = request->m_req_json["instrument"].asCString();
		}

		CThostFtdcQryInvestorPositionField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
		strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
		strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31
		int32_t request_id = GetRequestID();
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqQryInvestorPosition( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

void Session::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		bool no_error_and_is_last = false; // 标记是否需要结束标志消息

		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			results_json["ret_func"] = request->m_req_json["function"].asInt();
			results_json["ret_task"] = request->m_req_json["task_id"].asInt();
			//results_json["ret_data"] = "";

			if( pRspInfo && pRspInfo->ErrorID != 0 ) {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "客户持仓查询提交失败！{0}", pRspInfo->ErrorMsg );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
			}
			else if( !pInvestorPosition ) { // 查询无结果数据
				log_info = "客户持仓查询提交成功。无结果数据。";
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "客户持仓查询应答结束标记。" );
				results_json["ret_numb"] = 0;
			}
			else {
				if( true == bIsLast ) { // 需要补发结束标志消息
					no_error_and_is_last = true;
				}

				results_json["ret_last"] = false;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 1;

				Json::Value ret_data_json;
				ret_data_json["otc_code"] = 0;
				ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
				ret_data_json["instrument"] = pInvestorPosition->InstrumentID; // 合约代码 char 31
				std::string direction( 1, pInvestorPosition->PosiDirection );
				ret_data_json["exch_side"] = atoi( direction.c_str() ) - 1; // 持仓多空方向 char // 为 TSgitFtdcPosiDirectionType 类型，'2' 多，'3' 空，故为 - 1
				ret_data_json["position"] = pInvestorPosition->Position; // 总持仓 int
				ret_data_json["tod_position"] = pInvestorPosition->TodayPosition; // 今日持仓 int
				ret_data_json["pre_position"] = pInvestorPosition->YdPosition; // 上日持仓 int
				ret_data_json["open_volume"] = pInvestorPosition->OpenVolume; // 开仓量 int
				ret_data_json["close_volume"] = pInvestorPosition->CloseVolume; // 平仓量 int
				// pInvestorPosition->BrokerID; // 经纪公司代码 char 11
				// pInvestorPosition->InvestorID; // 投资者代码 char 13
				// pInvestorPosition->HedgeFlag; // 投机套保标志 char
				// pInvestorPosition->PositionDate; // 持仓日期 char
				// pInvestorPosition->LongFrozen; // 多头冻结 int
				// pInvestorPosition->ShortFrozen; // 空头冻结 int
				// pInvestorPosition->LongFrozenAmount; // 多头冻结金额 double
				// pInvestorPosition->ShortFrozenAmount; // 空头冻结金额 double
				// pInvestorPosition->OpenAmount; // 开仓金额 double
				// pInvestorPosition->CloseAmount; // 平仓金额 double
				// pInvestorPosition->PositionCost; // 持仓成本 double
				// pInvestorPosition->PreMargin; // 上次占用的保证金 double
				// pInvestorPosition->UseMargin; // 占用的保证金 double
				// pInvestorPosition->FrozenMargin; // 冻结的保证金 double
				// pInvestorPosition->FrozenCash; // 冻结的资金 double
				// pInvestorPosition->FrozenCommission; // 冻结的手续费 double
				// pInvestorPosition->CashIn; // 资金差额 double
				// pInvestorPosition->Commission; // 手续费 double
				// pInvestorPosition->CloseProfit; // 平仓盈亏 double
				// pInvestorPosition->PositionProfit; // 持仓盈亏 double
				// pInvestorPosition->PreSettlementPrice; // 上次结算价 double
				// pInvestorPosition->SettlementPrice; // 本次结算价 double
				// pInvestorPosition->TradingDay; // 交易日 char 9
				// pInvestorPosition->SettlementID; // 结算编号 int
				// pInvestorPosition->OpenCost; // 开仓成本 double
				// pInvestorPosition->ExchangeMargin; // 交易所保证金 double
				// pInvestorPosition->CombPosition; // 组合成交形成的持仓 int
				// pInvestorPosition->CombLongFrozen; // 组合多头冻结 int
				// pInvestorPosition->CombShortFrozen; // 组合空头冻结 int
				// pInvestorPosition->CloseProfitByDate; // 逐日盯市平仓盈亏 double
				// pInvestorPosition->CloseProfitByTrade; // 逐笔对冲平仓盈亏 double
				// pInvestorPosition->MarginRateByMoney; // 保证金率 double
				// pInvestorPosition->MarginRateByVolume; // 保证金率(按手数) double
				results_json["ret_data"].append( ret_data_json );
			}

			result_data = Json::writeString( m_json_writer, results_json );
		}

		m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );

		if( true == no_error_and_is_last ) { // 补发结束标志消息
			FormatLibrary::StandardLibrary::FormatTo( log_info, "客户持仓查询提交成功。{0}", pInvestorPosition->InstrumentID );
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

			if( NW_MSG_CODE_JSON == request->m_code ) {
				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "客户持仓查询应答结束标记。" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";
				result_data = Json::writeString( m_json_writer, results_json );
			}

			m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
		}
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "客户持仓查询回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

// 用户未指定委托号，则不填 ExchangeID、OrderSysID 信息，查询所有，返回所有
// 用户指定委托号，则尝试从缓存中取得该委托号对应的 ExchangeID、OrderSysID 从而查询得到对应委托信息
// 如果缓存中没有该委托号（其他系统交易或本系统重启过），则不填 ExchangeID、OrderSysID 信息，查询所有，再在 OnRspQryOrder 中通过委托号过滤
// 可以考虑添加指定期货合约项
int32_t Session::ReqQryOrder( Request* request ) {
	try {
		std::string order_ref = "";

		if( NW_MSG_CODE_JSON == request->m_code ) {
			order_ref = request->m_req_json["order_id"].asCString();
		}

		CThostFtdcOrderField* order_item = nullptr;
		if( order_ref != "" ) { // 用户指定委托号
			order_item = GetOrderItemByID( atoi( order_ref.c_str() ) );
		}

		CThostFtdcQryOrderField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
		strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
		//strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31
		if( order_item != nullptr ) { // 如果 order_item == nullptr 则查询所有委托，否则查询单笔委托
			std::string instrument = order_item->InstrumentID;
			strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31 // 可起到过滤作用减少返回数据
			strcpy_s( req.ExchangeID, order_item->ExchangeID ); // 交易所代码 char 9
			strcpy_s( req.OrderSysID, order_item->OrderSysID ); // 报单编号 char 21
		}
		// req.InsertTimeStart; // 开始时间 char 9
		// req.InsertTimeEnd; // 结束时间 char 9
		int32_t request_id = GetRequestID();
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqQryOrder( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

void Session::OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			if( pRspInfo && pRspInfo->ErrorID != 0 ) {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "客户当日委托查询提交失败！{0}", pRspInfo->ErrorMsg );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";

				result_data = Json::writeString( m_json_writer, results_json );
				m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
			}
			else if( !pOrder ) { // 查询无结果数据
				log_info = "客户当日委托查询提交成功。无结果数据。";
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "客户当日委托查询应答结束标记。" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";

				result_data = Json::writeString( m_json_writer, results_json );
				m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
			}
			else {
				std::string order_ref_req = request->m_req_json["order_id"].asCString();
				std::string order_ref_rsp = pOrder->OrderRef;
				// 对于如 Q7 等客户端其 OrderRef 是全字符串左端补空格的，虽然数值可能相等，但这里就会被过滤掉
				if( "" == order_ref_req || order_ref_rsp == order_ref_req ) { // 过滤出用户请求的
					Json::Value results_json;
					results_json["ret_func"] = request->m_req_json["function"].asInt();
					results_json["ret_task"] = request->m_req_json["task_id"].asInt();
					results_json["ret_last"] = false;
					results_json["ret_code"] = 0;
					results_json["ret_info"] = basicx::StringToUTF8( log_info );
					results_json["ret_numb"] = 1;
					//results_json["ret_data"] = "";

					Json::Value ret_data_json;
					ret_data_json["otc_code"] = 0;
					ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
					ret_data_json["order_id"] = pOrder->OrderRef; // 报单引用 char 13
					ret_data_json["order_sys_id"] = pOrder->OrderSysID; // 报单编号 char 21
					ret_data_json["instrument"] = pOrder->InstrumentID; // 合约代码 char 31
					ret_data_json["exchange"] = pOrder->ExchangeID; // 交易所代码 char 9
					std::string direction( 1, pOrder->Direction );
					ret_data_json["exch_side"] = atoi( direction.c_str() ) + 1; // 买卖方向 char
					ret_data_json["fill_qty"] = pOrder->VolumeTraded; // 成交数量 int
					// 正常获取 n_status 的话 "status" 字段一定存在
					if( pOrder->OrderStatus != 'a' && pOrder->OrderStatus != 'b' && pOrder->OrderStatus != 'c' ) {
						// 0：全部成交，1：部分成交，2：部成部撤，3：已报未成，4：自动挂起，5：全部撤单
						int32_t status_temp[] = { 5, 4, 7, 3, 13, 8 };
						std::string s_status( 1, pOrder->OrderStatus ); // 报单状态 char
						int32_t n_status = atoi( s_status.c_str() );
						if( n_status >= 0 && n_status <= 5 ) {
							ret_data_json["status"] = status_temp[n_status];
						}
					}
					else if( 'a' == pOrder->OrderStatus ) { // 未知
						ret_data_json["status"] = 14;
					}
					else if( 'b' == pOrder->OrderStatus ) { // 尚未触发
						ret_data_json["status"] = 11;
					}
					else if( 'c' == pOrder->OrderStatus ) { // 已经触发
						ret_data_json["status"] = 12;
					}
					ret_data_json["status_msg"] = basicx::StringToUTF8( pOrder->StatusMsg ); // 状态信息 char 81
					// pOrder->RequestID; // 请求编号 int
					// pOrder->BrokerID; // 经纪公司代码 char 11
					// pOrder->InvestorID; // 投资者代码 char 13
					// pOrder->UserID; // 用户代码 char 16
					// pOrder->OrderPriceType; // 报单价格条件 char
					// pOrder->CombOffsetFlag; // 组合开平标志 char 5
					// pOrder->CombHedgeFlag; // 组合投机套保标志 char 5
					// pOrder->LimitPrice; // 价格 double
					// pOrder->VolumeTotalOriginal; // 数量 int
					// pOrder->TimeCondition; // 有效期类型 char
					// pOrder->GTDDate; // GTD日期 char 9
					// pOrder->VolumeCondition; // 成交量类型 char
					// pOrder->MinVolume; // 最小成交量 int
					// pOrder->ContingentCondition; // 触发条件 char
					// pOrder->StopPrice; // 止损价 double
					// pOrder->ForceCloseReason; // 强平原因 char
					// pOrder->IsAutoSuspend; // 自动挂起标志 int
					// pOrder->BusinessUnit; // 业务单元 char 21
					// pOrder->OrderLocalID; // 本地报单编号 char 13
					// pOrder->ParticipantID; // 会员代码 char 11
					// pOrder->ClientID; // 客户代码 char 11
					// pOrder->ExchangeInstID; // 合约在交易所的代码 char 31
					// pOrder->TraderID; // 交易所交易员代码 char 21
					// pOrder->InstallID; // 安装编号 int
					// pOrder->OrderSubmitStatus; // 报单提交状态 char
					// pOrder->NotifySequence; // 报单提示序号 int
					// pOrder->TradingDay; // 交易日 char 9
					// pOrder->SettlementID; // 结算编号 int
					// pOrder->OrderSource; // 报单来源 char
					// pOrder->OrderType; // 报单类型 char
					// pOrder->VolumeTotal; // 剩余数量 int
					// pOrder->InsertDate; // 报单日期 char 9
					// pOrder->InsertTime; // 委托时间 char 9
					// pOrder->ActiveTime; // 激活时间 char 9
					// pOrder->SuspendTime; // 挂起时间 char 9
					// pOrder->UpdateTime; // 最后修改时间 char 9
					// pOrder->CancelTime; // 撤销时间 char 9
					// pOrder->ActiveTraderID; // 最后修改交易所交易员代码 char 21
					// pOrder->ClearingPartID; // 结算会员编号 char 11
					// pOrder->SequenceNo; // 序号 int
					// pOrder->FrontID; // 前置编号 int
					// pOrder->SessionID; // 会话编号 int
					// pOrder->UserProductInfo; // 用户端产品信息 char 11
					// pOrder->UserForceClose; // 用户强评标志 int
					// pOrder->ActiveUserID; // 操作用户代码 char 16
					// pOrder->BrokerOrderSeq; // 经纪公司报单编号 int
					// pOrder->RelativeOrderSysID; // 相关报单 char 21
					// pOrder->ZCETotalTradedVolume; // 郑商所成交数量 int
					// pOrder->IsSwapOrder; // 互换单标志 int
					results_json["ret_data"].append( ret_data_json );

					result_data = Json::writeString( m_json_writer, results_json );
					m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
				}

				if( true == bIsLast ) { // 补发结束标志消息
					log_info = "客户当日委托查询提交成功。";
					m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

					Json::Value results_json;
					results_json["ret_func"] = request->m_req_json["function"].asInt();
					results_json["ret_task"] = request->m_req_json["task_id"].asInt();
					results_json["ret_last"] = true;
					results_json["ret_code"] = 0;
					results_json["ret_info"] = basicx::StringToUTF8( "客户当日委托查询应答结束标记。" );
					results_json["ret_numb"] = 0;
					//results_json["ret_data"] = "";
					result_data = Json::writeString( m_json_writer, results_json );
					m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
				}
			}
		}
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "客户当日委托查询回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

// 因为 CThostFtdcQryTradeField 中无法填 OrderRef 字段，指定 TradeID 意义不大且这样接口不统一，故只能全部查询再根据 OrderRef 过滤了
// 可以考虑添加指定期货合约项
int32_t Session::ReqQryTrade( Request* request ) {
	try {
		std::string order_ref = "";

		if( NW_MSG_CODE_JSON == request->m_code ) {
			order_ref = request->m_req_json["order_id"].asCString();
		}

		CThostFtdcOrderField* order_item = nullptr;
		if( order_ref != "" ) { // 用户指定委托号，这里其实无太大意义，因为 CThostFtdcQryTradeField 中无法填 OrderRef 字段
			order_item = GetOrderItemByID( atoi( order_ref.c_str() ) );
		}

		CThostFtdcQryTradeField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.BrokerID, const_cast<char*>(m_broker_id.c_str()) ); // 经纪公司代码 char 11
		strcpy_s( req.InvestorID, const_cast<char*>(m_username.c_str()) ); // 投资者代码 char 13
		//strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31
		if( order_item != nullptr ) { // 如果 order_item == nullptr 则查询所有委托，否则查询单笔委托（需过滤）
			std::string instrument = order_item->InstrumentID;
			strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31 // 可起到过滤作用减少返回数据
			strcpy_s( req.ExchangeID, order_item->ExchangeID ); // 交易所代码 char 9
			// req.TradeID; // 成交编号 char 21 // 这个就不填了
		}
		// req.TradeTimeStart; // 开始时间 char 9
		// req.TradeTimeEnd; // 结束时间 char 9
		int32_t request_id = GetRequestID();
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqQryTrade( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

void Session::OnRspQryTrade( CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			if( pRspInfo && pRspInfo->ErrorID != 0 ) {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "客户当日成交查询提交失败！{0}", pRspInfo->ErrorMsg );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";

				result_data = Json::writeString( m_json_writer, results_json );
				m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
			}
			else if( !pTrade ) { // 查询无结果数据
				log_info = "客户当日成交查询提交成功。无结果数据。";
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "客户当日成交查询应答结束标记。" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";

				result_data = Json::writeString( m_json_writer, results_json );
				m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
			}
			else {
				std::string order_ref_req = request->m_req_json["order_id"].asCString();
				std::string order_ref_rsp = pTrade->OrderRef;
				// 对于如 Q7 等客户端其 OrderRef 是全字符串左端补空格的，虽然数值可能相等，但这里就会被过滤掉
				if( "" == order_ref_req || order_ref_rsp == order_ref_req ) { // 过滤出用户请求的
					Json::Value results_json;
					results_json["ret_func"] = request->m_req_json["function"].asInt();
					results_json["ret_task"] = request->m_req_json["task_id"].asInt();
					results_json["ret_last"] = false;
					results_json["ret_code"] = 0;
					results_json["ret_info"] = basicx::StringToUTF8( log_info );
					results_json["ret_numb"] = 1;
					//results_json["ret_data"] = "";

					Json::Value ret_data_json;
					ret_data_json["otc_code"] = 0;
					ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
					ret_data_json["order_id"] = pTrade->OrderRef; // 报单引用 char 13
					ret_data_json["trans_id"] = pTrade->TradeID; // 成交编号 char 21
					ret_data_json["instrument"] = pTrade->InstrumentID; // 合约代码 char 31
					ret_data_json["exchange"] = pTrade->ExchangeID; // 交易所代码 char 9
					std::string direction( 1, pTrade->Direction );
					ret_data_json["exch_side"] = atoi( direction.c_str() ) + 1; // 买卖方向 char
					ret_data_json["fill_qty"] = pTrade->Volume; // 成交数量 int
					ret_data_json["fill_price"] = pTrade->Price; // 成交价格 double
					ret_data_json["fill_time"] = pTrade->TradeTime; // 成交时间 char 9
					// pTrade->BrokerID; // 经纪公司代码 char 11
					// pTrade->InvestorID; // 投资者代码 char 13
					// pTrade->UserID; // 用户代码 char 16
					// pTrade->OrderSysID; // 报单编号 char 21
					// pTrade->ParticipantID; // 会员代码 char 11
					// pTrade->ClientID; // 客户代码 char 11
					// pTrade->TradingRole; // 交易角色 char
					// pTrade->ExchangeInstID; // 合约在交易所的代码 char 31
					// pTrade->OffsetFlag; // 开平标志 char
					// pTrade->HedgeFlag; // 投机套保标志 char
					// pTrade->TradeDate; // 成交时期 char 9
					// pTrade->TradeType; // 成交类型 char
					// pTrade->PriceSource; // 成交价来源 char
					// pTrade->TraderID; // 交易所交易员代码 char 21
					// pTrade->OrderLocalID; // 本地报单编号 char 13
					// pTrade->ClearingPartID; // 结算会员编号 char 11
					// pTrade->BusinessUnit; // 业务单元 char 21
					// pTrade->SequenceNo; // 序号 int
					// pTrade->TradingDay; // 交易日 char 9
					// pTrade->SettlementID; // 结算编号 int
					// pTrade->BrokerOrderSeq; // 经纪公司报单编号 int
					// pTrade->TradeSource; // 成交来源 char
					results_json["ret_data"].append( ret_data_json );

					result_data = Json::writeString( m_json_writer, results_json );
					m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
				}

				if( true == bIsLast ) { // 补发结束标志消息
					log_info = "客户当日成交查询提交成功。";
					m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

					Json::Value results_json;
					results_json["ret_func"] = request->m_req_json["function"].asInt();
					results_json["ret_task"] = request->m_req_json["task_id"].asInt();
					results_json["ret_last"] = true;
					results_json["ret_code"] = 0;
					results_json["ret_info"] = basicx::StringToUTF8( "客户当日成交查询应答结束标记。" );
					results_json["ret_numb"] = 0;
					//results_json["ret_data"] = "";
					result_data = Json::writeString( m_json_writer, results_json );
					m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
				}
			}
		}
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "客户当日成交查询回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

int32_t Session::ReqQryInstrument( Request* request ) {
	try {
		std::string instrument = "";

		if( NW_MSG_CODE_JSON == request->m_code ) {
			instrument = request->m_req_json["instrument"].asCString();
		}
		//request->m_req_json["category"].asCString();

		CThostFtdcQryInstrumentField req;
		memset( &req, 0, sizeof( req ) );
		strcpy_s( req.InstrumentID, const_cast<char*>(instrument.c_str()) ); // 合约代码 char 31 // 为空表示查询所有合约
		// req.ExchangeID; // 交易所代码 char 9
		// req.ExchangeInstID; // 合约在交易所的代码 char 31
		// req.ProductID; // 产品代码 char 31
		int32_t request_id = GetRequestID();
		m_request_map_lock.lock();
		m_map_request[request_id] = *request;
		m_request_map_lock.unlock();
		return m_user_api->ReqQryInstrument( &req, request_id );
	}
	catch( ... ) {
		return -4;
	}
}

void Session::OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		bool no_error_and_is_last = false; // 标记是否需要结束标志消息

		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			results_json["ret_func"] = request->m_req_json["function"].asInt();
			results_json["ret_task"] = request->m_req_json["task_id"].asInt();
			//results_json["ret_data"] = "";

			if( pRspInfo && pRspInfo->ErrorID != 0 ) {
				FormatLibrary::StandardLibrary::FormatTo( log_info, "期货合约查询提交失败！{0}", pRspInfo->ErrorMsg );
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				results_json["ret_last"] = true;
				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
			}
			else if( !pInstrument ) { // 查询无结果数据
				log_info = "期货合约查询提交成功。无结果数据。";
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "期货合约查询应答结束标记。" );
				results_json["ret_numb"] = 0;
			}
			else {
				if( true == bIsLast ) { // 需要补发结束标志消息
					no_error_and_is_last = true;
				}

				results_json["ret_last"] = false;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 1;

				Json::Value ret_data_json;
				ret_data_json["otc_code"] = 0;
				ret_data_json["otc_info"] = basicx::StringToUTF8( log_info );
				ret_data_json["instrument"] = pInstrument->InstrumentID; // 合约代码 char 31
				ret_data_json["exchange"] = pInstrument->ExchangeID; // 交易所代码 char 9
				ret_data_json["delivery_y"] = pInstrument->DeliveryYear; // 交割年份 int
				ret_data_json["delivery_m"] = pInstrument->DeliveryMonth; // 交割月 int
				ret_data_json["long_margin"] = pInstrument->LongMarginRatio; // 多头保证金率 double
				ret_data_json["short_margin"] = pInstrument->ShortMarginRatio; // 空头保证金率 double
				// pInstrument->InstrumentName; // 合约名称 char 21
				// pInstrument->ExchangeInstID; // 合约在交易所的代码 char 31
				// pInstrument->ProductID; // 产品代码 char 31
				// pInstrument->ProductClass; // 产品类型 char
				// pInstrument->MaxMarketOrderVolume; // 市价单最大下单量 int
				// pInstrument->MinMarketOrderVolume; // 市价单最小下单量 int
				// pInstrument->MaxLimitOrderVolume; // 限价单最大下单量 int
				// pInstrument->MinLimitOrderVolume; // 限价单最小下单量 int
				// pInstrument->VolumeMultiple; // 合约数量乘数 int
				// pInstrument->PriceTick; // 最小变动价位 double
				// pInstrument->CreateDate; // 创建日 char 9
				// pInstrument->OpenDate; // 上市日 char 9
				// pInstrument->ExpireDate; // 到期日 char 9
				// pInstrument->StartDelivDate; // 开始交割日 char 9
				// pInstrument->EndDelivDate; // 结束交割日 char 9
				// pInstrument->InstLifePhase; // 合约生命周期状态 char
				// pInstrument->IsTrading; // 当前是否交易 int
				// pInstrument->PositionType; // 持仓类型 char
				// pInstrument->PositionDateType; // 持仓日期类型 char
				results_json["ret_data"].append( ret_data_json );
			}

			result_data = Json::writeString( m_json_writer, results_json );
		}

		m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );

		if( true == no_error_and_is_last ) { // 补发结束标志消息
			FormatLibrary::StandardLibrary::FormatTo( log_info, "期货合约查询提交成功。{0}", pInstrument->InstrumentID );
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

			if( NW_MSG_CODE_JSON == request->m_code ) {
				Json::Value results_json;
				results_json["ret_func"] = request->m_req_json["function"].asInt();
				results_json["ret_task"] = request->m_req_json["task_id"].asInt();
				results_json["ret_last"] = true;
				results_json["ret_code"] = 0;
				results_json["ret_info"] = basicx::StringToUTF8( "期货合约查询应答结束标记。" );
				results_json["ret_numb"] = 0;
				//results_json["ret_data"] = "";
				result_data = Json::writeString( m_json_writer, results_json );
			}

			m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
		}
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "期货合约查询回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

// FrontID、SessionID、OrderRef：在没有得到报单响应前，用户可以使用这组交易序列号进行撤单操作，三者唯一标识任何一笔委托
// BrokerID、BrokerOrderSeq：收到用户报单后，为每个经纪公司的报单生成的交易序列号
// exchangeID、traderID、OrderLocalID：交易席位在向交易所报单时，产生这组交易序列号，标示每一笔发往交易所的报单
// exchangeID、OrderSysID：交易所接受了投资者报单，产生这组交易序列号，标示每一笔收到的报单
// TThostFtdcOrderStatusType：报单状态，如果未知，表示 CTP 已经接受用户的委托指令但还没有转发到交易所
// 全部成交 = 全部成交；部分成交还在队列中 = 部分成交；部分成交不在队列中 = 部成部撤；未成交还在队列中 = 已报未成；未成交不在队列中 = 自动挂起；撤单 = 全部撤单

void Session::OnRtnOrder( CThostFtdcOrderField* pOrder ) {
	if( true == m_query_only ) { // 接口仅供查询时关闭委托和成交回报处理
		return;
	}

	std::string log_info;
	std::string result_risk = "";
	std::string result_data = "";
	std::string asset_account = "";

	// 委托撤单、委托查询应答、成交查询应答、成交回报中需要
	m_order_map_lock.lock();
	m_map_order[atoi( pOrder->OrderRef )] = *pOrder;
	m_order_map_lock.unlock();

	std::string direction( 1, pOrder->Direction );

	Request* request = GetRequestByID( pOrder->RequestID );
	if( request != nullptr ) {
		try {
			Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
			results_json["ret_func"] = 290001;
			//results_json["task_id"] = pOrder->RequestID; // 请求编号 int
			results_json["task_id"] = request->m_req_json["task_id"].asInt(); // 注意：这里应还原为用户的 task_id 而非直接用服务端的请求编号
			results_json["order_id"] = pOrder->OrderRef; // 报单引用 char 13
			results_json["order_sys_id"] = pOrder->OrderSysID; // 报单编号 char 21
			results_json["instrument"] = pOrder->InstrumentID; // 合约代码 char 31
			results_json["exchange"] = pOrder->ExchangeID; // 交易所代码 char 9
			results_json["exch_side"] = atoi( direction.c_str() ) + 1; // 买卖方向 char
			results_json["fill_qty"] = pOrder->VolumeTraded; // 成交数量 int
			// 正常获取 n_status 的话 "status" 字段一定存在
			if( pOrder->OrderStatus != 'a' && pOrder->OrderStatus != 'b' && pOrder->OrderStatus != 'c' ) {
				// 0：全部成交，1：部分成交，2：部成部撤，3：已报未成，4：自动挂起，5：全部撤单
				int32_t status_temp[] = { 5, 4, 7, 3, 13, 8 };
				std::string s_status( 1, pOrder->OrderStatus ); // 报单状态 char
				int32_t n_status = atoi( s_status.c_str() );
				if( n_status >= 0 && n_status <= 5 ) {
					results_json["status"] = status_temp[n_status];
				}
			}
			else if( 'a' == pOrder->OrderStatus ) { // 未知
				results_json["status"] = 14;
			}
			else if( 'b' == pOrder->OrderStatus ) { // 尚未触发
				results_json["status"] = 11;
			}
			else if( 'c' == pOrder->OrderStatus ) { // 已经触发
				results_json["status"] = 12;
			}
			results_json["status_msg"] = basicx::StringToUTF8( pOrder->StatusMsg ); // 状态信息 char 81
			// pOrder->BrokerID; // 经纪公司代码 char 11
			// pOrder->InvestorID; // 投资者代码 char 13
			// pOrder->UserID; // 用户代码 char 16
			// pOrder->OrderPriceType; // 报单价格条件 char
			// pOrder->CombOffsetFlag; // 组合开平标志 char 5
			// pOrder->CombHedgeFlag; // 组合投机套保标志 char 5
			// pOrder->LimitPrice; // 价格 double
			// pOrder->VolumeTotalOriginal; // 数量 int
			// pOrder->TimeCondition; // 有效期类型 char
			// pOrder->GTDDate; // GTD日期 char 9
			// pOrder->VolumeCondition; // 成交量类型 char
			// pOrder->MinVolume; // 最小成交量 int
			// pOrder->ContingentCondition; // 触发条件 char
			// pOrder->StopPrice; // 止损价 double
			// pOrder->ForceCloseReason; // 强平原因 char
			// pOrder->IsAutoSuspend; // 自动挂起标志 int
			// pOrder->BusinessUnit; // 业务单元 char 21
			// pOrder->OrderLocalID; // 本地报单编号 char 13
			// pOrder->ParticipantID; // 会员代码 char 11
			// pOrder->ClientID; // 客户代码 char 11
			// pOrder->ExchangeInstID; // 合约在交易所的代码 char 31
			// pOrder->TraderID; // 交易所交易员代码 char 21
			// pOrder->InstallID; // 安装编号 int
			// pOrder->OrderSubmitStatus; // 报单提交状态 char
			// pOrder->NotifySequence; // 报单提示序号 int
			// pOrder->TradingDay; // 交易日 char 9
			// pOrder->SettlementID; // 结算编号 int
			// pOrder->OrderSource; // 报单来源 char
			// pOrder->OrderType; // 报单类型 char
			// pOrder->VolumeTotal; // 剩余数量 int
			// pOrder->InsertDate; // 报单日期 char 9
			// pOrder->InsertTime; // 委托时间 char 9
			// pOrder->ActiveTime; // 激活时间 char 9
			// pOrder->SuspendTime; // 挂起时间 char 9
			// pOrder->UpdateTime; // 最后修改时间 char 9
			// pOrder->CancelTime; // 撤销时间 char 9
			// pOrder->ActiveTraderID; // 最后修改交易所交易员代码 char 21
			// pOrder->ClearingPartID; // 结算会员编号 char 11
			// pOrder->SequenceNo; // 序号 int
			// pOrder->FrontID; // 前置编号 int
			// pOrder->SessionID; // 会话编号 int
			// pOrder->UserProductInfo; // 用户端产品信息 char 11
			// pOrder->UserForceClose; // 用户强评标志 int
			// pOrder->ActiveUserID; // 操作用户代码 char 16
			// pOrder->BrokerOrderSeq; // 经纪公司报单编号 int
			// pOrder->RelativeOrderSysID; // 相关报单 char 21
			// pOrder->ZCETotalTradedVolume; // 郑商所成交数量 int
			// pOrder->IsSwapOrder; // 互换单标志 int
			result_data = Json::writeString( m_json_writer, results_json );
		}
		catch( ... ) {
			log_info = "获取 OnRtnOrder 信息异常！";
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
		}

		if( result_data != "" ) {
			m_trader_ctp_p->CommitResult( 1, request->m_identity, NW_MSG_CODE_JSON, result_data ); // 回报统一用 NW_MSG_CODE_JSON 编码 //Trade：1、Risks：2
		}

		asset_account = request->m_req_json["asset_account"].asString();
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "报单回报回调查找失败！{0}", pOrder->RequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}

	//////////////////// 发送给风控服务端 ////////////////////
	try {
		Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
		results_json["ret_func"] = TD_FUNC_RISKS_ORDER_REPORT_FUE;
		results_json["task_id"] = 0;
		results_json["asset_account"] = asset_account; // 产品账号
		results_json["account"] = m_username; // 交易账号
		results_json["order_id"] = pOrder->OrderRef; // 报单引用 char 13
		results_json["order_sys_id"] = pOrder->OrderSysID; // 报单编号 char 21
		results_json["instrument"] = pOrder->InstrumentID; // 合约代码 char 31
		results_json["exchange"] = pOrder->ExchangeID; // 交易所代码 char 9
		results_json["exch_side"] = atoi( direction.c_str() ) + 1; // 买卖方向 char
		results_json["fill_qty"] = pOrder->VolumeTraded; // 成交数量 int
		// 正常获取 n_status 的话 "status" 字段一定存在
		if( pOrder->OrderStatus != 'a' && pOrder->OrderStatus != 'b' && pOrder->OrderStatus != 'c' ) {
			// 0：全部成交，1：部分成交，2：部成部撤，3：已报未成，4：自动挂起，5：全部撤单
			int32_t status_temp[] = { 5, 4, 7, 3, 13, 8 };
			std::string s_status( 1, pOrder->OrderStatus ); // 报单状态 char
			int32_t n_status = atoi( s_status.c_str() );
			if( n_status >= 0 && n_status <= 5 ) {
				results_json["status"] = status_temp[n_status];
			}
		}
		else if( 'a' == pOrder->OrderStatus ) { // 未知
			results_json["status"] = 14;
		}
		else if( 'b' == pOrder->OrderStatus ) { // 尚未触发
			results_json["status"] = 11;
		}
		else if( 'c' == pOrder->OrderStatus ) { // 已经触发
			results_json["status"] = 12;
		}
		results_json["status_msg"] = basicx::StringToUTF8( pOrder->StatusMsg ); // 状态信息 char 81
		result_risk = Json::writeString( m_json_writer, results_json );
	}
	catch( ... ) {
		log_info = "风控：获取 OnRtnOrder 信息异常！";
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}

	if( result_risk != "" ) {
		m_risker->CommitResult( NW_MSG_CODE_JSON, result_risk ); // 回报统一用 NW_MSG_CODE_JSON 编码
	}
	//////////////////// 发送给风控服务端 ////////////////////

	FormatLibrary::StandardLibrary::FormatTo( log_info, "报单回报：UserID：{0}, OrderRef：{1}, OrderSysID：{2}, Instrument：{3}, Exchange：{4}, ExchSide：{5}, FillQty：{6}, Status：{7}, StatusMsg：{8}", 
		pOrder->UserID, pOrder->OrderRef, pOrder->OrderSysID, pOrder->InstrumentID, pOrder->ExchangeID, atoi( direction.c_str() ) + 1, pOrder->VolumeTraded, std::string( 1, pOrder->OrderStatus ), pOrder->StatusMsg );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_debug, log_info, FILE_LOG_ONLY );
}

void Session::OnRtnTrade( CThostFtdcTradeField* pTrade ) {
	if( true == m_query_only ) { // 接口仅供查询时关闭委托和成交回报处理
		return;
	}

	std::string log_info;
	std::string result_risk = "";
	std::string result_data = "";
	std::string asset_account = "";

	std::string direction( 1, pTrade->Direction );

	CThostFtdcOrderField* order_item = GetOrderItemByID( atoi( pTrade->OrderRef ) );
	if( nullptr == order_item ) {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "成交回报 委托 查找失败！{0}", pTrade->OrderRef );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
		return;
	}

	Request* request = GetRequestByID( order_item->RequestID );
	if( request != nullptr ) {
		try {
			Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
			results_json["ret_func"] = 290002;
			results_json["task_id"] = request->m_req_json["task_id"].asInt(); // 注意：这里应还原为用户的 task_id 而非直接用服务端的请求编号
			results_json["order_id"] = pTrade->OrderRef; // 报单引用 char 13
			results_json["trans_id"] = pTrade->TradeID; // 成交编号 char 21
			results_json["instrument"] = pTrade->InstrumentID; // 合约代码 char 31
			results_json["exchange"] = pTrade->ExchangeID; // 交易所代码 char 9
			results_json["exch_side"] = atoi( direction.c_str() ) + 1; // 买卖方向 char
			results_json["fill_qty"] = pTrade->Volume; // 成交数量 int
			results_json["fill_price"] = pTrade->Price; // 成交价格 double
			results_json["fill_time"] = pTrade->TradeTime; // 成交时间 char 9
			// pTrade->BrokerID; // 经纪公司代码 char 11
			// pTrade->InvestorID; // 投资者代码 char 13
			// pTrade->UserID; // 用户代码 char 16
			// pTrade->OrderSysID; // 报单编号 char 21
			// pTrade->ParticipantID; // 会员代码 char 11
			// pTrade->ClientID; // 客户代码 char 11
			// pTrade->TradingRole; // 交易角色 char
			// pTrade->ExchangeInstID; // 合约在交易所的代码 char 31
			// pTrade->OffsetFlag; // 开平标志 char
			// pTrade->HedgeFlag; // 投机套保标志 char
			// pTrade->TradeDate; // 成交时期 char 9
			// pTrade->TradeType; // 成交类型 char
			// pTrade->PriceSource; // 成交价来源 char
			// pTrade->TraderID; // 交易所交易员代码 char 21
			// pTrade->OrderLocalID; // 本地报单编号 char 13
			// pTrade->ClearingPartID; // 结算会员编号 char 11
			// pTrade->BusinessUnit; // 业务单元 char 21
			// pTrade->SequenceNo; // 序号 int
			// pTrade->TradingDay; // 交易日 char 9
			// pTrade->SettlementID; // 结算编号 int
			// pTrade->BrokerOrderSeq; // 经纪公司报单编号 int
			// pTrade->TradeSource; // 成交来源 char
			result_data = Json::writeString( m_json_writer, results_json );
		}
		catch( ... ) {
			log_info = "获取 OnRtnTrade 信息异常！";
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
		}

		if( result_data != "" ) {
			m_trader_ctp_p->CommitResult( 1, request->m_identity, NW_MSG_CODE_JSON, result_data ); // 回报统一用 NW_MSG_CODE_JSON 编码 // Trade：1、Risks：2
		}

		asset_account = request->m_req_json["asset_account"].asString();
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "成交回报回调查找失败！{0}", order_item->RequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}

	//////////////////// 发送给风控服务端 ////////////////////
	try {
		Json::Value results_json; // 回报统一用 NW_MSG_CODE_JSON 编码
		results_json["ret_func"] = TD_FUNC_RISKS_TRANSACTION_REPORT_FUE;
		results_json["task_id"] = 0;
		results_json["asset_account"] = asset_account; // 产品账号
		results_json["account"] = m_username; // 交易账号
		results_json["order_id"] = pTrade->OrderRef; // 报单引用 char 13
		results_json["trans_id"] = pTrade->TradeID; // 成交编号 char 21
		results_json["instrument"] = pTrade->InstrumentID; // 合约代码 char 31
		results_json["exchange"] = pTrade->ExchangeID; // 交易所代码 char 9
		results_json["exch_side"] = atoi( direction.c_str() ) + 1; // 买卖方向 char
		results_json["fill_qty"] = pTrade->Volume; // 成交数量 int
		results_json["fill_price"] = pTrade->Price; // 成交价格 double
		results_json["fill_time"] = pTrade->TradeTime; // 成交时间 char 9
		result_risk = Json::writeString( m_json_writer, results_json );
	}
	catch( ... ) {
		log_info = "风控：获取 OnRtnTrade 信息异常！";
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}

	if( result_risk != "" ) {
		m_risker->CommitResult( NW_MSG_CODE_JSON, result_risk ); // 回报统一用 NW_MSG_CODE_JSON 编码
	}
	//////////////////// 发送给风控服务端 ////////////////////

	FormatLibrary::StandardLibrary::FormatTo( log_info, "成交回报：UserID：{0}, OrderRef：{1}, TradeID：{2}, Instrument：{3}, Exchange：{4}, ExchSide：{5}, FillQty：{6}, FillPrice：{7}", 
		pTrade->UserID, pTrade->OrderRef, pTrade->TradeID, pTrade->InstrumentID, pTrade->ExchangeID, atoi( direction.c_str() ) + 1, pTrade->Volume, pTrade->Price );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_debug, log_info, FILE_LOG_ONLY );
}

void Session::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
	std::string log_info;
	std::string result_data = "";
	Request* request = GetRequestByID( nRequestID );

	if( request != nullptr ) {
		if( NW_MSG_CODE_JSON == request->m_code ) {
			Json::Value results_json;
			results_json["ret_func"] = request->m_req_json["function"].asInt();
			results_json["ret_task"] = request->m_req_json["task_id"].asInt();
			results_json["ret_last"] = bIsLast;
			//results_json["ret_data"] = "";

			if( pRspInfo && pRspInfo->ErrorID != 0 ) {
				if( pRspInfo ) {
					FormatLibrary::StandardLibrary::FormatTo( log_info, "用户请求出错通知！{0}", pRspInfo->ErrorMsg );
				}
				else {
					log_info = "用户请求出错通知！原因未知！";
				}
				m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );

				results_json["ret_code"] = -1;
				results_json["ret_info"] = basicx::StringToUTF8( log_info );
				results_json["ret_numb"] = 0;
			}

			result_data = Json::writeString( m_json_writer, results_json );
		}

		m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
	}
	else {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "异常回调查找失败！{0}", nRequestID );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

std::string Session::GetLastErrorMsg() {
	std::string last_error_msg = m_last_error_msg;
	m_last_error_msg = "无错误信息。";
	return last_error_msg;
}

int32_t Session::GetRequestID() {
	m_request_id++;
	if( m_request_id > 2147483600 ) { // 2147483648
		m_request_id = 1;
	}
	return m_request_id;
}

Request* Session::GetRequestByID( int32_t request_id ) {
	Request* request = nullptr;
	m_request_map_lock.lock();
	std::map<int32_t, Request>::iterator it_r = m_map_request.find( request_id );
	if( it_r != m_map_request.end() ) {
		request = &( it_r->second);
	}
	m_request_map_lock.unlock();
	return request;
}

CThostFtdcOrderField* Session::GetOrderItemByID( int32_t order_id ) {
	CThostFtdcOrderField* order_item = nullptr;
	m_order_map_lock.lock();
	std::map<int32_t, CThostFtdcOrderField>::iterator it_of = m_map_order.find( order_id );
	if( it_of != m_map_order.end() ) {
		order_item = &( it_of->second);
	}
	m_order_map_lock.unlock();
	return order_item;
}

void Session::CreateCtpTradeApi( std::string trade_front ) {
	std::string ext_folder = m_syscfg->GetPath_ExtFolder() + "\\";
	m_user_api = CThostFtdcTraderApi::CreateFtdcTraderApi( ext_folder.c_str() );
	m_user_api->RegisterSpi( (CThostFtdcTraderSpi*)this ); // 注册事件类
	m_user_api->RegisterFront( const_cast<char*>( trade_front.c_str()) ); // 注册交易前置地址
	m_user_api->SubscribePublicTopic( THOST_TERT_QUICK ); // 注册公有流 // TERT_RESTART、TERT_RESUME、TERT_QUICK
	m_user_api->SubscribePrivateTopic( THOST_TERT_QUICK ); // 注册私有流 // TERT_RESTART、TERT_RESUME、TERT_QUICK
	m_user_api->Init();
	m_user_api->Join();
}

void Session::CreateServiceUser() {
	std::string log_info;

	FormatLibrary::StandardLibrary::FormatTo( log_info, "创建 会话 {0} 输入输出服务线程完成, 开始进行输入输出服务 ...", m_session );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

	try {
		try {
			m_service_user = boost::make_shared<boost::asio::io_service>();
			boost::asio::io_service::work Work( *m_service_user );
			m_work_thread_user = boost::make_shared<boost::thread>( boost::bind( &boost::asio::io_service::run, m_service_user ) );
			m_service_user_running = true;
			m_work_thread_user->join();
		}
		catch( std::exception& ex ) {
			FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 输入输出服务 初始化 异常：{1}", m_session, ex.what() );
			m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
		}
	} // try
	catch( ... ) {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 输入输出服务线程发生未知错误！", m_session );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_fatal, log_info );
	}

	StopServiceUser();

	FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 输入输出服务线程退出！", m_session );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_warn, log_info );
}

void Session::HandleRequestMsg() {
	std::string log_info;

	try {
		std::string result_data = "";
		Request* request = &m_list_request.front(); // 肯定有

		//FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 开始处理 {1} 号任务 ...", m_session, request->m_task_id );
		//m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

		int32_t func_id = 0;
		int32_t task_id = 0;
		try {
			if( NW_MSG_CODE_JSON == request->m_code ) {
				func_id = request->m_req_json["function"].asInt();
				task_id = request->m_req_json["task_id"].asInt();
			}
			// func_id > 0 已在 HandleTaskMsg() 中保证

			switch( func_id ) {
			case TD_FUNC_FUTURE_ADDSUB:
				result_data = OnSubscibe( request );
				break;
			case TD_FUNC_FUTURE_DELSUB:
				result_data = OnUnsubscibe( request );
				break;
			default: // 其他交易类功能编号
				result_data = OnTradeRequest( request );
			}
		}
		catch( ... ) {
			FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 处理任务 {1} 时发生未知错误！", m_session, request->m_task_id );
			result_data = m_trader_ctp_p->OnErrorResult( func_id, -1, log_info, task_id, request->m_code );
		}

		if( result_data != "" ) { // 不为空说明有错，否则由回调返回给用户
			m_trader_ctp_p->CommitResult( request->m_task_id, request->m_identity, request->m_code, result_data );
			if( 220001 == func_id || 220002 == func_id || 220003 == func_id ) { // 风控通知
				std::string asset_account = request->m_req_json["asset_account"].asString();
				m_risker->CheckTradeResultForRisk( asset_account, func_id, task_id, result_data ); // 这里 result_data 为整个 JSON 语句，最好能只使用错误信息
			}
		}

		//FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 处理 {1} 号任务完成。", m_session, request->m_task_id );
		//m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

		m_request_list_lock.lock();
		m_list_request.pop_front();
		bool write_on_progress = !m_list_request.empty();
		m_request_list_lock.unlock();

		if( write_on_progress && true == m_service_user_running ) { // m_service_user_running
			m_service_user->post( boost::bind( &Session::HandleRequestMsg, this ) );
		}
	}
	catch( std::exception& ex ) {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "会话 {0} 处理 Request 消息 异常：{1}", m_session, ex.what() );
		m_trader_ctp_p->LogPrint( basicx::syslog_level::c_error, log_info );
	}
}

void Session::StopServiceUser() {
	if( true == m_service_user_running ) {
		m_service_user_running = false;
		m_service_user->stop();
	}
}

std::string Session::OnSubscibe( Request* request ) {
	std::string log_info;

	int32_t task_id = 0;
	std::string password = "";
	if( NW_MSG_CODE_JSON == request->m_code ) {
		task_id = request->m_req_json["task_id"].asInt();
		password = request->m_req_json["password"].asString();
	}
	if( "" == password ) {
		FormatLibrary::StandardLibrary::FormatTo( log_info, "用户订阅时 密码 为空！session：{0}", m_session );
		return m_trader_ctp_p->OnErrorResult( TD_FUNC_FUTURE_ADDSUB, -1, log_info, task_id, request->m_code );
	}
	if( m_password != password ) { // 需要密码才能订阅
		FormatLibrary::StandardLibrary::FormatTo( log_info, "用户订阅时 密码 错误！session：{0}", m_session );
		return m_trader_ctp_p->OnErrorResult( TD_FUNC_FUTURE_ADDSUB, -1, log_info, task_id, request->m_code );
	}

	// 这里 CTP 不需要发起真实 订阅
	
	m_sub_endpoint_map_lock.lock();
	m_map_sub_endpoint[request->m_identity] = request->m_identity;
	m_sub_endpoint_map_lock.unlock();

	FormatLibrary::StandardLibrary::FormatTo( log_info, "用户订阅成功。session：{0}", m_session );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

	if( NW_MSG_CODE_JSON == request->m_code ) {
		Json::Value results_json;
		results_json["ret_func"] = TD_FUNC_FUTURE_ADDSUB;
		results_json["ret_code"] = 0;
		results_json["ret_info"] = basicx::StringToUTF8( log_info );
		results_json["ret_task"] = task_id;
		results_json["ret_last"] = true;
		results_json["ret_numb"] = 0;
		results_json["ret_data"] = "";
		return Json::writeString( m_json_writer, results_json );
	}

	return "";
}

std::string Session::OnUnsubscibe( Request* request ) {
	std::string log_info;

	int32_t task_id = 0;
	if( NW_MSG_CODE_JSON == request->m_code ) {
		task_id = request->m_req_json["task_id"].asInt();
	}

	m_sub_endpoint_map_lock.lock();
	m_map_sub_endpoint.erase( request->m_identity );
	m_sub_endpoint_map_lock.unlock();

	if( m_map_sub_endpoint.empty() ) { // 订阅已无用户
		// 这里 CTP 不需要发起真实 退订
	}

	FormatLibrary::StandardLibrary::FormatTo( log_info, "用户退订成功。session：{0}", m_session );
	m_trader_ctp_p->LogPrint( basicx::syslog_level::c_info, log_info );

	if( NW_MSG_CODE_JSON == request->m_code ) {
		Json::Value results_json;
		results_json["ret_func"] = TD_FUNC_FUTURE_DELSUB;
		results_json["ret_code"] = 0;
		results_json["ret_info"] = basicx::StringToUTF8( log_info );
		results_json["ret_task"] = task_id;
		results_json["ret_last"] = true;
		results_json["ret_numb"] = 0;
		results_json["ret_data"] = "";
		return Json::writeString( m_json_writer, results_json );
	}

	return "";
}

std::string Session::OnTradeRequest( Request* request ) {
	std::string log_info = "";

	int32_t func_id = 0;
	int32_t task_id = 0;
	if( NW_MSG_CODE_JSON == request->m_code ) {
		func_id = request->m_req_json["function"].asInt();
		task_id = request->m_req_json["task_id"].asInt();
	}

	if( 220001 == func_id || 220002 == func_id || 220003 == func_id ) { // 风控检查
		std::string asset_account = request->m_req_json["asset_account"].asString();
		int32_t risk_ret = m_risker->HandleRiskCtlCheck( asset_account, func_id, task_id, request, log_info );
		if( risk_ret < 0 ) {
			return m_trader_ctp_p->OnErrorResult( func_id, risk_ret, log_info, task_id, request->m_code );
		}
	}

	int32_t ret = 0;
	switch( func_id ) {
	case 220001: // 单个期货委托下单
		ret = ReqOrderInsert( request, false );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "单个期货 下单 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 220002: // 单个期货委托撤单
		ret = ReqOrderAction( request );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "单个期货 撤单 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 220003: // 套利期货委托下单
		ret = ReqOrderInsert( request, true );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "套利期货 下单 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 230002: // 查询客户资金
		ret = ReqQryTradingAccount( request );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "查询 客户资金 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 230004: // 查询客户持仓
		ret = ReqQryInvestorPosition( request );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "查询 客户持仓 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 230005: // 查询客户当日委托
		ret = ReqQryOrder( request );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "查询 客户当日委托 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 230006: // 查询客户当日成交
		ret = ReqQryTrade( request );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "查询 客户当日成交 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	case 230009: // 查询期货合约
		ret = ReqQryInstrument( request );
		return m_trader_ctp_p->TransErrorResult( func_id, ret, "查询 期货合约 时请求失败！", task_id, request->m_code ); // 有错时由这里返回给用户，无错时返回空，则由回调返回给用户
	default:
		FormatLibrary::StandardLibrary::FormatTo( log_info, "业务 {0} 未知！", func_id );
		return m_trader_ctp_p->OnErrorResult( func_id, -1, log_info, task_id, request->m_code );
	}

	return "";
}
