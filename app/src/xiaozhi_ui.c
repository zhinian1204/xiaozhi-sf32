#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "littlevgl2rtt.h"
#include "lv_tiny_ttf.h"
#include "string.h"

#include "bf0_pm.h"
#include "gui_app_pm.h"
#include "drv_gpio.h"
#include "lv_timer.h"
#include "lv_display.h"
#include "lv_obj_pos.h"
#include "ulog.h"
#include "drv_flash.h"
#include "xiaozhi2.h"
#define IDLE_TIME_LIMIT  (30000)
#define SHOW_TEXT_LEN 150
#define LCD_DEVICE_NAME  "lcd"
#define TOUCH_NAME  "touch"
static struct rt_semaphore update_ui_sema;
/*Create style with the new font*/
static lv_style_t style;
extern const unsigned char droid_sans_fallback_font[];
extern const int droid_sans_fallback_font_size;

extern const lv_image_dsc_t neutral;
extern const lv_image_dsc_t happy;
extern const lv_image_dsc_t laughing;
extern const lv_image_dsc_t funny;
extern const lv_image_dsc_t sad;
extern const lv_image_dsc_t angry;
extern const lv_image_dsc_t crying;
extern const lv_image_dsc_t loving;
extern const lv_image_dsc_t embarrassed;
extern const lv_image_dsc_t surprised;
extern const lv_image_dsc_t shocked;
extern const lv_image_dsc_t thinking;
extern const lv_image_dsc_t winking;
extern const lv_image_dsc_t cool;
extern const lv_image_dsc_t relaxed;
extern const lv_image_dsc_t delicious;
extern const lv_image_dsc_t kissy;
extern const lv_image_dsc_t confident;
extern const lv_image_dsc_t sleepy;
extern const lv_image_dsc_t silly;
extern const lv_image_dsc_t confused;


extern const lv_image_dsc_t ble;//ble
extern const lv_image_dsc_t ble_close;

static lv_obj_t *global_label1;
static lv_obj_t *global_label2;

static lv_obj_t *global_img;



static lv_obj_t *global_img_ble;

static rt_timer_t g_split_text_timer = RT_NULL;
static char g_second_part[512];
static lv_obj_t *g_label_for_second_part = NULL;
//xiaozhi2
extern rt_mailbox_t g_button_event_mb;
extern xiaozhi_ws_t g_xz_ws;
extern void ws_send_speak_abort(void *ws, char *session_id, int reason);
extern void ws_send_listen_start(void *ws, char *session_id, enum ListeningMode mode);
extern void ws_send_listen_stop(void *ws, char *session_id);
void set_position_by_percentage(lv_obj_t * obj, int x_percent, int y_percent) {
    // Gets the width and height of the screen resolution
    int screen_width = lv_disp_get_hor_res(NULL);
    int screen_height = lv_disp_get_ver_res(NULL);

    // Calculate the sitting of the target position
    int target_x = (screen_width * x_percent) / 100;
    int target_y = (screen_height * y_percent) / 100;

    // Sets the location of the object
    lv_obj_set_pos(obj, target_x, target_y);
}

