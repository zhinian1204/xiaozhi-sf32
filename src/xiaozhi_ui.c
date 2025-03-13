#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "littlevgl2rtt.h"
#include "lv_tiny_ttf.h"

static struct rt_semaphore update_ui_sema;
/*Create style with the new font*/
static lv_style_t style;
extern const unsigned char FZYTK_font[];
extern const int FZYTK_font_size;

void xiaozhi_ui_update(char *string)
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        //Clean screen
        lv_obj_clean(lv_screen_active());


        lv_obj_t *label1 = lv_label_create(lv_screen_active());
        lv_label_set_long_mode(label1, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
        lv_label_set_text(label1, "Recolor \u4E0D\u4E0D\u652f\u6301 v9 now.");
        lv_obj_add_style(label1, &style, 0);
        lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
        lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);

        lv_obj_t *label2 = lv_label_create(lv_screen_active());
        lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
        lv_obj_set_width(label2, LV_HOR_RES_MAX);
        lv_label_set_text(label2, string);
        lv_obj_add_style(label2, &style, 0);

        lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    }



    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_task(void *args)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t ms;

    rt_sem_init(&update_ui_sema, "update_ui", 1, RT_IPC_FLAG_FIFO);

    /* init littlevGL */
    ret = littlevgl2rtt_init("lcd");
    if (ret != RT_EOK)
    {
        return;
    }



    lv_style_init(&style);
    lv_font_t *font = lv_tiny_ttf_create_data(FZYTK_font, FZYTK_font_size, 30);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);


    xiaozhi_ui_update("Ready");

    while (1)
    {
        if (RT_EOK == rt_sem_trytake(&update_ui_sema))
        {
            ms = lv_task_handler();
            rt_thread_mdelay(ms);
            rt_sem_release(&update_ui_sema);
        }

    }

}





