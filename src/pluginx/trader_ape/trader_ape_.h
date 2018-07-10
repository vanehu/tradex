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

#ifndef TRADER_APE_TRADER_APE_P_H
#define TRADER_APE_TRADER_APE_P_H

#include "struct_ape.h"
#include "trader_ape.h"
#include "session_ape.h"

typedef boost::shared_ptr<basicx::NetServer> net_server_ptr;
typedef boost::shared_ptr<basicx::NetClient> net_client_ptr;

class TraderAPE_P : public basicx::NetServer_X, public basicx::NetClient_X
{
public:
	TraderAPE_P();
	~TraderAPE_P();

public:
	void SetGlobalPath();
	bool ReadConfig( std::string file_path );
	void LogPrint( basicx::syslog_level log_level, std::string& log_cate, std::string& log_info, int32_t log_show = 0 );

public:
	bool Initialize();
	bool InitializeExt();
	bool StartPlugin();
	bool IsPluginRun();
	bool StopPlugin();
	bool UninitializeExt();
	bool Uninitialize();
	bool AssignTask( int32_t task_id, int32_t identity, int32_t code, std::string& data );

	void CreateService();
	void HandleTaskMsg();

private:
	Config m_configs;
	std::string m_location;
	std::string m_info_file_path;
	std::string m_cfg_file_path;

	service_ptr m_service;
	bool m_service_running;
	thread_ptr m_thread_service;
	int32_t m_work_thread_number;
	std::vector<thread_ptr> m_vec_work_thread;

	std::atomic<int32_t> m_task_id;
	boost::mutex m_task_list_lock;
	std::list<TaskItem> m_list_task;

	bool m_plugin_running;

	std::string m_log_cate;
	basicx::SysLog_S* m_syslog;
	basicx::SysCfg_S* m_syscfg;
	basicx::SysRtm_S* m_sysrtm;
	basicx::Plugins* m_plugins;

// 自定义成员函数和变量
public:
	void InitRiskCtl();
	void RiskOnTimer();
	bool InitFixInfo();

	void StartNetServer();
	void OnNetServerInfo( basicx::NetServerInfo& net_server_info_temp ) override;
	void OnNetServerData( basicx::NetServerData& net_server_data_temp ) override;
	void StartNetClient();
	void OnNetClientInfo( basicx::NetClientInfo& net_client_info_temp ) override;
	void OnNetClientData( basicx::NetClientData& net_client_data_temp ) override;

	int32_t GetTaskId();
	int32_t GetSessionID();
	void HandleUserRequest();
	std::string OnUserLogin( Request* request );
	std::string OnUserLogout( Request* request );
	int32_t RiskConfigRead( std::string asset_account );
	void OnRiskConfigRead( int32_t risks_type, Json::Value& reply_json );
	int32_t RiskCheckRead( std::string asset_account );
	void OnRiskCheckRead( int32_t risks_type, Json::Value& reply_json );
	int32_t RiskNameListRead( std::string asset_account );
	void OnRiskNameListRead( int32_t risks_type, Json::Value& reply_json );
	int32_t RiskStockListRead();
	void OnRiskStockListRead( int32_t risks_type, Json::Value& reply_json );
	void OnRiskStatDataReset( int32_t risks_type, Json::Value& reply_json );
	void CommitResult( int32_t code, std::string& data ); // Risks
	void CommitResult( int32_t task_id, int32_t identity, int32_t code, std::string& data ); // Trade
	std::string OnErrorResult( int32_t func_id, int32_t ret_code, std::string ret_info, int32_t ret_task, int32_t encode );

private:
	Json::CharReader* m_json_reader_trader;
	Json::CharReader* m_json_reader_risker;
	Json::CharReaderBuilder m_json_reader_builder;
	Json::StreamWriterBuilder m_json_writer_trader;
	Json::StreamWriterBuilder m_json_writer_risker;

	net_server_ptr m_net_server_trade;
	net_client_ptr m_net_client_risks;
	basicx::ConnectInfo* m_connecter_risks;

	thread_ptr m_thread_user_request;
	boost::mutex m_user_request_list_lock;
	std::list<Request> m_list_user_request;

	int32_t m_count_session_id;
	boost::mutex m_session_map_lock;
	std::map<int32_t, Session*> m_map_session;
	std::vector<Session*> m_vec_useless_session;

	thread_ptr m_thread_risk_on_timer;
	std::atomic<bool> m_risk_on_timer_running;
	std::map<std::string, Risker*> m_map_risker; // 产品账号为索引
	std::map<std::string, RiskConfig> m_map_risk_config; // 产品账号为索引
	std::map<std::string, RiskNameList> m_map_risk_name_list; // 产品账号为索引
	RiskStockList m_risk_stock_list;
};

#endif // TRADER_APE_TRADER_APE_P_H
