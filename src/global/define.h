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

#ifndef TRADEX_GLOBAL_DEFINE_H
#define TRADEX_GLOBAL_DEFINE_H

// 软件信息
#define DEF_APP_NAME "TradeX" // 系统英文名称
#define DEF_APP_NAME_CN "证 券 交 易 服 务 器" // 系统中文名称
#define DEF_APP_VERSION "V0.1.0-Beta Build 20180601" // 系统版本号
#define DEF_APP_DEVELOPER "Developed by the X-Lab." // 开发者声明
#define DEF_APP_COMPANY "X-Lab (Shanghai) Co., Ltd." // 公司声明
#define DEF_APP_COPYRIGHT "Copyright 2018-2018 X-Lab All Rights Reserved." // 版权声明
#define DEF_APP_HOMEURL "http://www.xlab.com" // 主页链接

#define TRAY_POP_TITLE L"系统消息：" // 托盘气球型提示标题
#define TRAY_POP_START L"TradeX" // 托盘气球型提示启动

// 配置定义
#define CFG_MAX_WORD_LEN 64 // 字串最大字符数
#define CFG_MAX_PATH_LEN 256 // 路径最大字符数

#define NON_WARNING( n ) __pragma( warning( push ) ) __pragma( warning( disable : n ) )
#define USE_WARNING( n ) __pragma( warning( default : n ) ) __pragma( warning( pop ) )

#define GD_ASSET_NAME_LIST_TYPE_BLACK 1 // 黑名单
#define GD_ASSET_NAME_LIST_TYPE_WHITE 2 // 白名单

// 股票类交易功能编号定义
#define TD_FUNC_STOCK_LOGIN                    110001 //用户登录
#define TD_FUNC_STOCK_LOGOUT                   110002 //用户登出
#define TD_FUNC_STOCK_ADDSUB                   110003 //订阅回报
#define TD_FUNC_STOCK_DELSUB                   110004 //退订回报
//////////////////////////////////////////////////////////////
#define TD_FUNC_STOCK_T_SINGLE_ORDER           120001 //单个证券委托下单 //目前直接使用数字
#define TD_FUNC_STOCK_T_SINGLE_CANCEL          120002 //单个证券委托撤单 //目前直接使用数字
#define TD_FUNC_STOCK_T_BATCH_ORDER            120003 //批量证券委托下单 //目前直接使用数字
#define TD_FUNC_STOCK_T_BATCH_CANCEL           120004 //批量证券委托撤单 //目前直接使用数字
//////////////////////////////////////////////////////////////
#define TD_FUNC_STOCK_Q_USER_CAPITAL           130002 //查询客户资金 //目前直接使用数字
#define TD_FUNC_STOCK_Q_USER_POSITION          130004 //查询客户持仓 //目前直接使用数字
#define TD_FUNC_STOCK_Q_USER_ORDER             130005 //查询客户当日委托 //目前直接使用数字
#define TD_FUNC_STOCK_Q_USER_TRANS             130006 //查询客户当日成交 //目前直接使用数字
#define TD_FUNC_STOCK_Q_ETF_BASE_INFO          130008 //查询ETF基本信息 //目前直接使用数字
#define TD_FUNC_STOCK_Q_ETF_STOCKS_INFO        130009 //查询ETF成分股信息 //目前直接使用数字
//////////////////////////////////////////////////////////////
#define TD_FUNC_STOCK_R_ORDER                  190001 //申报回报 //目前直接使用数字
#define TD_FUNC_STOCK_R_TRANS                  190002 //成交回报 //目前直接使用数字
#define TD_FUNC_STOCK_R_CANCEL                 190003 //撤单回报 //目前直接使用数字

// 期货类交易功能编号定义
#define TD_FUNC_FUTURE_LOGIN                   210001 //用户登录
#define TD_FUNC_FUTURE_LOGOUT                  210002 //用户登出
#define TD_FUNC_FUTURE_ADDSUB                  210003 //订阅回报
#define TD_FUNC_FUTURE_DELSUB                  210004 //退订回报
//////////////////////////////////////////////////////////////
#define TD_FUNC_FUTURE_T_SINGLE_ORDER          220001 //单个期货委托下单 //目前直接使用数字
#define TD_FUNC_FUTURE_T_SINGLE_CANCEL         220002 //单个期货委托撤单 //目前直接使用数字
#define TD_FUNC_FUTURE_T_COMBIN_ORDER          220003 //组合期货委托下单 //目前直接使用数字
//////////////////////////////////////////////////////////////
#define TD_FUNC_FUTURE_Q_USER_CAPITAL          230002 //查询客户资金 //目前直接使用数字
#define TD_FUNC_FUTURE_Q_USER_POSITION         230004 //查询客户持仓 //目前直接使用数字
#define TD_FUNC_FUTURE_Q_USER_ORDER            230005 //查询客户当日委托 //目前直接使用数字
#define TD_FUNC_FUTURE_Q_USER_TRANS            230006 //查询客户当日成交 //目前直接使用数字
#define TD_FUNC_FUTURE_Q_INSTRUMENT            230009 //查询期货合约 //目前直接使用数字
#define TD_FUNC_FUTURE_Q_USER_POSITION_DETAIL  230010 //查询客户持仓明细 //目前直接使用数字
//////////////////////////////////////////////////////////////
#define TD_FUNC_FUTURE_R_ORDER                 290001 //报单回报 //目前直接使用数字
#define TD_FUNC_FUTURE_R_TRANS                 290002 //成交回报 //目前直接使用数字

