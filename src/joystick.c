/*joystick.c*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "listop.h"
#include "joystick.h"
//#include "Recv.h"
char number_of_axes = 0;

char number_of_btns = 0;
AXES_T* tp_axes = NULL;

unsigned int buttons_state = 0;
unsigned int  print_init_stat = 0;
struct js_event jse;
int fd;
char steer[11]={1,1,1,1,1,1,1,1,1,1,1};
int speed_result[4][2];


pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
#define  DATA_LOCK		do{pthread_mutex_lock(&data_mutex);} while (0)
#define  DATA_UNLOCK	do{pthread_mutex_unlock(&data_mutex);} while (0)


#if 1
#define LOG_DBG(fmt, ...)  fprintf(stdout, fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)
#endif

#define LOG_ERR(fmt, ...)  fprintf(stderr, fmt, ##__VA_ARGS__)


typedef struct _joy_stick_ctx {
    struct list_head list;
    int i4_js_fd;
    unsigned int i4_op_block;
} JOYSTICK_CTX_T;

LIST_HEAD(_t_js_ctx_head);
/*==>  struct list_head _t_js_ctx_head = {&_t_js_ctx_head, &_t_js_ctx_head};*/

int joystick_open(char* cp_js_dev_name, int i4_block)
{
    int i4_open_flags = O_RDONLY;
    JOYSTICK_CTX_T*  pt_joystick_ctx = NULL;

    if (!cp_js_dev_name) {
        LOG_ERR("[%s] js device name is NULL\n", __func__);
        return -1;
    }

    pt_joystick_ctx = (JOYSTICK_CTX_T*)calloc(sizeof(JOYSTICK_CTX_T), 1);
    if (!pt_joystick_ctx) {
        LOG_ERR("[%s] no memory!!\n", __func__);
        return -1;
    }

    pt_joystick_ctx->i4_op_block = i4_block ? 1 : 0;

    if (pt_joystick_ctx->i4_op_block == 0) {
        i4_open_flags |= O_NONBLOCK;
    }

    pt_joystick_ctx->i4_js_fd = open(cp_js_dev_name, i4_open_flags);
    if (pt_joystick_ctx->i4_js_fd < 0) {
        LOG_ERR("[%s] open device %s error\n", __func__, cp_js_dev_name);
        free(pt_joystick_ctx);
        return -1;
    }
    list_add_tail(&pt_joystick_ctx->list, &_t_js_ctx_head);

    return pt_joystick_ctx->i4_js_fd;
}

int joystick_close(int i4_fd)
{
    struct list_head* pt_entry;
    struct list_head* pt_next;
    JOYSTICK_CTX_T* pt_node;

    if (list_empty(&_t_js_ctx_head)) {
        LOG_ERR("[%s] device not opened\n", __func__);
        return -1;
    }

    list_for_each_safe(pt_entry, pt_next, &_t_js_ctx_head) {
        pt_node = list_entry(pt_entry, JOYSTICK_CTX_T, list);
        if (pt_node->i4_js_fd == i4_fd) {
            list_del_init(&pt_node->list);
            free(pt_node);
            return close(i4_fd);
        }
    }

    LOG_ERR("[%s] i4_fd=%d invalid\n", __func__, i4_fd);
    return -1;
}

int joystick_read_one_event(int i4_fd, struct js_event* tp_jse)
{
    int i4_rd_bytes;

    /*do not check i4_fd again*/

    i4_rd_bytes = read(i4_fd, tp_jse, sizeof(struct js_event));

    if (i4_rd_bytes == -1) {
        if (errno == EAGAIN) { /*when no block, it is not error*/
            return 0;
        }
        else {
            return -1;
        }
    }

    return i4_rd_bytes;
}

int joystick_read_ready(int i4_fd)
{
    int i4_block = 2;
    struct list_head* pt_entry;
    JOYSTICK_CTX_T* pt_node;

    if (list_empty(&_t_js_ctx_head)) {
        LOG_ERR("[%s] device not opened\n", __func__);
        return -1;
    }

    list_for_each(pt_entry, &_t_js_ctx_head) {
        pt_node = list_entry(pt_entry, JOYSTICK_CTX_T, list);
        if (pt_node->i4_js_fd == i4_fd) {
            i4_block = pt_node->i4_op_block;
            break;
        }
    }

    if (i4_block == 2) {
        LOG_ERR("[%s] i4_fd=%d invalid\n", __func__, i4_fd);
        return 0;
    }
    else if (i4_block == 1) {
        fd_set readfd;
        int i4_ret = 0;
        struct timeval timeout = {0, 0};
        FD_ZERO(&readfd);
        FD_SET(i4_fd, &readfd);

        i4_ret = select(i4_fd + 1, &readfd, NULL, NULL, &timeout);

        if (i4_ret > 0 && FD_ISSET(i4_fd, &readfd)) {
            return 1;
        }
        else {
            return 0;
        }

    }

    return 1; /*noblock read, aways ready*/
}


