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

#ifdef TRADER_APE_COMMUNITY

#ifndef TRADER_APE_RISKER_APE_H
#define TRADER_APE_RISKER_APE_H

#include "struct_ape.h"

class TraderAPE_P;

class Risker
{
public:
	Risker( TraderAPE_P* trader_ape_p ) {};
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

#endif // TRADER_APE_RISKER_APE_H

#endif // TRADER_APE_COMMUNITY
