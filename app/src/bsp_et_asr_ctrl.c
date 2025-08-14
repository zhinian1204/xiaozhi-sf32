#if 1 //ET_WAKEUP_EN
#include <rtthread.h>

#include <string.h>
#include <stdlib.h>
#include "bsp_et_asr_ctrl.h"
#include "bsp_board.h"
#include "bf0_sys_cfg.h"
#include "xiaozhi_public.h"
extern void  simulate_button_pressed();
#if 1
    #include <lib_et_asr.h>
    #if ET_ASR_UI_PRINT_STATES_EN == 1
        #include "et_config/et_asr_default_state.h"
    #endif //ET_ASR_UI_PRINT_STATES_EN

    #include "et_config/et_keyword.h"
    #include "et_config/et_weights.h"
    #include "et_config/et_thres_list.h"
    #if ET_ASR_KWS_LIST_BIN_EN
        #include "et_config/et_kws_labels.h"
    #endif
    #if ET_COLLECT_BYTES_BIN_EN
        #include "et_config/et_collects.h"
    #endif
#endif

#if 0
    extern void *et_res_file_open(char *path, int *length);
    extern int et_res_file_close(void *addr);
#endif

#if 1//THIRD_UI_LIGHT_EN
    #include "et_config/third_kws_event.h"
    //#include "third_func.h"

#endif // THIRD_UI_LIGHT_EN

void et_func_kws_event(int event)
{
#if 1
    if (event <= 0 || event >= 255)
    {
        return;
    }
#else
    if (event == 0)
    {
        return;
    }
#endif // 0
#if 1//ET_LOG_OPEN
    printf("ctrl event:%d\n", event);
#endif // ET_LOG_OPEN

    // 检测到唤醒词后的处理逻辑
    printf("xiaozhi hx ok: %d\n", event);
    simulate_button_pressed();
    // 这里可以添加具体的业务逻辑
    // 比如：播放提示音、启动录音、发送网络请求等
    
#if 0
    if (youjie_offline_using_type_get() == 2)
    {
        return;
    }
#endif //0
#if 0
    //UI su du bu gou, shi bie Shi Fang Dian Suan Li
    //160ms
    set_et_asr_ignore_cnt(4);
#endif //0
#if 0//THIRD_UI_LIGHT_EN
    //third_kws_event(event);
    set_et_kws_event(event);
    //msg_enqueue(EVT_ET_FUNC_KWS_DEAL);
#endif // THIRD_UI_LIGHT_EN
}

uint32_t et_asr_get_time_cnt(void)
{
    return rt_tick_get();
}

et_asr_kws_cfg_t et_kws_cfg;
#define ET_SDK_BASE_VERSION "202408271131"


//#define ET_BIN_ET_KEYWORD_PATH   "/preset_user/yj/et_keyword.bin"
void et_bsp_ctrl_main_init()
{
    printf("-et_bsp_ctrl_main_init(),start-\n");
    printf("base[v:%s]\n", ET_SDK_BASE_VERSION);
//    printf("ui[name:%s, v:%s]\n", ET_UI_PROJ_NAME, ET_UI_VERSION);
    memset(&et_kws_cfg, 0, sizeof(et_asr_kws_cfg_t));
#if 0
    int len = 0;
    et_kws_cfg.keyFile = et_res_file_open(ET_BIN_ET_KEYWORD_PATH, &len);
    et_kws_cfg.keyLen = len;
    printf("keyFile:%x, keyLen:%d\n", (uint32_t)et_kws_cfg.keyFile, et_kws_cfg.keyLen);
#else
    et_kws_cfg.keyFile = ET_ASR_KEYWORD_BIG_STR;
    et_kws_cfg.keyLen = strlen(ET_ASR_KEYWORD_BIG_STR);
    printf("keyFile:%x, keyLen:%d\n", (uint32_t)et_kws_cfg.keyFile, et_kws_cfg.keyLen);
#endif

    et_kws_cfg.weightFile = (char *)et_asr_weights_buf;
    et_kws_cfg.weightLen = ET_ASR_WEIGHTS_BUF_LEN;
    et_kws_cfg.thresListFile = (char *)ET_ASR_THRES_LIST_BIG_STR;
    et_kws_cfg.thresListLen = strlen(ET_ASR_THRES_LIST_BIG_STR);
#if ET_ASR_KWS_LIST_BIN_EN
    et_kws_cfg.kwsLabelsFile = (char *)et_asr_kws_labels_buf;
    et_kws_cfg.kwsLabelsLen = ET_ASR_KWS_LABELS_BUF_LEN;
#endif // ET_ASR_KWS_LIST_BIN_EN
#if ET_COLLECT_BYTES_BIN_EN
    et_kws_cfg.collectFile = (char *)et_asr_collects_buf;
    et_kws_cfg.collectLen = ET_ASR_COLLECTS_BUF_LEN;
#endif //



#if ET_USED_USBKEY
    et_kws_cfg.c_soft_key = (char *)xcfg_cb.soft_key;
#else
    et_kws_cfg.c_soft_key = NULL;
#endif // ET_USED_USBKEY

    et_kws_cfg.time_cb = et_asr_get_time_cnt;

    et_hm_parm_cfg_t t_hm1_cfg;
    memset(&t_hm1_cfg, 0, sizeof(et_hm_parm_cfg_t));
    t_hm1_cfg.et_db_thd = 40;
    t_hm1_cfg.et_hm_max_thd = 14;
    t_hm1_cfg.et_hm_min_thd = 8;
    t_hm1_cfg.et_hm_no_cnt_max = 16;
    t_hm1_cfg.et_hm_voice_max_num = 270;
    et_sentence1_init(&t_hm1_cfg);


    t_hm1_cfg.et_db_thd = 40;
    t_hm1_cfg.et_hm_max_thd = 24;
    t_hm1_cfg.et_hm_min_thd = 18;
    t_hm1_cfg.et_hm_no_cnt_max = 12;
    t_hm1_cfg.et_hm_voice_max_num = 1000;
    et_sentence2_init(&t_hm1_cfg);

    int ret = et_asr_wakeup_first_init(&et_kws_cfg, et_func_kws_event);
    printf("init ret:%d\n", ret);
#if 0
    extern int yj_ui_get_mac(char *str, int len);
    char cj_mac_str[24] = {0};
    yj_ui_get_mac(cj_mac_str, sizeof(cj_mac_str));
    printf("cj_mac_str:%s\n", cj_mac_str);
#endif
    printf("-et_bsp_ctrl_main_init(),end-\n");
    //et_asr_file_test();
}

