#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "pbmio.h"
#include "pnmrdr.h"

Bit2_T pbmread(FILE * fp)
{
	Pnmrdr_T r = Pnmrdr_new(fp);
	Pnmrdr_mapdata data = Pnmrdr_data(r);
	if (data.type != Pnmrdr_bit) {
		fprintf(stderr, "Read %s where bitmap was expected\n",
			Pnmrdr_maptype_names[data.type]);
		exit(1);
	}
	assert(data.type == Pnmrdr_bit);
	Bit2_T bitmap = Bit2_new(data.width, data.height);
	for (unsigned j = 0; j < data.height; j++)
		for (unsigned i = 0; i < data.width; i++)
			Bit2_put(bitmap, i, j, Pnmrdr_get(r));
	Pnmrdr_free(&r);
	return bitmap;
}

void pbmwrite(FILE * fp, Bit2_T bitmap)
{
	int column = 0;	    /* number of characters written to current row */
	int w = Bit2_width(bitmap);
	int h = Bit2_height(bitmap);

	fprintf(fp, "P1\n# written by pbmwrite in pbmio.c\n%d %d\n", w, h);
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			putc(Bit2_get(bitmap, i, j) ? '1' : '0', fp);

			/* if [[column]] is too big, 
                         * write a newline and reset it
                         */
			if (column++ >= 69) {
				putc('\n', fp);
				column = 0;
			}
		}
		/* at end of each row, write a newline and reset column */
		putc('\n', fp);
		column = 0;
	}
}
