#include "rtthread.h"
#include "rgbled_mcp.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "drivers/rt_drv_pwm.h"
#include "bf0_hal.h"

// 定义针脚
#define LED_PIN 32

RGBLEDController& GetRGBLEDController() {
    static RGBLEDController instance;
    return instance;
}
// 当前是否循环变色状态标志
bool RGBLEDTool::is_color_cycling_ = false;


// 循环变色
// void rgb_color_array_display()
// {
//     uint16_t i = 0;
//     rt_kprintf("start display color!\n");
//     while (1)
//     {
//         if (i < sizeof(rgb_color_arry) / sizeof(struct rt_color))
//         {
//             rt_kprintf("-> %s\n", rgb_color_arry[i].color_name);
//             rgb_led_set_color(rgb_color_arry[i].color);
//             rt_thread_mdelay(500);
//         }
//         i++;
//         if (i >= 8)
//             i = 0;
//     }
// }

// 循环变色线程入口
void RGBLEDTool::ColorCycleThreadEntry(void *param)
{
    uint16_t index = 0;
    rt_kprintf("start display color!\n");
    while (is_color_cycling_)
    {
        if (index < sizeof(rgb_color_arry) / sizeof(struct rt_color))
        {
            // rt_kprintf("-> %s\n", rgb_color_arry[index].color_name);
            // rgb_led_set_color(rgb_color_arry[i].color);
            uint32_t color = rgb_color_arry[index].color;
            GetRGBLEDController().SetColor(color);
            rt_thread_mdelay(500);
        }
        index++;
        if (index >= 8)
            index = 0;
    }
}

// 检查LED是否点亮
bool RGBLEDTool::IsLightOn()
{
    // 如果正在循环变色，则认为灯是开启状态
    return is_color_cycling_;
}

void RGBLEDTool::RegisterRGBLEDTool(McpServer *server)
{
    // 注册循环变色工具
    server->AddTool("self.led.turn_on_the_light", "turn on the light.",
                    PropertyList(),
                    [](const PropertyList &) -> ReturnValue
                    {
                        if (is_color_cycling_)
                            return true;
                        is_color_cycling_ = true;
                        GetRGBLEDController().SetColor(rgb_color_arry[4].color);

                        // rt_thread_t thread =
                        //     rt_thread_create("rgb_cycle", ColorCycleThreadEntry,
                        //                      nullptr, 1024, 10, 10);
                        // if (thread)
                        //     rt_thread_startup(thread);
                        return true;
                    });

    // 注册关闭工具
    server->AddTool(
        "self.led.turn_off_the_light", "turn off the light.", PropertyList(),
        [](const PropertyList &) -> ReturnValue
        {
            is_color_cycling_ = false;
            rt_thread_mdelay(100);

            GetRGBLEDController().SetColor(rgb_color_arry[0].color); // 关闭时设置为黑色
            return true;
        });
    // 注册获取灯光状态工具
    server->AddTool("self.led.get_light_status",
                    "Get the current status of the LED (on or off).",
                    PropertyList(), [](const PropertyList &) -> ReturnValue
                    { return RGBLEDTool::IsLightOn(); });
}
