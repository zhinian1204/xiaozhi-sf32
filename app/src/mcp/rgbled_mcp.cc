#include "rtthread.h"
#include "rgbled_mcp.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "drivers/rt_drv_pwm.h"
#include "bf0_hal.h"

#define LED_PIN 32 
RGBLEDController& GetRGBLEDController() {
    static RGBLEDController instance;
    return instance;
}

void rgb_led_set_color(uint32_t color)
{
    struct rt_rgbled_configuration configuration;
    configuration.color_rgb = color;
    rt_device_control(rgbled_device, PWM_CMD_SET_COLOR, &configuration);
}

void rgb_color_array_display()
{
    uint16_t i = 0;
    rt_kprintf("start display color!\n");
    while (1)
    {
        if (i < sizeof(rgb_color_arry) / sizeof(struct rt_color))
        {
            rt_kprintf("-> %s\n", rgb_color_arry[i].color_name);
            rgb_led_set_color(rgb_color_arry[i].color);
            rt_thread_mdelay(1000);

        }
        i++;
        if (i >= 8)
            i = 0;

    }
}

bool RGBLEDTool::is_color_cycling_ = false;
void RGBLEDTool::ColorCycleThreadEntry(void* param) {
    int index = 0;
    // uint32_t toggle = 0; // 用于52J板子的LED切换状态
    while (is_color_cycling_) {
#if defined(BSP_USING_BOARD_SF32LB52_NANO_52J) || defined(BSP_USING_BOARD_SF32LB52_XTY_AI_THT)
        // 对于52J板子，控制PA32引脚以1秒周期亮灭
        // rt_pin_write(LED_PIN, toggle ? PIN_HIGH : PIN_LOW);
        // toggle = !toggle;
        // rt_thread_mdelay(1000);
        
        rgb_color_array_display();

#else
        uint32_t color = rgb_color_arry[index].color;
        GetRGBLEDController().SetColor(color);
        index = (index + 1) % (sizeof(rgb_color_arry)/sizeof(rgb_color_arry[0]));
        rt_thread_mdelay(500);
#endif
    }
}
bool RGBLEDTool::IsLightOn() {
    // 如果正在循环变色，则认为灯是开启状态
    return is_color_cycling_;
}
void RGBLEDTool::RegisterRGBLEDTool(McpServer* server) {
    // 循环变色工具
        server->AddTool(
            "self.led.turn_on_the_light",
            "turn on the light.",
            PropertyList(),
            [](const PropertyList&) -> ReturnValue {
            if (is_color_cycling_) return true;
            is_color_cycling_ = true;
#if defined(BSP_USING_BOARD_SF32LB52_NANO_52J) || defined(BSP_USING_BOARD_SF32LB52_XTY_AI_THT)
            // 配置PA32为GPIO输出模式并输出低电平（点亮）
            // rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
            // rt_pin_write(LED_PIN, PIN_LOW);
            rgb_led_set_color(rgb_color_arry[7].color);

#endif      
            rt_thread_t thread = rt_thread_create("rgb_cycle",
                            ColorCycleThreadEntry, 
                            nullptr,
                            1024,
                            10,
                            10);
            if (thread) rt_thread_startup(thread);
            return true;
    }
    );

    server->AddTool(
        "self.led.turn_off_the_light",
        "turn off the light.",
        PropertyList(),
        [](const PropertyList&) -> ReturnValue {
            is_color_cycling_ = false;
            rt_thread_mdelay(100); 
#if defined(BSP_USING_BOARD_SF32LB52_NANO_52J) || defined(BSP_USING_BOARD_SF32LB52_XTY_AI_THT)
            // 对于52J板子，控制PA32引脚输出高电平熄灭LED
            // rt_pin_write(LED_PIN, PIN_HIGH);
            rgb_led_set_color(rgb_color_arry[0].color);
#else
            
            GetRGBLEDController().SetColor(0x000000);  // 关灯
#endif
            return true;
        }
    );

    server->AddTool(
        "self.led.get_light_status",
        "Get the current status of the LED (on or off).",
        PropertyList(),
        [](const PropertyList&) -> ReturnValue {
            return RGBLEDTool::IsLightOn();
        }
    );
}
