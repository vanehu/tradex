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

#include "../../global/compile.h"

#ifdef TRADER_CTP_COMMUNITY

#ifndef TRADER_CTP_RISKER_CTP_H
#define TRADER_CTP_RISKER_CTP_H

#include "struct_ctp.h"

class TraderCTP_P;

class Risker
{
public:
	Risker( TraderCTP_P* trader_ctp_p ) {};
	~Risker() {};

public:
	void Start( std::string file_path, basicx::CfgBasic* cfg_basic ) {};
	void Stop() {};
	void PrintSessionAssetAccount( std::string username ) {};
	int32_t HandleRiskCtlCheck( std::string asset_account, int32_t func_id, int32_t task_id, Request* request, std::string& risk_msg ) { return 0; };
	void CheckTradeResultForRisk( std::string asset_account, int32_t func_id, int32_t task_id, std::string& results ) {};
	void CommitResult( int32_t code, std::string& data ) {};
};

typedef boost::shared_ptr<Risker> risker_ptr;

#endif // TRADER_CTP_RISKER_CTP_H

#endif // TRADER_CTP_COMMUNITY
