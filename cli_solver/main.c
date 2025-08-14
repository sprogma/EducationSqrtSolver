#include "sqsv/sqsv.h"

#include "inttypes.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"


#define MAX_POWER 2
#define MAX_COEFF_LENGTH ((MAX_POWER) + 1)

#define CMD_ARG_FLAG_SILENT 0x00002

/* conditional printf */
#define cprintf(...) do {if (!(cmd_arg_flags | CMD_ARG_FLAG_SILENT)) { printf(__VA_ARGS__); }} while (0)
#define cfprintf(...) do {if (!(cmd_arg_flags | CMD_ARG_FLAG_SILENT)) { fprintf(__VA_ARGS__); }} while (0)

uint64_t read_coefficients(uint64_t flags, double *coefficients, size_t *coefficients_length);

int main(int argc, char **argv)
{
    double *coefficients = malloc(sizeof(*coefficients) * MAX_COEFF_LENGTH);
    size_t coefficients_length = 0;
    uint32_t coefficients_set = 0;
    uint64_t cmd_arg_flags = 0;
    
    /* parse cmdline args */
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "--"))
        {
            break;
        }
        else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            if (!(cmd_arg_flags | CMD_ARG_FLAG_SILENT))
            {
                puts(
                    #include ".help.inc"
                );
            }
            return 0;
        }
        else if (!strcmp(argv[i], "--silent") || !strcmp(argv[i], "-s"))
        {
            cmd_arg_flags |= CMD_ARG_FLAG_SILENT;
        }
        else if (!strcmp(argv[i], "--coeff") || !strcmp(argv[i], "-c"))
        {
            int id = 0;
            while (i < argc)
            {
                char *end_ptr;
                if (id >= MAX_COEFF_LENGTH)
                {
                    cfprintf(stderr, "Warning: too many coefficients given. skip them.\n");
                    break;
                }
                coefficients[id] = strtod(argv[i], &end_ptr);
                if (end_ptr != argv[i])
                {
                    break;
                }
                id++;
                i++;
            }
            cfprintf(stderr, "Read command line coefficients: ");
            for (size_t i = 0; i < coefficients_length; ++i)
            {
                cfprintf(stderr, "%g ", coefficients[i]);
            }
            cfprintf(stderr, "\n");
            coefficients_set = -1;
        }
        else
        {
            cfprintf(stderr, "Unknown command line flag: %s\n", argv[i]);
        }
    }

    if (!coefficients_set)
    {
        uint64_t error_code = read_coefficients(cmd_arg_flags, coefficients, &coefficients_length);
        if (error_code != 0)
        {
            return 0x1000 | error_code;
        }
    }

    /* solve */
    
    cfprintf(stderr, "Solving equation:");
    for (int i = MAX_COEFF_LENGTH - 1; i > 0; --i)
    {
        cfprintf(stderr, " %c %gx^%d", (i != MAX_COEFF_LENGTH - 1 ? '+' : ' '), coefficients[i], i);
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
        }
        if (SQSV_ERROR(res_code))
        {
            fprintf(stderr, "error in call of sqsv_solve_equation. code %08x%08x\n", (uint32_t)(res_code >> 32), (uint32_t)res_code);
            return 1;
        }

        slv.roots = malloc(sizeof(*slv.roots) * slv.roots_length);

        res_code = sqsv_solve_equation(&eq, &slv);
        if (res_code == SQSV_ERROR_CODE_INFINITE_SOLUTIONS)
        {
            cfprintf(stderr, "error: equation have infinite solutions\n");
        }
        if (SQSV_ERROR(res_code))
        {
            fprintf(stderr, "error in call of sqsv_solve_equation. code %08x%08x\n", (uint32_t)(res_code >> 32), (uint32_t)res_code);
            return 1;
        }

        cfprintf(stderr, "Found roots:\n");
        for (size_t i = 0; i < slv.roots_length; ++i)
        {
            cfprintf(stdout, "%g\n", slv.roots[i]);
        }
    }
    
    
    return 0;
}

uint64_t read_coefficients(uint64_t flags, double *coefficients, size_t *coefficients_length)
{
    (void)flags;
    (void)coefficients;
    (void)coefficients_length;
    return 0;
}
