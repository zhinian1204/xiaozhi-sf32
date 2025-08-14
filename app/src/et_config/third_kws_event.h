#ifndef _THIRD_KWS_EVENT_H__
#define _THIRD_KWS_EVENT_H__

#include <stdio.h>
#include <stdint.h>

#if 1//THIRD_UI_LIGHT_EN

typedef enum
{
    UI_EVENT_START = 0x000,
    UI_EVENT_XIAO_FAN_XIAO_FAN, //小范小范  1
    UI_EVENT_FAN_HUI_BIAO_PAN,  //返回表盘  2
    UI_EVENT_DA_KAI_YING_YONG_SHI_CHANG,    //打开应用市场  3
    UI_EVENT_DA_KAI_YOU_XI_SHI_CHANG,   //打开游戏市场  4
    UI_EVENT_DA_KAI_BIAO_PAN_SHI_CHANG, //打开表盘市场  5
    UI_EVENT_DA_KAI_JING_MEI_BIAO_PAN,  //打开精美表盘  6
    UI_EVENT_DA_KAI_FAN_YI_GUAN,    //打开翻译官  7
    UI_EVENT_DA_KAI_XI_MA_LA_YA,    //打开喜马拉雅  8
    UI_EVENT_DA_KAI_DOU_YIN,    //打开抖音  9
    UI_EVENT_DA_KAI_BAI_DU_DAO_HANG,    //打开百度导航  10
    UI_EVENT_DA_KAI_YU_YIN_BEI_WANG_LU, //打开语音备忘录  11
    UI_EVENT_DA_KAI_YUN_DONG,   //打开运动  12
    UI_EVENT_DA_KAI_SHUI_MIAN,  //打开睡眠  13
    UI_EVENT_DA_KAI_XIN_LV, //打开心率  14
    UI_EVENT_DA_KAI_XUE_YANG,   //打开血氧  15
    UI_EVENT_DA_KAI_ZHI_FU_BAO, //打开支付宝  16
    UI_EVENT_DA_KAI_WEI_XIN_ZHI_FU, //打开微信支付  17
    UI_EVENT_DA_KAI_TIAN_QI,    //打开天气  18
    UI_EVENT_DA_KAI_RI_LI,  //打开日历  19
    UI_EVENT_DA_KAI_ZHI_NAN_ZHEN,   //打开指南针  20
    UI_EVENT_DA_KAI_DIAN_HUA,   //打开电话  21
    UI_EVENT_DA_KAI_YIN_YUE,    //打开音乐  22
    UI_EVENT_CHA_ZHAO_SHE_BEI,  //查找设备  23
    UI_EVENT_DA_KAI_MIAO_BIAO,  //打开秒表  24
    UI_EVENT_DA_KAI_XIN_XI, //打开信息  25
    UI_EVENT_DA_KAI_NAO_ZHONG,  //打开闹钟  26
    UI_EVENT_DA_KAI_JI_SUAN_QI, //打开计算器  27
    UI_EVENT_DA_KAI_JI_SHI_QI,  //打开计时器  28
    UI_EVENT_DA_KAI_SHE_ZHI,    //打开设置  29
    UI_EVENT_DA_KAI_LIAO_TIAN_MO_SHI,   //打开聊天模式  30
    UI_EVENT_DA_KAI_WEN_DA_MO_SHI,  //打开问答模式  31
    UI_EVENT_DA_KAI_XUE_YA, //打开血压  32
    UI_EVENT_MAX
} action_youjie;

#endif // THIRD_UI_LIGHT_EN

#endif