// 资管功能编号定义
#define TD_FUNC_RISKS_CONFIG_READ_STK              110001 // 股票类风控配置读取
//#define TD_FUNC_RISKS_CONFIG_WRITE_STK             110002 // 股票类风控配置写入
#define TD_FUNC_RISKS_NAMELIST_READ_STK            110003 // 股票类风控黑白名单读取
//#define TD_FUNC_RISKS_NAMELIST_WRITE_STK           110004 // 股票类风控黑白名单写入
#define TD_FUNC_RISKS_STOCKLIST_READ_STK           110005 // 股票类风控证券列表读取
//#define TD_FUNC_RISKS_GUBENLIST_READ_STK           110006 // 股票类风控股本列表读取
//#define TD_FUNC_RISKS_PRE_QUOTE_READ_STK           110007 // 股票类风控前日行情读取
#define TD_FUNC_RISKS_STATDATA_RESET_STK           120001 // 股票类风控统计变量重置
#define TD_FUNC_RISKS_ERROR_REPORT_STK             190001 // 股票类风控异常警报推送
#define TD_FUNC_RISKS_STATDATA_REPORT_STK          190002 // 股票类风控统计变量推送
#define TD_FUNC_RISKS_TRANSACTION_REPORT_STK       190003 // 股票类风控成交回报推送
#define TD_FUNC_RISKS_ORDER_REPORT_STK             190004 // 股票类风控委托回报推送
#define TD_FUNC_RISKS_CONFIG_READ_FUE              210001 // 期货类风控配置读取
//#define TD_FUNC_RISKS_CONFIG_WRITE_FUE             210002 // 期货类风控配置写入
//#define TD_FUNC_RISKS_NAMELIST_READ_FUE            210003 // 期货类风控黑白名单读取
//#define TD_FUNC_RISKS_NAMELIST_WRITE_FUE           210004 // 期货类风控黑白名单写入
//#define TD_FUNC_RISKS_STOCKLIST_READ_FUE           210005 // 期货类风控证券列表读取
//#define TD_FUNC_RISKS_GUBENLIST_READ_FUE           210006 // 期货类风控股本列表读取
//#define TD_FUNC_RISKS_PRE_QUOTE_READ_FUE           210007 // 期货类风控前日行情读取
#define TD_FUNC_RISKS_STATDATA_RESET_FUE           220001 // 期货类风控统计变量重置
#define TD_FUNC_RISKS_ERROR_REPORT_FUE             290001 // 期货类风控异常警报推送
#define TD_FUNC_RISKS_STATDATA_REPORT_FUE          290002 // 期货类风控统计变量推送
#define TD_FUNC_RISKS_TRANSACTION_REPORT_FUE       290003 // 期货类风控成交回报推送
#define TD_FUNC_RISKS_ORDER_REPORT_FUE             290004 // 期货类风控委托回报推送
//#define TD_FUNC_ASSET_LOGIN                        910001 // 资管类用户登录
//#define TD_FUNC_ASSET_LOGOUT                       910002 // 资管类用户登出
//#define TD_FUNC_ASSET_USER_READ                    920001 // 资管类产品用户读取
//#define TD_FUNC_ASSET_USER_WRITE                   920002 // 资管类产品用户写入
//#define TD_FUNC_ASSET_INFO_READ                    920003 // 资管类产品信息读取
//#define TD_FUNC_ASSET_INFO_WRITE                   920004 // 资管类产品信息写入
//#define TD_FUNC_ASSET_SUBS_READ                    920005 // 资管类子账户读取
//#define TD_FUNC_ASSET_SUBS_WRITE                   920006 // 资管类子账户写入
//#define TD_FUNC_ASSET_AUTH_READ                    920007 // 资管类用户权限读取
//#define TD_FUNC_ASSET_AUTH_WRITE                   920008 // 资管类用户权限写入
//#define TD_FUNC_ASSET_CONF_READ                    920009 // 资管类产品参数读取
//#define TD_FUNC_ASSET_CONF_WRITE                   920010 // 资管类产品参数写入
//#define TD_FUNC_ASSET_RISK_STK_FUE_READ            920011 // 资管类期现联合风控参数读取
//#define TD_FUNC_ASSET_RISK_STK_FUE_WRITE           920012 // 资管类期现联合风控参数写入
#define TD_FUNC_ASSET_RISK_CHECK_READ              920013 // 资管类风控参数复核读取
//#define TD_FUNC_ASSET_RISK_CHECK_WRITE             920014 // 资管类风控参数复核写入

// 风控复核编号定义
//#define TD_FUNC_RISKS_CONFIG_CHECK_STK                  1 // 股票类风控配置复核
//#define TD_FUNC_RISKS_CONFIG_CHECK_FUE                  2 // 期货类风控配置复核
//#define TD_FUNC_RISKS_NAMELIST_CHECK_STK                3 // 股票类风控黑白名单复核
//#define TD_FUNC_RISKS_NAMELIST_CHECK_FUE                4 // 期货类风控黑白名单复核
//#define TD_FUNC_ASSET_RISK_STK_FUE_CHECK                5 // 资管类期现联合风控参数复核

#endif // TRADEX_GLOBAL_DEFINE_H
