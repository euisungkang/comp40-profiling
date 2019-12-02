#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "unblackedges.h"
#include "pbmio.h"

static FILE *openread(const char *path)
{
        if (path == NULL)
                return stdin;
        else {
                FILE *input = fopen(path, "rb");
                if (!input) {
                        perror(path);
                        exit(1);
                }
                return input;
        }
}

int main(int argc, char *argv[])
{
        assert(argc <= 2);
        FILE *input = openread(argc == 2 ? argv[1] : NULL);
        Bit2_T bitmap = pbmread(input);
        if (input != stdin)
                fclose(input);
        remove_black_edges(bitmap);
        pbmwrite(stdout, bitmap);
        Bit2_free(&bitmap);
        return EXIT_SUCCESS;
}
