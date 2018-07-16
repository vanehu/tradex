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

#ifndef TRADER_CTP_SESSION_CTP_H
#define TRADER_CTP_SESSION_CTP_H

#include "../../global/compile.h"

#ifdef TRADER_CTP_COMMUNITY
    #include "risker_ctp.h"
#endif
#ifdef TRADER_CTP_PROFESSIONAL
    #include "risker_ctp_pro.h"
#endif

#define FILE_LOG_ONLY 1

class TraderCTP_P;

// 一个会话与一个连接与一个账户三者关联，每个会话可以有多个客户端以同一账号进行交易
// 回报根据 m_map_sub_endpoint 中的连接标识进行广播，已断开的连接将在发送数据时取得空连接指针
// 交易根据 request 中连接标识进行结果发送，客户端非正常登出则 m_map_con_endpoint 中连接标识将被保留
// 如果存在非正常登出客户端，则即使该会话已无有效连接，因为 m_map_con_endpoint 非空该会话将被保留
// 在未开启新连接的会话认证时，非正常登出或正常登出的客户端均可使用原会话号连接后直接交易
// 开启新连接的会话认证时，因为每个连接标识不同，所以必须重新登录将连接标识添加至 m_map_con_endpoint 后才能交易
// 因为回报根据 m_map_sub_endpoint 中连接标识广播，所以非正常退订或正常退订都需要重新连接并订阅才能收到回报数据
// 交易和回报可以使用同一连接，但此时客户端需具备异步处理交易进行过程中穿插的回报数据的能力

// 关于 CTP 的 OrderRef 和 OrderActionRef 其实客户端在报送下单或撤单时是可以自己指定的，这样能事先将 Order 放入内存中再下单然后等待回报
// 但很多同步的交易接口中是不能指定的，只能从委托请求返回的数据中获得，在 CTP 中相当于要等待 OnRspOrderInsert 返回
// 但是如果用户报单正确根本不会“马上收到报单响应 OnRspOrderInsert”， 只有报单被 CTP 拒绝才会收到
// 所以目前 OrderRef 和 OrderActionRef 统一由服务端生成，也避免多个客户端登录同一账号时委托号的分配问题
// 客户端下单后在收到 OnRtnOrder 时，根据 task_id 辨识是否是自己的委托，再将 Order 放入内存进行管理（撤单也一样？会不会有成交回报先于报单回报的问题？）
// 查看外盘 Patsystems 的接口，其下单后也有一个临时的委托号，在报到交易所以后才会有正式的委托号，这里的 task_id 类似于临时委托号

class Session : public CThostFtdcTraderSpi
{
public:
	Session( TraderCTP_P* trader_ctp_p );
	~Session();

public:
	// 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected() override;
	// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	void OnFrontDisconnected( int nReason ) override;
	// 心跳超时警告。当长时间未收到报文时，该方法被调用。
	void OnHeartBeatWarning( int nTimeLapse ) override;
	// 登录请求响应。
	void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 登出请求响应。
	void OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 投资者结算结果确认响应。
	void OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 报单录入请求响应。
	void OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 交易所报单录入错误回报。
	void OnErrRtnOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo ) override;
	// 报单操作请求响应。
	void OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 交易所报单操作错误回报。
	void OnErrRtnOrderAction( CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo ) override;
	// 请求查询资金账户响应。
	void OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 请求查询投资者持仓响应。
	void OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 请求查询报单响应。
	void OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 请求查询成交响应。
	void OnRspQryTrade( CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
	// 请求查询合约响应。
	void OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;

	// 报单通知。
	void OnRtnOrder( CThostFtdcOrderField* pOrder ) override;
	// 成交通知。
	void OnRtnTrade( CThostFtdcTradeField* pTrade ) override;
	// 错误应答。
	void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;

public:
	// 用户登录请求。
	int32_t ReqUserLogin( std::string broker_id, std::string user_id, std::string password );
	// 用户登出请求。
	int32_t ReqUserLogout();
	// 投资者结算结果确认。
	int32_t ReqSettlementInfoConfirm();
	// 报单录入请求。
	int32_t ReqOrderInsert( Request* request, bool is_arbitrage );
	// 报单操作请求。
	int32_t ReqOrderAction( Request* request );
	// 请求查询资金账户。
	int32_t ReqQryTradingAccount( Request* request );
	// 请求查询投资者持仓。
	int32_t ReqQryInvestorPosition( Request* request );
	// 请求查询报单。
	int32_t ReqQryOrder( Request* request );
	// 请求查询成交。
	int32_t ReqQryTrade( Request* request );
	// 请求查询合约。
	int32_t ReqQryInstrument( Request* request );

public:
	std::string GetLastErrorMsg(); // 调用后 m_last_error_msg 将被重置
	int32_t GetRequestID();
	Request* GetRequestByID( int32_t request_id );
	CThostFtdcOrderField* GetOrderItemByID( int32_t order_id );
	void CreateCtpTradeApi( std::string trade_front );
	void CreateServiceUser();
	void HandleRequestMsg();
	void StopServiceUser();

	std::string OnSubscibe( Request* request );
	std::string OnUnsubscibe( Request* request );
	std::string OnTradeRequest( Request* request );
	
public:
	int32_t m_session;
	risker_ptr m_risker;
	std::string m_username;
	std::string m_password;
	std::string m_broker_id;
	bool m_query_only; // 接口是否仅供查询
	boost::mutex m_con_endpoint_map_lock;
	std::map<int32_t, int32_t> m_map_con_endpoint;
	boost::mutex m_sub_endpoint_map_lock;
	std::map<int32_t, int32_t> m_map_sub_endpoint;

	service_ptr m_service_user;
	bool m_service_user_running;
	thread_ptr m_thread_service_user;
	thread_ptr m_work_thread_user;

	CThostFtdcTraderApi* m_user_api;
	thread_ptr m_init_api_thread_user;
	bool m_last_rsp_is_error; // 用于最近异步错误
	std::string m_last_error_msg; // 用于最近错误信息
	int32_t m_request_id; // 请求标识编号
	bool m_connect_ok; // 连接已成功
	bool m_login_ok; // 登录已成功
	bool m_logout_ok; // 登出已成功
	bool m_settle_ok; // 结算确认已成功
	int32_t m_front_id; // 前置编号
	int32_t m_session_id; // 会话编号

	boost::mutex m_request_list_lock;
	std::list<Request> m_list_request;

	boost::mutex m_request_map_lock;
	std::map<int32_t, Request> m_map_request; // 等待异步回调
	boost::mutex m_order_map_lock;
	std::map<int32_t, CThostFtdcOrderField> m_map_order; // 虽然 OrderRef 为字符串，这里还是转成整数以提高查找效率吧

	Json::StreamWriterBuilder m_json_writer;

private:
	std::string m_log_cate;
	basicx::SysCfg_S* m_syscfg;
	TraderCTP_P* m_trader_ctp_p;
};

#endif // TRADER_CTP_SESSION_CTP_H
