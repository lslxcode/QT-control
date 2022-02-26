#ifndef JOYSTICK_H 
#define JOYSTICK_H   
#include <linux/joystick.h>

typedef struct _axes_t {
    int x;
    int y;
} AXES_T;

#ifdef __cplusplus
extern "C" {
#endif


int fd_Init();
//void get_point(char **result);
void getBtnStatus(char *result);
int **get_point(void);
void closefd(void);


#ifdef __cplusplus
}
#endif
 
#endif
