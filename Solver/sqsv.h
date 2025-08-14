#ifndef SQRT_SOLVER
#define SQRT_SOLVER

#include "inttypes.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __WIN32
    #define SQSV_DYNAMIC_EXPORT __declspec(dllexport)
#else
    #define SQSV_DYNAMIC_EXPORT
#endif


#define SQSV_ERROR(error_code) ((error_code) != 0)
enum
{
    SQSV_RETURN_CODE_OK,
    SQSV_ERROR_CODE_INFINITE_SOLUTIONS,
    SQSV_ERROR_CODE_INVALID_ARGUMENT,
    SQSV_ERROR_CODE_INVALID_EQUATION_POWER,
};


struct sqsv_equation_t
{
    double *coefficients;
    size_t coefficients_length;
};

struct sqsv_solution_t
{
    double epsilon;
    double *roots;
    size_t roots_length;
};


/*
    This function solves given squation and stores roots in given structure

    parameters:
        equation - structure containing equation to solve.
                   .coefficients        - coefficients
                   .coefficients_length - power of equation + 1, 
                                          (so, it is length of 
                                           coefficients array)
        solution - structure which will be filled with roots.
                   .
                   .roots         - must be allocated by caller. 
                   .roots_length  - must contain allocated size of 
                                    `double *roots` array.
                                    if .roots is NULL, function
                                    returns total roots count in 
                                    roots_length.

    returns:
        zero on success
*/

SQSV_DYNAMIC_EXPORT uint64_t sqsv_solve_equation(struct sqsv_equation_t *equation, struct sqsv_solution_t *solution);



#ifdef __cplusplus
}
#endif


#endif