rt_err_t xiaozhi_ui_obj_init(void)
{
    LV_IMAGE_DECLARE(neutral);
    LV_IMAGE_DECLARE(happy);
    LV_IMAGE_DECLARE(laughing);
    LV_IMAGE_DECLARE(funny);
    LV_IMAGE_DECLARE(sad);
    LV_IMAGE_DECLARE(angry);
    LV_IMAGE_DECLARE(crying);
    LV_IMAGE_DECLARE(loving);
    LV_IMAGE_DECLARE(embarrassed);
    LV_IMAGE_DECLARE(surprised);
    LV_IMAGE_DECLARE(shocked);
    LV_IMAGE_DECLARE(thinking);
    LV_IMAGE_DECLARE(winking);
    LV_IMAGE_DECLARE(cool);
    LV_IMAGE_DECLARE(relaxed);
    LV_IMAGE_DECLARE(delicious);
    LV_IMAGE_DECLARE(kissy);
    LV_IMAGE_DECLARE(confident);
    LV_IMAGE_DECLARE(sleepy);
    LV_IMAGE_DECLARE(silly);
    LV_IMAGE_DECLARE(confused);

    LV_IMAGE_DECLARE(ble);
    LV_IMAGE_DECLARE(ble_close);

    global_img_ble = lv_img_create(lv_screen_active());//ble
    lv_img_set_src(global_img_ble, &ble);
    lv_obj_align(global_img_ble, LV_ALIGN_TOP_LEFT, 40, 0);
    

    global_img = lv_img_create(lv_screen_active());//emoji
    lv_img_set_src(global_img, &neutral);
    lv_obj_align(global_img, LV_ALIGN_CENTER, 0, -40);
    
    
    global_label1 = lv_label_create(lv_screen_active());//top text

    lv_label_set_long_mode(global_label1, LV_LABEL_LONG_SCROLL_CIRCULAR); 
    lv_obj_add_style(global_label1, &style, 0);
    lv_obj_set_width(global_label1, 150);
    lv_obj_set_style_text_align(global_label1,LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(global_label1, LV_ALIGN_TOP_MID, 0, 0);
    

    global_label2 = lv_label_create(lv_screen_active());//output text

    lv_label_set_long_mode(global_label2, LV_LABEL_LONG_WRAP);  /*Break the long lines*/
    lv_obj_add_style(global_label2, &style, 0);
    lv_obj_set_width(global_label2, LV_HOR_RES_MAX);
    lv_obj_set_style_text_align(global_label2,LV_TEXT_ALIGN_CENTER, 0);
    
    lv_obj_align_to(global_label2, global_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

   
    return RT_EOK;
}

void xiaozhi_ui_chat_status(char *string)//top text
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        lv_label_set_text(global_label1, string);

    }

    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_chat_output(char *string)
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        lv_label_set_text(global_label2, string);
#ifdef BSP_USING_PM
        lv_display_trigger_activity(NULL);
#endif // BSP_USING_PM
    }

    rt_sem_release(&update_ui_sema);
}

