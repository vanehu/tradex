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

#ifndef TRADEX_GLOBAL_COMPILE_H
#define TRADEX_GLOBAL_COMPILE_H

//---------- 基础组件 ----------//

#define TRADEX_GLOBAL_EXP
//#define TRADEX_GLOBAL_IMP

#define TRADEX_SHARES_EXP
//#define TRADEX_SHARES_IMP

//---------- 设置结束 ----------//

#ifdef TRADEX_GLOBAL_EXP
    #define TRADEX_GLOBAL_EXPIMP __declspec(dllexport)
#endif

#ifdef TRADEX_GLOBAL_IMP
    #define TRADEX_GLOBAL_EXPIMP __declspec(dllimport)
#endif

//------------------------------//

#ifdef TRADEX_SHARES_EXP
    #define TRADEX_SHARES_EXPIMP __declspec(dllexport)
#endif

#ifdef TRADEX_SHARES_IMP
    #define TRADEX_SHARES_EXPIMP __declspec(dllimport)
#endif

//------------------------------//

#endif // TRADEX_GLOBAL_COMPILE_H
