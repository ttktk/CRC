#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#define OCTET           256
#define ECTET_MAX       255
#define FRAME_SIZE      1024
#define FCS_SIZE        4
#define TOTAL_SIZE      1028
#define ERROR_STEP      200
#define ERROR_MAX       4100
#define TEST_TIMES      1000


void frame_gen(char* frame);
void error_gen(char* frame, int err_cnt);
void crcgen(void);
void err_pos_gen(int err_cnt);
int get_fcs(char* frame, int frame_size);


unsigned long   crcTable[256];
int             error_pos[TOTAL_SIZE * 8];


int main(int argc, char* argv[])
{
    int             err_cnt, i, count;
    unsigned long   crc;
    time_t          seed;
    char            in_frame[TOTAL_SIZE], out_frame[TOTAL_SIZE];

    time(&seed);
    srand(seed);
    crcgen();       


    printf("  @@@@ CRC 에러 검출 시뮬레이션 @@@@ \n\n");
    printf("에러수\t\t전송수\t\t검출수\n");
    for (err_cnt = ERROR_STEP; err_cnt < ERROR_MAX; err_cnt += ERROR_STEP)   
    {
        count = 0;
        err_pos_gen(err_cnt);
        
        for (i = 0; i < TEST_TIMES; i++)
        {
            frame_gen(in_frame);
            crc = get_fcs(in_frame, FRAME_SIZE);
            memcpy(in_frame + FRAME_SIZE, &crc, sizeof(crc));
            memcpy(out_frame, in_frame, TOTAL_SIZE);
            error_gen(out_frame, err_cnt);
            crc = get_fcs(out_frame, TOTAL_SIZE);
            if (crc != 0) count++;
        }
        printf("%5d\t\t%5d\t\t%5d\n", err_cnt, i, count);
    }

    return(0);
}


void frame_gen(char* frame)
{
    int     i;

    for (i = 0; i < FRAME_SIZE; i++)
        frame[i] = rand() % 255;         
}



void error_gen(char* frame, int err_cnt)
{
    int     i = 0, l, k;

    for (i = 0; i < err_cnt; i++)
    {
        l = error_pos[i] / 8;
        k = error_pos[i] % 8;
        frame[l] = frame[l] ^ (1 << k);
    }     
}


void err_pos_gen(err_cnt)
{
    int i;
    int j = 0;
    int k;

    for (i = 0; i < err_cnt; i++)
    {
        while (j != i)
        {
            k = rand() % (TOTAL_SIZE * 8);
            for (j = 0; j < i; j++)
                if (error_pos[j] == k) break;
            error_pos[i] = k;

        }
        
    }
}


void crcgen(void)
{
    unsigned long   crc, poly;
    int     i, j;

    poly = 0xEDB88320L; 
    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 8; j > 0; j--) {
            if (crc & 1) {
                crc = (crc >> 1) ^ poly; 
            }
            else {
                crc >>= 1;
            }
        }
        crcTable[i] = crc;
    } 
}


int get_fcs(char* frame, int frame_size)
{
    register unsigned long          crc;
    int                             b, i;

    crc = 0xFFFFFFFF;

   
    for (i = 0; i < frame_size; i++)
        crc = ((crc >> 8) & 0x00FFFFFF) ^ crcTable[(crc ^ frame[i]) & 0xFF];
    return (crc ^ 0xFFFFFFFF);
}