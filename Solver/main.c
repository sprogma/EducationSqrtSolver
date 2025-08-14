#include "sqsv.h"




static uint64_t solve_constant_equation(double *coefficients, struct sqsv_solution_t *solution)
{
    (void)coefficients;
    (void)solution;
    return -1;
}


static uint64_t solve_linear_equation(double *coefficients, struct sqsv_solution_t *solution)
{
    (void)coefficients;
    (void)solution;
    return -1;
}

static uint64_t solve_square_equation(double *coefficients, struct sqsv_solution_t *solution)
{
    (void)coefficients;
    (void)solution;
    return -1;
}


uint64_t sqsv_solve_equation(struct sqsv_equation_t *equation, struct sqsv_solution_t *solution)
{
    if (equation == NULL || 
        equation->coefficients == NULL || 
        solution == NULL)
    {
        return SQSV_ERROR_CODE_INVALID_ARGUMENT;
    }
    
    switch (equation->coefficients_length)
    {
        case 0:
            return SQSV_ERROR_CODE_INFINITE_SOLUTIONS;
        case 1:
            return solve_constant_equation(equation->coefficients, solution);
        case 2:
            return solve_linear_equation(equation->coefficients, solution);
        case 3:
            return solve_square_equation(equation->coefficients, solution);
        default:
            return SQSV_ERROR_CODE_INVALID_EQUATION_POWER;
    }
}