void debug_list(void)
{
    char s_char[128];
    int i = 0;


    if (! list_empty(&_t_js_ctx_head)) {
        struct list_head* pt_entry;
        JOYSTICK_CTX_T* pt_node;

        list_for_each(pt_entry, &_t_js_ctx_head) {
            pt_node = list_entry(pt_entry, JOYSTICK_CTX_T, list);
            LOG_DBG("fd:%d--block:%d\n", pt_node->i4_js_fd, pt_node->i4_op_block);
        }
    }
    else {
        LOG_DBG("-----------> EMPTY NOW\n");
    }
}

void* pthread_joystick(void* args)
{
    int rc;
    char but_sta = 0;
    char status[11];
    while(1)
    {
        if (joystick_read_ready(fd))
        {
            rc = joystick_read_one_event(fd, &jse);
            if (rc > 0)
            {
                if (jse.type  == JS_EVENT_BUTTON) //按键状态改变时输出相关信息
                {
                    if (jse.value)
                        buttons_state |= (1 << jse.number);
                    else
                        buttons_state &= ~(1 << jse.number);//LOG_DBG("joystick state: button %d is %s.\n", jse.number, ((buttons_state & (1 << jse.number)) == (1 << jse.number)) ? "DOWN" : "UP");
                    if(jse.number < 12)
                    {

                        but_sta = ((buttons_state & (1 << jse.number)) == (1 << jse.number)) ? 0 : 1;
                        status[jse.number] = but_sta;
                        DATA_LOCK;
                        if((but_sta == 0) && (status[jse.number]!=steer[jse.number]))
                        {
                            steer[jse.number] = 0;
                            status[jse.number] = 0;
                        }
                        DATA_UNLOCK;

                    }

                }
                else if (jse.type == JS_EVENT_AXIS) //手杆状态改变时输出相关信息
                {
                    if (tp_axes)
                    {
                        if ((jse.number & 1) == 0)
                            tp_axes[jse.number / 2].x = jse.value;
                        else
                            tp_axes[jse.number / 2].y = jse.value;
                        if((jse.number / 2) == 0)  //yaogai0
                        {
                            DATA_LOCK;
                            speed_result[0][0] = tp_axes[jse.number / 2].x;
                            speed_result[0][1] = tp_axes[jse.number / 2].y;
                            DATA_UNLOCK;
                        }

                        //tp_axes[jse.number / 2] jse.number/2为手杆序号  .x .y分别对应手杆当前x轴和y轴的值
                    }
                }

            }
        }
    }
}

int fd_Init()
{
    int rc;

    char js_name_str[128];
    fd = joystick_open("/dev/input/js0", 1);
    //printf("cs\n");
    if (fd < 0)
    {
        LOG_ERR("open failed.\n");
        return -1;
    }
    rc = ioctl(fd, JSIOCGAXES, &number_of_axes);
    if (rc != -1)
    {
        LOG_DBG("number_of_axes:%d\n", number_of_axes);
        if (number_of_axes > 0)
            tp_axes = (AXES_T*)calloc(sizeof(AXES_T), 1);
    }

    rc = ioctl(fd, JSIOCGBUTTONS, &number_of_btns);
    if (rc != -1)
    {
        LOG_DBG("number_of_btns:%d\n", number_of_btns);
    }

    if (ioctl(fd, JSIOCGNAME(sizeof(js_name_str)), js_name_str) < 0)
    {
        LOG_DBG(js_name_str, "Unknown", sizeof(js_name_str));
    }
    LOG_DBG("joystick Name: %s\n", js_name_str);
    pthread_t tids;

    //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    pthread_create(&tids, NULL, pthread_joystick, NULL);


    return fd;
}

void getBtnStatus(char *result)
{
    for(int i = 0;i<12;i++)
    {
        result[i] = 1;
        DATA_LOCK;
        if(steer[i] == 0)
        {
            result[i] =0;
            steer[i] = 1;
        }
        DATA_UNLOCK;
    }
}

int **get_point(void)
{
    int **a,i,j;
    a = (int**)malloc(4*sizeof(int*));
    for(i=0;i<2;++i)
    {
        a[i] = (int*)malloc(2*sizeof(int));
    }
    //return a;//error: cannot convert `int (*)[5]' to `int**' in return
    //int rc;
    DATA_LOCK;
    a[0][0] = (speed_result[0][0])+32767;
    a[0][1] = (speed_result[0][1])+32767;
    DATA_UNLOCK;
    return a;

}
void closefd(void)
{
  //
    //close(fd);
    LOG_DBG("fd close!\n");
}
