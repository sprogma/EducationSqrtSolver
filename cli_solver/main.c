#include "sqsv/sqsv.h"

#include "inttypes.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"


#define MAX_POWER ((size_t)2)
#define MAX_COEFF_LENGTH ((MAX_POWER) + 1)

#define CMD_ARG_FLAG_SILENT               0x00002
#define CMD_ARG_FLAG_REVERSE_COEFFICIENTS 0x00004

/* conditional printf */
#define cprintf(...) do {if (!(cmd_arg_flags & CMD_ARG_FLAG_SILENT)) { printf(__VA_ARGS__); }} while (0)
#define cfprintf(...) do {if (!(cmd_arg_flags & CMD_ARG_FLAG_SILENT)) { fprintf(__VA_ARGS__); }} while (0)

uint64_t read_coefficients(uint64_t flags, double **coefficients, size_t *coefficients_length);

int main(int argc, char **argv)
{
    double *coefficients = NULL;
    size_t coefficients_length = 0;
    uint32_t coefficients_set = 0;
    uint64_t cmd_arg_flags = 0;
    
    /* parse cmdline args */
    for (int curr = 1; curr < argc; ++curr)
    {
        if (!strcmp(argv[curr], "--"))
        {
            break;
        }
        else if (!strcmp(argv[curr], "--reverse") || !strcmp(argv[curr], "-r"))
        {
            cmd_arg_flags |= CMD_ARG_FLAG_REVERSE_COEFFICIENTS;
        }
        else if (!strcmp(argv[curr], "--help") || !strcmp(argv[curr], "-h"))
        {
            if (!(cmd_arg_flags & CMD_ARG_FLAG_SILENT))
            {
                puts(
                    #include ".help.inc"
                );
            }
            free(coefficients);
            return 0;
        }
        else if (!strcmp(argv[curr], "--silent") || !strcmp(argv[curr], "-s"))
        {
            cmd_arg_flags |= CMD_ARG_FLAG_SILENT;
        }
        else if (!strcmp(argv[curr], "--coeff") || !strcmp(argv[curr], "-c"))
        {
            if (coefficients_set)
            {
                fprintf(stderr, "Given two \"coeff\" arguments at the same time.");
            }
            /* read coefficients */
            size_t coefficients_alloc = 16;
            coefficients = malloc(sizeof(*coefficients) * coefficients_alloc);
            if (coefficients == NULL)
            {
                fprintf(stderr, "No more memory\n");
                return 1;
            }
            curr++;
            double value = 0.0;
            size_t id = 0;
            while (curr < argc)
            {
                char *end_ptr;
                value = strtod(argv[curr], &end_ptr);
                if (*end_ptr != 0)
                {
                    break;
                }
                if (id >= MAX_COEFF_LENGTH)
                {
                    cfprintf(stderr, "Warning: too many coefficients given. skip them.\n");
                    break;
                }
                if (id >= coefficients_alloc)
                {
                    coefficients_alloc *= 2;
                    double *new_ptr = realloc(coefficients, sizeof(*coefficients) * coefficients_alloc);
                    if (new_ptr == NULL)
                    {
                        fprintf(stderr, "No more memory\n");
                        
                        free(coefficients);
                        return 1;
                    }
                    coefficients = new_ptr;
                }
                coefficients[id] = value;
                id++;
                curr++;
            }
            coefficients_length = id;
            cfprintf(stderr, "Read %"PRIuMAX" command line coefficients: ", (uintmax_t)coefficients_length);
            for (size_t i = 0; i < coefficients_length; ++i)
            {
                cfprintf(stderr, "%g ", coefficients[i]);
            }
            cfprintf(stderr, "\n");
            coefficients_set = -1;
            /* set pointer on last read argument */
            --curr;
        }
        else
        {
            cfprintf(stderr, "Unknown command line flag: %s\n", argv[curr]);
        }
    }

    if (!coefficients_set)
    {
        uint64_t error_code = read_coefficients(cmd_arg_flags, &coefficients, &coefficients_length);
        if (error_code != 0)
        {
            free(coefficients);
            return 0x1000 | error_code;
        }
    }


    if (cmd_arg_flags & CMD_ARG_FLAG_REVERSE_COEFFICIENTS)
    {
        double tmp;
        for (size_t i = 0; i + i < coefficients_length; ++i)
        {
            tmp = coefficients[i];
            coefficients[i] = coefficients[coefficients_length - i - 1];
            coefficients[coefficients_length - i - 1] = tmp;
        }
    }


    /* solve */
    
    cfprintf(stderr, "Solving equation:");
    for (size_t i = coefficients_length - 1; i > 0; --i)
    {
        cfprintf(stderr, " %c %gx^%"PRIuMAX, (i + 1 != coefficients_length ? '+' : ' '), coefficients[i], (uintmax_t)i);
    }
    cfprintf(stderr, " + %g\n", coefficients[0]);


    {
        uint64_t res_code;
        struct sqsv_equation_t eq = {
            .coefficients = coefficients,
            .coefficients_length = coefficients_length,
        };
        struct sqsv_solution_t slv = {
            .flags = SQSV_SOLUTION_FLAG_DEFAULT,
            .roots = NULL,
            .roots_length = 0,
        };
        
        res_code = sqsv_solve_equation(&eq, &slv);
        if (res_code == SQSV_ERROR_CODE_INFINITE_SOLUTIONS)
        {
            cfprintf(stderr, "error: equation have infinite solutions\n");
            printf("inf\n");

            free(coefficients);
            return 2;
        }
        if (SQSV_ERROR(res_code))
        {
            fprintf(stderr, "error in call of sqsv_solve_equation. code %08x%08x\n", (uint32_t)(res_code >> 32), (uint32_t)res_code);

            free(coefficients);
            return 1;
        }

        slv.roots = malloc(sizeof(*slv.roots) * slv.roots_length);

        res_code = sqsv_solve_equation(&eq, &slv);
        if (res_code == SQSV_ERROR_CODE_INFINITE_SOLUTIONS)
        {
            cfprintf(stderr, "error: equation have infinite solutions\n");
            printf("inf\n");

            free(coefficients);
            return 2;
        }
        if (SQSV_ERROR(res_code))
        {
            fprintf(stderr, "error in call of sqsv_solve_equation. code %08x%08x\n", (uint32_t)(res_code >> 32), (uint32_t)res_code);

            free(coefficients);
            return 1;
        }

        cfprintf(stderr, "Found %"PRIuMAX" roots:\n", (uintmax_t)slv.roots_length);
        for (size_t i = 0; i < slv.roots_length; ++i)
        {
            printf("%g\n", slv.roots[i]);
        }
    }

    free(coefficients);
    
    
    return 0;
}



