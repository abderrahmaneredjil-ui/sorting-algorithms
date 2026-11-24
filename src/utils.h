#ifndef UTILS_H
#define UTILS_H

typedef enum { INPUT_RANDOM = 1, INPUT_SORTED, INPUT_REVERSE, INPUT_DUPLICATES } InputType;

#define SEED 42u   

const char *input_name(InputType t);
void fill_array(int a[], int n, InputType t, unsigned seed);
int  is_sorted(const int a[], int n);
void print_preview(const int a[], int n);          
void print_array(const int a[], int n);            
int  read_int(const char *prompt, int min, int max); 

#endif