static void switch_to_second_part(void *parameter)
{
    if (g_label_for_second_part && strlen(g_second_part) > 0)
    {
        rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);
        lv_label_set_text(g_label_for_second_part, g_second_part);
        rt_sem_release(&update_ui_sema);
        // 清空内容，避免重复触发
        memset(g_second_part, 0, sizeof(g_second_part));
        g_label_for_second_part = NULL;
    }
}
void xiaozhi_ui_tts_output(char *string)
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
       int len = strlen(string);
        rt_kprintf("len == %d\n", len);

        if (len > SHOW_TEXT_LEN)
        {
            // 查看 SHOW_TEXT_LEN 是否落在一个多字节字符中间
            int cut_pos = SHOW_TEXT_LEN;

            // 向前调整到完整的 UTF-8 字符起点
            while (cut_pos > 0 && ((unsigned char)string[cut_pos] & 0xC0) == 0x80)
            {
                cut_pos--;
            }

            if (cut_pos == 0) // 找不到合适的截断点，直接截断
                cut_pos = SHOW_TEXT_LEN;

            // 截取第一部分
            char first_part[SHOW_TEXT_LEN + 1];
            strncpy(first_part, string, cut_pos);
            first_part[cut_pos] = '\0'; // 确保字符串结束

            // 剩余部分从 cut_pos 开始
            strncpy(g_second_part, string + cut_pos, sizeof(g_second_part) - 1);
            g_second_part[sizeof(g_second_part) - 1] = '\0'; // 确保结尾
            g_label_for_second_part = global_label2;

            lv_label_set_text(global_label2, first_part);
#ifdef BSP_USING_PM
            lv_display_trigger_activity(NULL);
#endif // BSP_USING_PM

            // 创建定时器
           if (!g_split_text_timer)
            {
               g_split_text_timer = rt_timer_create("next_text",
                                    switch_to_second_part,
                                    NULL,
                                    rt_tick_from_millisecond(10000),
                                    RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
            }
            else
            {
                rt_timer_stop(g_split_text_timer); // 停止旧定时器

                // 设置新的超时时间
                rt_tick_t timeout_tick = rt_tick_from_millisecond(10000);
                rt_timer_control(g_split_text_timer, RT_TIMER_CTRL_SET_TIME, &timeout_tick);
            }
            rt_timer_start(g_split_text_timer);
        }
        else
        {
            lv_label_set_text(global_label2, string);
#ifdef BSP_USING_PM
            lv_display_trigger_activity(NULL);
#endif // BSP_USING_PM
        }
    }

    rt_sem_release(&update_ui_sema);
}
void xiaozhi_ui_update_emoji(char *string)//emoji
{
    
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string) {
        if (strcmp(string, "neutral") == 0) {
            lv_img_set_src(global_img, &neutral);
        } else if (strcmp(string, "happy") == 0) {
            lv_img_set_src(global_img, &happy);
        } else if (strcmp(string, "laughing") == 0) {
            lv_img_set_src(global_img, &laughing);
        } else if (strcmp(string, "funny") == 0) {
            lv_img_set_src(global_img, &funny);
        } else if (strcmp(string, "sad") == 0) {
            lv_img_set_src(global_img, &sad);
        } else if (strcmp(string, "angry") == 0) {
            lv_img_set_src(global_img, &angry);
        } else if (strcmp(string, "crying") == 0) {
            lv_img_set_src(global_img, &crying);
        } else if (strcmp(string, "loving") == 0) {
            lv_img_set_src(global_img, &loving);
        } else if (strcmp(string, "embarrassed") == 0) {
            lv_img_set_src(global_img, &embarrassed);
        } else if (strcmp(string, "surprised") == 0) {
            lv_img_set_src(global_img, &surprised);
        } else if (strcmp(string, "shocked") == 0) {
            lv_img_set_src(global_img, &shocked);
        } else if (strcmp(string, "thinking") == 0) {
            lv_img_set_src(global_img, &thinking);
        } else if (strcmp(string, "winking") == 0) {
            lv_img_set_src(global_img, &winking);
        } else if (strcmp(string, "cool") == 0) {
            lv_img_set_src(global_img, &cool);
        } else if (strcmp(string, "relaxed") == 0) {
            lv_img_set_src(global_img, &relaxed);
        } else if (strcmp(string, "delicious") == 0) {
            lv_img_set_src(global_img, &delicious);
        } else if (strcmp(string, "kissy") == 0) {
            lv_img_set_src(global_img, &kissy);
        } else if (strcmp(string, "confident") == 0) {
            lv_img_set_src(global_img, &confident);
        } else if (strcmp(string, "sleepy") == 0) {
            lv_img_set_src(global_img, &sleepy);
        } else if (strcmp(string, "silly") == 0) {
            lv_img_set_src(global_img, &silly);
        } else if (strcmp(string, "confused") == 0) {
            lv_img_set_src(global_img, &confused);
        } else {
            lv_img_set_src(global_img, &neutral); // common emoji is neutral
        }
    }
    

    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_update_ble(char *string)//ble
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if(string)
    {
        if(strcmp(string,"open") == 0)
        {
            lv_img_set_src(global_img_ble, &ble);
        }
        else if(strcmp(string,"close") == 0)
        {
            lv_img_set_src(global_img_ble, &ble_close);
        }
    }

    rt_sem_release(&update_ui_sema);
}


static rt_device_t lcd_device;
static void pm_event_handler(gui_pm_event_type_t event)
{
    LOG_I("in pm_event_handle"); 
    switch (event)
     {
     case GUI_PM_EVT_SUSPEND:
     {
        LOG_I("in GUI_PM_EVT_SUSPEND"); 
        lv_timer_enable(false);
        break;
     }
     case GUI_PM_EVT_RESUME:
     {
         lv_timer_enable(true);
         break;
     }
     default:
     {
         RT_ASSERT(0);
     }
     }
} 
void pm_ui_init()
{
    
    int8_t wakeup_pin;
    uint16_t gpio_pin;
    GPIO_TypeDef *gpio;

    gpio = GET_GPIO_INSTANCE(34);
    gpio_pin = GET_GPIOx_PIN(34);

    wakeup_pin = HAL_HPAON_QueryWakeupPin(gpio, gpio_pin);
    RT_ASSERT(wakeup_pin >= 0);
    
    lcd_device = rt_device_find(LCD_DEVICE_NAME);
    if(lcd_device==RT_NULL)
    {
        LOG_I("lcd_device!=NULL!");
        RT_ASSERT(0);
    }
#ifdef BSP_USING_PM
    pm_enable_pin_wakeup(wakeup_pin, AON_PIN_MODE_DOUBLE_EDGE);
#endif
    gui_ctx_init();
    gui_pm_init(lcd_device, pm_event_handler);

}


