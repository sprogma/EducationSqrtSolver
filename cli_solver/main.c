#include "sqsv/sqsv.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"


#define MAX_POWER 2


int main(int argc, char **argv)
{
    double *coefficients = malloc(sizeof(*coefficients) * (MAX_POWER + 1));
    
    /* parse cmdline args */
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "--"))
        {
            break;
        }
        else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            puts(
                #include ".help.inc"
            );
            return 0;
        }
        else if (!strcmp(argv[i], "--coeff") || !strcmp(argv[i], "-c"))
        {
            
        }
        else
        {
            fprintf(stderr, "Unknown comand line flag: %s\n", argv[i]);
        }
    }


    
    

    return 0;
}