void et_bsp_ctrl_exit()
{
    printf("-et_bsp_ctrl_exit(),start-\n");
    et_stop_asr();
    et_asr_wakeup_exit();


//    et_res_file_close(et_kws_cfg.keyFile);
    printf("-et_bsp_ctrl_exit(),end-\n");
}


void *et_asr_sys_malloc(int size)
{
    return (void *)rt_malloc(size);
}

extern void et_asr_sys_free(void *ptr)
{
    rt_free(ptr);
}

extern void *app_db_get_setting_data(uint16_t key_id);
extern uint8_t rt_flash_config_read(uint8_t id, uint8_t *data, uint8_t size);

#if 0
void et_ui_get_mac_str(char *str, int len)
{
#if 0
    uint8_t *mac_addr = (uint8_t *) app_db_get_setting_data(APP_SETTING_MAC_ADDR);
    snprintf(str, 18, "%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[5], mac_addr[4],
             mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);
#elif 1
//C0:01:AB:60:37:85
    str[0] = 0XC0;
    str[1] = 0X01;
    str[2] = 0XAB;
    str[3] = 0X60;
    str[4] = 0X37;
    str[5] = 0X85;
#else
    uint8_t *mac_addr = (uint8_t *) app_db_get_setting_data(APP_SETTING_MAC_ADDR);
    //memcpy(str, mac_addr, 6 * sizeof(uint8_t) );
    str[0] = mac_addr[5];
    str[1] = mac_addr[4];
    str[2] = mac_addr[3];
    str[3] = mac_addr[2];
    str[4] = mac_addr[1];
    str[5] = mac_addr[0];
#endif
}

uint32_t et_ui_get_license_key_str(char *tmp_str, int len)
{
#if 0
    //char tmp_str[32] = {0};
    uint32_t res;
    res = rt_flash_config_read(FACTORY_CFG_ID_USERK1, (uint8_t *)tmp_str, sizeof(tmp_str));
    return res;
#else
    //9f 64 4b 45 f4 43 17 da a9 53 59 7b 95 ef 84 e2
    tmp_str[0] = 0X9f;
    tmp_str[1] = 0X64;
    tmp_str[2] = 0X4b;
    tmp_str[3] = 0X45;
    tmp_str[4] = 0Xf4;
    tmp_str[5] = 0X43;

    tmp_str[6] = 0X17;
    tmp_str[7] = 0Xda;
    tmp_str[8] = 0Xa9;
    tmp_str[9] = 0X53;
    tmp_str[10] = 0X59;
    tmp_str[11] = 0X7b;

    tmp_str[12] = 0X95;
    tmp_str[13] = 0Xef;
    tmp_str[14] = 0X84;
    tmp_str[15] = 0Xe2;

    return 16;
#endif
}
#else

uint16_t get_ui_APP_SETTING_MAC_ADDR_key_id()
{
    return 0x2345;
}


int yj_ui_get_mac(char *str, int len)
{
    if (len < 18)
    {
        return -1;
    }
    uint8_t *mac_addr = (uint8_t *)"C122334455667788";
    snprintf(str, 18, "%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[5], mac_addr[4],
             mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);
    return 0;
}

int ui_read_licensekey_info(int8_t *buf, int buf_len, int key_type)
{
    char demo_str[] = "9e442e59abcf2e67753fc7f545a9f938";
    memcpy(buf, demo_str, strlen(demo_str));
    return 0;
}

int ui_write_licensekey_info(int8_t *buf, int buf_len, int key_type)
{
    return 0;
}

#endif //0


