#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "pnmrdr.h"
#include "assert.h"
#include "unblackedges.h"
#include "seq.h"
#include "pbmio.h"

static void clear_blackedge_bit(int i, int j, Bit2_T bitmap, int bit, void *cl)
{
	Bit2_T blackedge = cl;
	(void)bit;	    /* avoid compiler warning for unused argument */
	if (Bit2_get(blackedge, i, j))
		Bit2_put(bitmap, i, j, 0);
}

static void clear_blackedge_bits(Bit2_T bitmap, Bit2_T blackedge)
{
	Bit2_map_row_major(bitmap, clear_blackedge_bit, blackedge);
}
static inline void maybe_enqueue(Bit2_T bitmap, int i, int j, Seq_T iq,
				 Seq_T jq)
{
	if (Bit2_get(bitmap, i, j)) {
		Seq_addhi(iq, (void *)(uintptr_t) i);
		Seq_addhi(jq, (void *)(uintptr_t) j);
	}
}
static void set_blackedge_bits(Bit2_T bitmap, Bit2_T blackedge)
{
	int h = Bit2_height(bitmap);
	int w = Bit2_width(bitmap);

	Bit2_T visited = Bit2_new(w, h);  /* for classic depth-first search */
	int nvisited = 0;	          /* number of pixels in visited map */
	Seq_T iq = Seq_new(2 * w + 2 * h);
	Seq_T jq = Seq_new(2 * w + 2 * h);

	for (int row = 0; row < h; row++) {
		if (Bit2_get(bitmap, 0, row)) {
			Bit2_put(blackedge, 0, row, 1);
			maybe_enqueue(bitmap, 1, row, iq, jq);
		}
		Bit2_put(visited, 0, row, 1);
		if (Bit2_get(bitmap, w - 1, row)) {
			Bit2_put(blackedge, w - 1, row, 1);
			maybe_enqueue(bitmap, w - 2, row, iq, jq);
		}
		Bit2_put(visited, w - 1, row, 1);
	}
	nvisited += 2 * h;;
	for (int col = 0; col < w; col++) {
		if (Bit2_get(bitmap, col, 0)) {
			Bit2_put(blackedge, col, 0, 1);
			maybe_enqueue(bitmap, col, 1, iq, jq);
		}
		Bit2_put(visited, col, 0, 1);
		if (Bit2_get(bitmap, col, h - 1)) {
			Bit2_put(blackedge, col, h - 1, 1);
			maybe_enqueue(bitmap, col, h - 2, iq, jq);
		}
		Bit2_put(visited, col, h - 1, 1);
	}
	nvisited += 2 * (w - 2);	/* do not double-count corner pixels */

	int maxlen = Seq_length(iq);	/* maximum length of queue           */
	int blackened = 0;	        /* number of black edge pixels found */
	int tried = 2 * (w + h - 1);	/* number of touches of pixels       */
	while (Seq_length(iq) > 0) {
		tried++;
		if (Seq_length(iq) > maxlen)
			maxlen = Seq_length(iq);
		int i = (int)(uintptr_t) Seq_remlo(iq);
		int j = (int)(uintptr_t) Seq_remlo(jq);
		if (!Bit2_get(visited, i, j)) {
			Bit2_put(visited, i, j, 1);
			nvisited++;
			if (Bit2_get(bitmap, i, j)) {
				blackened++;
				Bit2_put(blackedge, i, j, 1);
				maybe_enqueue(bitmap, i + 1, j, iq, jq);
				maybe_enqueue(bitmap, i - 1, j, iq, jq);
				maybe_enqueue(bitmap, i, j + 1, iq, jq);
				maybe_enqueue(bitmap, i, j - 1, iq, jq);
			}
		}
	}
	assert(Seq_length(jq) == 0);;
	Bit2_free(&visited);
	Seq_free(&iq);	
	Seq_free(&jq);
	if (getenv("UNBLACKVERBOSE"))
		fprintf(stderr, "Tried %d%% (%.1f%% distinct) and unblacked "
			"%.1f%% of %dx%d == %d pixels; max q = %d\n",
			(100 * tried + (w * h / 2)) / (w * h),
			(100.0 * (double)nvisited) / (double)(w * h),
			(100.0 * (double)blackened) / (double)(w * h),
			w, h, w * h, maxlen);
}
static void clear_pixel(int i, int j, Bit2_T bitmap, int bit, void *cl)
{
	(void)bit;
	(void)cl;		/* stop compiler bleats */
	Bit2_put(bitmap, i, j, 0);
}

void remove_black_edges(Bit2_T bitmap)
{
	int h = Bit2_height(bitmap);
	int w = Bit2_width(bitmap);
	if (w == 1 || h == 1)
		Bit2_map_row_major(bitmap, clear_pixel, NULL);
	else {
		assert(w >= 2 && h >= 2);
		Bit2_T blackedge = Bit2_new(w, h);
		set_blackedge_bits(bitmap, blackedge);
		clear_blackedge_bits(bitmap, blackedge);
		Bit2_free(&blackedge);
	}
}
