#ifndef RGBLED_MCP_H
#define RGBLED_MCP_H

#include "mcp_server.h"
#include "rtthread.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "drivers/rt_drv_pwm.h"
#include "bf0_hal.h"

// 定义颜色常量
#define RGBLED_NAME "rgbled"

// 定义颜色结构体
struct rt_color
{
    const char *color_name;
    uint32_t color;
};

// 定义颜色数组
static struct rt_color rgb_color_arry[] = {
    {"black", 0x000000},  {"blue", 0x0000ff}, {"green", 0x00ff00},
    {"cyan", 0x00ffff},   {"red", 0xff0000},  {"purple", 0xff00ff},
    {"yellow", 0xffff00}, {"white", 0xffffff}};

// RGB LED控制类
class RGBLEDController {
public:
    RGBLEDController() {
        Init();
    }

    void SetColor(uint32_t color) {
        struct rt_rgbled_configuration configuration;
        configuration.color_rgb = color;
        rt_kprintf("Setting RGB LED color: 0x%06X\n", color);
        rt_device_control(rgbled_device_, PWM_CMD_SET_COLOR, &configuration);
    }

private:
    rt_device_t rgbled_device_;

    void Init() {
        HAL_PIN_Set(PAD_PA32, GPTIM2_CH1, PIN_NOPULL, 1);   // RGB LED 52x  pwm3_cc1
        /*rgbled poweron*/
        // HAL_PMU_ConfigPeriLdo(PMU_PERI_LDO3_3V3, true, true);

        rgbled_device_ = rt_device_find(RGBLED_NAME); //find rgb
        if (!rgbled_device_) {
            RT_ASSERT(0);  // 断言失败，终止程序执行
        }
    }
};

// 获取RGB LED控制器实例
class RGBLEDTool
{
public:
    static void RegisterRGBLEDTool(McpServer *server); // 注册RGB LED工具
    static bool is_color_cycling_;                     // 是否正在循环变色
    static void ColorCycleThreadEntry(void *param);    // 循环变色线程入口
    static bool IsLightOn();                           // 灯光状态
};

RGBLEDController& GetRGBLEDController();

#endif // RGBLED_MCP_H