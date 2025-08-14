#ifndef _LIB_ET_ASR_H__
#define _LIB_ET_ASR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define ET_VAD_ONCE_BUF_SIZE    160 //
#define ET_VAD_ONCE_BUF_NUM    6 //

#ifdef __linux__
#define ET_ASR_NEED_FFT_TYPE_EN    0
#define ET_ASR_NEED_UI_HEAP_EN   0
#define ET_ASR_KWS_LIST_BIN_EN    1 //kws_labels.bin
#define ET_COLLECT_BYTES_BIN_EN    0 //z_et_collects.bin;
#define ET_ASR_UI_PRINT_STATES_EN   1//1 ,Shi Yong//2, Sheng Cheng//0 , Guan Bi//songjian
#else
#define ET_ASR_NEED_FFT_TYPE_EN   0
#define ET_ASR_NEED_UI_HEAP_EN   0
#define ET_ASR_KWS_LIST_BIN_EN    1 //kws_labels.bin
#define ET_COLLECT_BYTES_BIN_EN    0 //z_et_collects.bin;
#define ET_ASR_UI_PRINT_STATES_EN   1//1 ,Shi Yong//2, Sheng Cheng//0 , Guan Bi//songjian
#endif


#define ET_CTC2_BIN_EN    0 //recheck_idx.bin;

typedef void (*et_asr_wakeup_event_cb_t)(int event);
typedef uint32_t (*et_asr_time_cb_t)(void);
#if ET_ASR_NEED_FFT_TYPE_EN
typedef void (*et_fft_user_init_cb_t)(void);
typedef void (*et_fft_forward_cb_t)(int32_t *buf);
typedef void (*et_fft_inverse_cb_t)(int32_t *buf);
#endif // ET_ASR_NEED_FFT_TYPE_EN
typedef void (*et_asr_vad_cb_t)(int16_t *ptr, uint32_t length, uint16_t idx);
typedef void (*et_vad_wordstart_cb_t)(int ret);
typedef void (*et_vad_wordend_cb_t)(int ret, uint16_t voice_cnt);

typedef struct
{
    char *keyFile;
    int keyLen;
#if 1//def __linux__
    char *weightFile;
#else
    u32 weightFile;
#endif // __linux__
    int weightLen;
    char *parmFile;
    int parmLen;
    char *thresListFile;
    int thresListLen;
    char *recheckIdxFile;
    int recheckIdxLen;
    char *c_soft_key;
#if ET_ASR_KWS_LIST_BIN_EN
    char *kwsLabelsFile;
    int kwsLabelsLen;
#endif // ET_ASR_KWS_LIST_BIN_EN

    char *collectFile;
    int collectLen;
#if ET_ASR_NEED_UI_HEAP_EN
    char *et_private_heap;
#endif //ET_ASR_NEED_UI_HEAP_EN
    et_asr_time_cb_t time_cb;
#if ET_ASR_NEED_FFT_TYPE_EN
    et_fft_user_init_cb_t fft_init_cb;
    et_fft_forward_cb_t forward_cb;
    et_fft_inverse_cb_t inverse_cb;
#endif // ET_ASR_NEED_FFT_TYPE_EN
} et_asr_kws_cfg_t;

typedef struct
{
    uint16_t et_vad_en_flag;
    uint16_t et_vad_pre_cnt;
    uint16_t wordStart;
    uint16_t et_db_min_thd;
    uint16_t et_db_max_thd;
    uint16_t et_db_no_cnt_max;
    uint16_t et_vad_word_cnt;
    uint16_t noVoiceCnt;
} et_vad_parm_cfg_t;

typedef struct
{
    uint16_t et_sentence_start;
    uint16_t et_db_thd;
    uint16_t et_hm_max_thd;
    uint16_t et_hm_min_thd;
    uint16_t et_hm_no_cnt_max;
    uint16_t et_hm_voice_max_num;
    uint16_t et_sentence_voiceCnt;
    uint16_t et_H_noVoiceCnt;
} et_hm_parm_cfg_t;


