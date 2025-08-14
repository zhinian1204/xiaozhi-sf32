#include "et_decoder_patch.h"
#include <lib_et_asr.h>
#include <string.h>
#include <stdio.h>

extern uint8_t et_units_item2pos(const char *key);
extern uint8_t *get_curr_process_word_path(int index);
extern int get_et_each_keyword_len_max(void);
extern int get_et_chuncksize(void);

#define USE_TOPK 1
#if USE_TOPK    //old decode without topk
#define TOPK1 10
int get_et_topk1(void)
{
    return TOPK1;
}
extern int get_et_ctc_cls(void);

#else
extern int get_et_topk1(void);
#endif

#define CNT_RESET 30000
#define EACH_KEYWORD_LEN_MAX 16
#define MUL_PATH_LEN_MAX 48
#define KWS_RAW_FLAG 232
#define SEQ_LEN_POS (MUL_PATH_LEN_MAX*2 + 2)

int is_et_ui_filter_keyword(int index, uint8_t  prefix_flag)
{
    return 0;
}

int is_et_ui_delay_keyword(int index, uint8_t prefix_flag)
{
#if 1
    if (prefix_flag == 254)
    {
        return 1;
    }
#endif
    return 0;
}

int find_phoneme_index(const char *phoneme)
{
    return et_units_item2pos(phoneme);
}

uint8_t get_score_row(uint8_t *ctcrow, int ph_idx, int topk)
{
    uint8_t iscore = 1;
    for (int p = 0; p < topk; p++)
    {
        if (ph_idx == ctcrow[2 * p + 1])
        {
            iscore = ctcrow[2 * p];
            break;
        }
    }
    return iscore;
}

int isnotinseq(int ph_idx, uint8_t *alipath, int T)
{
    for (int i = 0; i < T; i++)
    {
        if (ph_idx == alipath[i])
        {
            return 0;
        }
    }
    return 1;
}

// save topk ctc score &index
void update_ctc_topk_patch(uint8_t score, uint8_t index, int topk, uint8_t *ctc_out)
{

    uint8_t k = topk - 1;
    if (score <= ctc_out[2 * k])
    {
        return;
    }
    ctc_out[2 * k] = score;
    ctc_out[2 * k + 1] = index;

    int j = k - 1;
    while (j >= 0 && ctc_out[2 * j] < ctc_out[2 * j + 2])
    {
        //[分值 ,index ]
        for (int i = 0; i < 2; i++)
        {
            uint8_t tmp = ctc_out[2 * (j + 1) + i];
            ctc_out[2 * (j + 1) + i] =  ctc_out[2 * j + i];
            ctc_out[2 * j + i] =  tmp;
        }
        j--;
    }
}


void convert_topk(uint8_t *row_data, uint8_t *ctc_out, int topk, int cls_num)
{

    //case2 topk softmax
    memset(ctc_out, 0, 2 * topk);
    int topk_ph = topk - 1;
    for (int i = 0; i < cls_num ; i++)
    {
        if (i == cls_num - 1)
        {
            ctc_out[2 * topk_ph] = row_data[i];
            ctc_out[2 * topk_ph + 1] = i;
        }
        else
        {
            update_ctc_topk_patch(row_data[i], i, topk, ctc_out);
        }
    }
}

