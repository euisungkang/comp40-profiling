#include <stdio.h>
#include "bit2.h"

/* read and write bitmaps in pbm(5) format */

extern Bit2_T pbmread (FILE *infile);
extern void   pbmwrite(FILE *outfile, Bit2_T bitmap);