et_hm_parm_cfg_t *get_et_hm1_cfg_p(void);
et_hm_parm_cfg_t *get_et_hm2_cfg_p(void);

uint32_t get_et_asr_time_cnt(void);
#if ET_ASR_NEED_FFT_TYPE_EN
void et_asr_fft_user_init(void);//256samples
void et_asr_fft_forward(int32_t *buf);//FFT
void et_asr_fft_inverse(int32_t *buf);//iFFT
#endif // ET_ASR_NEED_FFT_TYPE_EN
int et_asr_wakeup_first_init(et_asr_kws_cfg_t *cfg, et_asr_wakeup_event_cb_t cb);

int et_asr_wakeup_init(void);
void et_asr_wakeup_exit(void);


int et_asr_buf_write(int16_t *ptr, uint32_t samples);
void et_print_fbank_begin(void);
int get_et_fbank_begin(void);
void et_reset_asr_state(void);
void et_start_asr(void);
void et_stop_asr(void);


uint8_t is_et_asr_work(void);
uint8_t is_et_asr_parm_suc(void);
uint8_t is_et_asr_initing(void);
uint8_t is_et_asr_exitting(void);
uint8_t et_search_kws_list_bin(int row, int col);

et_asr_kws_cfg_t *get_et_asr_kws_cfg_p(void);

void reset_et_vad_states(void);
void et_vad_init(et_vad_parm_cfg_t *vad_cfg);
void et_sentence1_init(et_hm_parm_cfg_t *hm1_cfg);
void et_sentence2_init(et_hm_parm_cfg_t *hm2_cfg);
int et_asr_wakeup_buf_write(int16_t *ptr, int length, uint16_t asr_type,
                            uint8_t voice_pos, uint8_t num,
                            et_asr_vad_cb_t asr_vad_cb, et_vad_wordstart_cb_t wordstart_cb, et_vad_wordend_cb_t wordend_cb);

int16_t et_asr_wakeup_check_once(int16_t *ptr, uint32_t samples, uint16_t asr_type, int voldb, et_hm_parm_cfg_t *hm_cfg);
int16_t et_asr_sentence_check_once(int16_t *ptr, uint32_t samples,
                                   int voldb, et_hm_parm_cfg_t *hm_cfg,
                                   et_asr_vad_cb_t asr_vad_cb, et_vad_wordstart_cb_t wordstart_cb, et_vad_wordend_cb_t wordend_cb);

void set_et_vad_need_hold(uint8_t flag);
void set_et_noise_env(uint8_t flag);
uint8_t get_et_noise_env(void);
uint16_t is_et_vad_wake(void);
uint16_t is_et_sentence_wake(void);
uint16_t is_et_sentence2_wake(void);
uint8_t is_et_vad_en_flag(void);
void set_et_vad_en_flag(uint8_t flag);

int et_asr_data_process(int16_t *ptr, uint32_t samples);
int et_asr_check_one_loop(void);

void set_et_play_tip_mode(uint8_t flag);
uint8_t is_et_play_tip_mode(void);
uint8_t is_et_asr_nosafe(void);

uint8_t is_et_off_asr_activited(void);
int8_t get_et_left_asr_cnt(void);
void et_open_mem_pools_init(void); //
void et_open_mem_pools_destroy(void);
void *et_open_mem_allocate(int size);
void *et_open_mem_zllocate(int size);
void et_open_mem_free(void *ptr);
const char *get_auth_language_name(void);
const char *get_cur_language_name(void);
#if ET_ASR_KWS_LIST_BIN_EN
uint8_t *get_et_kws_labels(int row, uint8_t *labels);
#endif // ET_ASR_KWS_LIST_BIN_EN

void set_et_asr_ignore_cnt(uint16_t cnt);
uint16_t get_et_asr_ignore_cnt(void);
void et_asr_ignore_cnt_dec(void);

#ifdef __cplusplus
}
#endif

#endif
