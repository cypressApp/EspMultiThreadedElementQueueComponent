#include <str_lib.h>

char split_array[100][20];
char replace_array[100];
int  split_size = 0;
int  str_len       =  0;
int  split_str_len =  0;

int compare_string(char *str1 , char *str2 , int len){

	for(int i = 0 ; i < len ; i++){
	
		if(str1[i] != str2[i]){
			return 1;
		}
	
	}
	
	return 0;

}

bool compare_str_offset(char *chr_1 , char *chr_2 , int offset1 , int offset2 , int len){

    chr_1 += offset1;
    chr_2 += offset2;
  
    for(int i = 0 ; i < len ; i++){

      if(*chr_1 != *chr_2){
        
        return false;
        
      }
      
      chr_1 ++;
      chr_2 ++;    
      
    }
    
    return true;
  
}

int compareUintArray(uint8_t *uint1 , uint8_t  *uint2 , int len){
		
	for(int i = 0 ; i < len ; i++){
		
		if(*uint1 != *uint2){
			return 1;
		}		
		
		uint1++;
		uint2++;
	
	}
	
	return 0;

}

void str_split(char *str , char *split_str){
  
  str_len       = get_string_length(str);
  split_str_len = get_string_length(split_str);

  for(int i = 0 ; i < 100 ; i++){
    for(int j = 0 ; j < 20 ; j++){
      split_array[i][j] = 0;
    }
  }
  
  split_size   = 0;
  int char_counter = 0;
  int i = 0;
  
  while(i < str_len){
    
    if(compare_str_offset(str , split_str , i , 0 , split_str_len)){
      split_size++;
      char_counter = 0;
      i += split_str_len;
    }else{
      split_array[split_size][char_counter] = str[i];
      char_counter++;
      i++;
    } 
    
  } 
  
}

int get_string_length(char *data){

	// for(int i = 0 ; i < 128 ; i++){
	// 	if(data[i] == '\0' || data[i] == 0){
	// 		return i;
	// 	}
	// }
	// return 0;

  int str_size = 0;
  
  while(*data != 0 || *data != '\0'){
    data      ++;
    str_size ++;
  }
  
  return str_size;

}