void xiaozhi_ui_task(void *args)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t ms;
    static rt_device_t touch_device;

    rt_sem_init(&update_ui_sema, "update_ui", 1, RT_IPC_FLAG_FIFO);

    /* init littlevGL */
    ret = littlevgl2rtt_init("lcd");
    if (ret != RT_EOK)
    {
        return;
    }

    touch_device = rt_device_find(TOUCH_NAME);
    if(touch_device==RT_NULL)
    {
        LOG_I("touch_device!=NULL!");
        RT_ASSERT(0);
    }
    rt_device_control(touch_device, RTGRAPHIC_CTRL_POWEROFF, NULL);

#ifdef BSP_USING_PM
    pm_ui_init();
#endif
    

    lv_style_init(&style);
    lv_font_t *font = lv_tiny_ttf_create_data(droid_sans_fallback_font, droid_sans_fallback_font_size, 30);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);//SET BG COLOR

    ret = xiaozhi_ui_obj_init();
    if (ret != RT_EOK)
    {
        return;
    }

    xiaozhi_ui_update_ble("close");
    xiaozhi_ui_chat_status("连接中...");
    xiaozhi_ui_chat_output("等待连接...");
    xiaozhi_ui_update_emoji("neutral");

    while (1)
    {
        uint32_t btn_event;
        if (rt_mb_recv(g_button_event_mb, &btn_event, 0) == RT_EOK) 
        {
            rt_kprintf("button event: %d\n",btn_event);
            switch (btn_event) {
                case BUTTON_EVENT_PRESSED:
                    //if (g_state == kDeviceStateSpeaking) 
                    {
                        ws_send_speak_abort(&g_xz_ws.clnt, g_xz_ws.session_id, kAbortReasonWakeWordDetected);
                        xz_speaker(0); // 关闭扬声器
                    }
                    ws_send_listen_start(&g_xz_ws.clnt, g_xz_ws.session_id, kListeningModeManualStop);
                    xiaozhi_ui_chat_status("聆听中...");
                    xz_mic(1);
                    break;

                case BUTTON_EVENT_RELEASED:
                    xiaozhi_ui_chat_status("待命中...");
                    ws_send_listen_stop(&g_xz_ws.clnt, g_xz_ws.session_id);
                    xz_mic(0);
                    break;

                default:
                    break;
            }
        }

        if (RT_EOK == rt_sem_trytake(&update_ui_sema))
        {
            ms = lv_task_handler();

            char * current_text=lv_label_get_text(global_label1);
    /*
            if (current_text) 
            {
                rt_kprintf("Label text: %s\n", current_text);
            }
    */      
#ifdef BSP_USING_PM
            if(strcmp(current_text, "聆听中...") == 0)
            {
                lv_display_trigger_activity(NULL);
            }
            if (lv_display_get_inactive_time(NULL) > IDLE_TIME_LIMIT)
            {
                LOG_I("10s no action \n");
                gui_pm_fsm(GUI_PM_ACTION_SLEEP);
            
            }
        
            if (gui_is_force_close())
            {
                LOG_I("in force_close");
                bool lcd_drawing;
                rt_device_control(lcd_device, RTGRAPHIC_CTRL_GET_BUSY, &lcd_drawing);
                if (!lcd_drawing)
                {
                    LOG_I("no input:%d", lv_display_get_inactive_time(NULL));
                    gui_suspend();
                    LOG_I("ui resume");
                    /* force screen to redraw */
                    lv_obj_invalidate(lv_screen_active());
                    /* reset activity timer */
                    lv_display_trigger_activity(NULL);
    
                }
            
            }
#endif // BSP_USING_PM

            rt_thread_mdelay(ms);
            rt_sem_release(&update_ui_sema);
        }

    }

}





