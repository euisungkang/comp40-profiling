#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED
#define T Bit2_T
typedef struct T *T;
extern T    Bit2_new (int width, int height);
extern void Bit2_free(T *bit2);
extern int  Bit2_width(T bit2);
extern int  Bit2_height(T bit2);
extern int  Bit2_get(T bit2, int i, int j);
extern int  Bit2_put(T bit2, int i, int j, int bit);
extern void Bit2_map_row_major(T bit2, 
                               void apply(int i, int j, T bitmap, 
                                          int bit, void *cl), 
                               void *cl);
extern void Bit2_map_col_major(T bit2, 
                               void apply(int i, int j, T bitmap, 
                                          int bit, void *cl), 
                               void *cl);
#undef T
#endif