#define ET_ASR_FILE_TEST_EN  0

#if ET_ASR_FILE_TEST_EN


FILE *et_open_file(char *path, int *length)
{
    FILE *pfile;
    char *data;
    pfile = fopen(path, "rb");
    if (pfile == NULL)
    {
        printf("open %s Fail!\n", path);
        return NULL;
    }
    fseek(pfile, 0, SEEK_END);
    *length = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);
    if (*length == 0)
    {
        fclose(pfile);
        return NULL;
    }
    return pfile;
}

#define ET_BIN_TEST_PCM_PATH   "/preset_user/yj/xiaozhito1.bin"
#define ET_FRAME_BUF_SIZE  160 * 4
int16_t et_frame_data[ET_FRAME_BUF_SIZE];
//char et_frame_data[ET_FRAME_BUF_SIZE * 2];
void et_asr_file_test()
{
#if 0
    printf("--et_asr_file_test(),--start\n");
    int pcm1_len = 0;
    //fd = open(RS_PRESET_PATH"wifi/host_aic8800mc.bin",O_RDONLY);
    FILE *pcm1_fin = et_open_file(ET_BIN_TEST_PCM_PATH, &pcm1_len);
    if (pcm1_fin == NULL)
    {
        printf("Error reading input pcm file: %s\n", ET_BIN_TEST_PCM_PATH);
        printf("--et_asr_file_test(),--end\n");
        return;
    }
    printf("pcm1_len:%d\n", pcm1_len);
    uint32_t pcm_len = pcm1_len;

    const int buffer_len = ET_FRAME_BUF_SIZE;
    const int unit_byte_len = ET_FRAME_BUF_SIZE * sizeof(int16_t);
    //int16_t pcm_data[buffer_len];
    memset(et_frame_data, 0, unit_byte_len);
    printf("start time:%d\n", get_et_asr_time_cnt());

    uint32_t total = 0;
    uint32_t total_time = 0;
    uint32_t start_time = get_et_asr_time_cnt();
    uint32_t end_time = start_time;
#if 0
    for (int i = 0; i < 5; i++)
    {
        memset(et_frame_data, 0, buffer_len * sizeof(int16_t));
        et_asr_buf_write(et_frame_data, buffer_len, 3);
        //total++;
    }
#endif // 0
//    int db2;
    int cost_time;
    while (pcm_len >= unit_byte_len)
    {
        fread(et_frame_data, 2, buffer_len, pcm1_fin);

        //db2 = getPcmDB(et_frame_data, 160);
        //printf("db2:%d\n", db2);
        //total += et_frame_data[0];
        total++;

        pcm_len -= unit_byte_len;

        //printf("start time:%d\n", sys_cb.time_count);
        start_time = get_et_asr_time_cnt();
        et_asr_buf_write(et_frame_data, buffer_len, 3);
        end_time = get_et_asr_time_cnt();
        cost_time = end_time - start_time;
        printf("cost:%d\n", cost_time);
        //printf("end time:%d\n", end_time);
        total_time += cost_time;
    }
    //printf("pcm_len:%d\n", pcm_len);
    // memset(et_frame_data, 0, buffer_len * sizeof(int16_t));


    for (int i = 0; i < 15; i++)
    {
        memset(et_frame_data, 0, buffer_len * sizeof(int16_t));
        et_asr_buf_write(et_frame_data, buffer_len, 3);
        //total++;
    }

    printf("\n");
    end_time = get_et_asr_time_cnt();
    printf("end_time:%d\n", end_time);
    printf("total:%d\n", total);
    printf("total_time:%d\n", total_time);
    //total_time


    fclose(pcm1_fin);
    printf("--et_asr_file_test(),--end\n");
#else
    printf("--et_asr_file_test(),--start\n");
//HAL_RCC_HCPU_EnableDLL1(312000000);
    printf("--et_asr_file_test(),--1111\n");
    const int buffer_len = ET_FRAME_BUF_SIZE;
    const int unit_byte_len = ET_FRAME_BUF_SIZE * sizeof(int16_t);
    //int16_t pcm_data[buffer_len];
    memset(et_frame_data, 0, unit_byte_len);
    printf("start time:%d\n", get_et_asr_time_cnt());

    uint32_t total = 0;
    uint32_t total_time = 0;
    uint32_t start_time = get_et_asr_time_cnt();
    uint32_t end_time = start_time;
    int cost_time;

#if 1
    for (int i = 0; i < 5; i++)
    {
        memset(et_frame_data, 0, buffer_len * sizeof(int16_t));
        //HAL_RCC_HCPU_EnableDLL1(312000000);
        start_time = get_et_asr_time_cnt();
        et_asr_buf_write(et_frame_data, buffer_len, 3);
        end_time = get_et_asr_time_cnt();
        cost_time = end_time - start_time;
        printf("cost:%d\n", cost_time);
        //total++;
    }
#endif // 0

    printf("--et_asr_file_test(),--end\n");
#endif
}
#else
void et_asr_file_test()
{
}
#endif // ET_ASR_FILE_TEST_EN

#endif // ET_WAKEUP_EN
