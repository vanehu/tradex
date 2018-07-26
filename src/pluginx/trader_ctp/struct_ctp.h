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

#ifndef TRADER_CTP_STRUCT_CTP_H
#define TRADER_CTP_STRUCT_CTP_H

#include <map>
#include <list>
#include <vector>
#include <atomic>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <common/define.h> // NW_MSG_CODE_JSON
#include <common/assist.h>
#include <common/common.h>
#include <syslog/syslog.h>
#include <syscfg/syscfg.h>
#include <sysrtm/sysrtm.h>
#include <network/server.h>
#include <network/client.h>
#include <plugins/plugins.h>

#include "define_ctp.h"

#include "ThostFtdcTraderApi.h"

typedef boost::shared_ptr<boost::thread> thread_ptr;
typedef boost::shared_ptr<boost::asio::io_service> service_ptr;

#pragma pack( push )
#pragma pack( 1 )

struct TaskItem // 保证均被赋值
{
	int32_t m_task_id;
	int32_t m_identity;
	int32_t m_code;
	std::string m_data;
};

struct Request // 保证均被赋值
{
	int32_t m_task_id;
	int32_t m_identity;
	int32_t m_code;
	Json::Value m_req_json;
	// 添加其他序列化对象
};

struct Config // 保证均被赋值
{
	std::string m_trade_front;
	std::string m_broker_id;
	int32_t m_query_only; // 接口是否仅供查询
};

#pragma pack( pop )

#endif // TRADER_CTP_STRUCT_CTP_H