int part_ph_ali_check(int8_t recheck_left, int8_t recheck_right, uint8_t *alipath, uint8_t *alipos, uint8_t *label_seq, uint8_t *ph_boundary, int cut_left, int cut_right)
{

    //logic
    int iph_idx = 0;
    uint8_t ph_pos = 0;
    int icnt = 0;
    bool skip_flag = false;
    int key_ph_score = 0;
    bool iflag = true;
    int next_idx = -1;
    int t_topk1 = get_et_topk1();
    int word_len_max_d2 = 2 * get_et_each_keyword_len_max();
    uint8_t *ctc_row = get_curr_process_word_path(get_et_chuncksize());

    for (int i = recheck_left; i <= recheck_right; i++)
    {
        if (i - recheck_left >= cut_right)
        {
            continue;
        }
        if (i - recheck_left <= cut_left)
        {
            continue;
        }
        ph_pos = alipath[i - recheck_left];
        iph_idx = alipos[i - recheck_left];
        next_idx = -1;
        if (i < recheck_right)
        {
            next_idx = alipos[i + 1 - recheck_left];
        }

#if USE_TOPK
        {
            convert_topk(get_curr_process_word_path(i), ctc_row, t_topk1, get_et_ctc_cls());
        }
#else
        {
            uint8_t *ctc_row = get_curr_process_word_path(i);
        }
#endif

        if (ph_pos == 65 || iph_idx == word_len_max_d2)
        {
            //The score for the blank position is too low,  phonemes are being force aligned to the blank.
            if (ctc_row[2 * (t_topk1 - 1)] < 50) //|| ctc_row[0] > 50
            {
                icnt += 1;
            }
            continue;
        }
        key_ph_score = 0;
        for (int k = ph_boundary[iph_idx]; k < ph_boundary[iph_idx + 1]; k++)
        {
            key_ph_score += get_score_row(ctc_row, label_seq[k], t_topk1); //ctc1
        }
        iflag = true;
        for (int j = 0; j < t_topk1; j++)
        {
            skip_flag = false;
            for (int k = ph_boundary[iph_idx]; k < ph_boundary[iph_idx + 1]; k++)
            {
                if (ctc_row[2 * j + 1] == label_seq[k])
                {
                    skip_flag = true;
                }
            }
            if (next_idx != -1 && next_idx != word_len_max_d2)
            {
                for (int k = ph_boundary[next_idx]; k < ph_boundary[next_idx + 1]; k++)
                {
                    if (ctc_row[2 * j + 1] == label_seq[k])
                    {
                        skip_flag = true;
                    }
                }
            }


            if (skip_flag)
            {
                continue;
            }
            if (iflag && ctc_row[2 * j] > 50 && key_ph_score < 20) //condition 1
            {
                icnt += 1;
                iflag = false;
                //printf("[1]%d,%d,\n",ctc_row[2*j +1],ctc_row[2 *j]);
            }
        }
    }

    //printf("abnormal ph info :(%d)\n",  icnt);
    if (icnt >= 1) //condition 2
    {
        return 0;//
    }
    return 1;
}

int8_t et_ui_decoder_recheck_result(char *audio_phone_str, int strLen, int16_t left, int16_t right, int chunk_size, int16_t *aipos, int *kwsIndex, uint8_t *alipath, uint8_t *alipos, uint8_t *label_seq, uint8_t *ph_boundary)
{
#if 0
    int8_t time_len = right - left;
    if (right > chunk_size - 1)
    {
        right = chunk_size - 1;
        left = right - time_len;
    }
    int8_t start = left;
    int8_t end = right;
    int8_t result = 1;

    uint8_t flag = label_seq[strLen - 1];
    if (KWS_RAW_FLAG == flag)
    {
        result =  recheck_kws_raw(left, right, aipos, alipath, alipos, label_seq, ph_boundary);
        return result;
    }

    uint8_t inoise = label_seq[strLen - 1];
    // return 1;

    if (strcmp(audio_phone_str, "d ing sh ix ee er sh ix f en zh ong ") == 0 ||
            strcmp(audio_phone_str, "d ing sh ix s iy sh ix f en zh ong ") == 0)
    {
        result = recheck_dingshixfenzhong(start, end, alipath, alipos, label_seq, ph_boundary);
    }
    if (strcmp(audio_phone_str, "ee er sh ix f en zh ong g uan d eng ") == 0 ||
            strcmp(audio_phone_str, "s iy sh ix f en zh ong g uan d eng ") == 0)
    {
        result = recheck_xfenzhongguandeng(start, end, alipath, alipos, label_seq, ph_boundary);
    }

    if (strcmp(audio_phone_str, "g uan d eng ") == 0)
    {
        result = recheck_guandeng(start, end, alipath, alipos, label_seq, ph_boundary);
    }

    if (strcmp(audio_phone_str, "x ia f an ii ie ") == 0)
    {
        result = recheck_xiafanye(start, end, alipath, alipos, label_seq, ph_boundary);
        if (result == 0)
        {
            *kwsIndex = 63; //shang fan ye
            result = 1;
        }
    }

    return result;

#else
    return 1;
#endif
}