/*
    Takes equation formula from user, allocates memory and return it into "coefficients". 
    returns equation length in "coefficients_length"

    if error occurs, *coefficients will be NULL (and return code != 0)
*/
#define cmd_arg_flags flags
uint64_t read_coefficients(uint64_t flags, double **coefficients, size_t *coefficients_length)
{
    int ch;
    uintmax_t read_coefficients_length = 0;
    while (1)
    {
        while (1) 
        {
            cfprintf(stderr, "Enter power of equation [integer] > ");
            if (scanf("%"PRIuMAX, &read_coefficients_length) == 1)
            {
                break;
            }
            do { ch = getchar(); } 
            while (ch != EOF && ch != '\n');
        }

        if (read_coefficients_length <= MAX_POWER)
        {
            break;
        }
        cfprintf(stderr, "Power is too big. Max power is : %"PRIuMAX"\n", (uintmax_t)MAX_POWER);
    }

    *coefficients_length = read_coefficients_length + 1;
    *coefficients = malloc(sizeof(**coefficients) * (*coefficients_length));

    size_t id = 0;
    
    while (id < *coefficients_length)
    {
        (*coefficients)[id] = 0.0;
        while (1)
        {
            cfprintf(stderr, "Enter coefficient at x^%"PRIuMAX" >", (uintmax_t)id);
            if (scanf("%lg", (*coefficients) + id) == 1)
            {
                break;
            }
            do { ch = getchar(); } 
            while (ch != EOF && ch != '\n');
        }
        id++;
    }

    return 0;
}
#undef cmd_arg_flags
