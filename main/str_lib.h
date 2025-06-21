#include "stdbool.h"
#include <stdio.h>
#include <stdint.h>

extern char split_array[100][20];
extern char replace_array[100];

int  compare_string     (char *str1 , char *str2 , int len);
bool compare_str_offset(char *chr_1 , char *chr_2 , int offset1 , int offset2 , int len);
int  compareUintArray  (uint8_t *uint1 , uint8_t  *uint2 , int len);
void str_split         (char *str , char *split_str);
int  get_string_length (char *data);
