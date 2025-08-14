#include "sqsv.h"
#include "math.h"


#define SETROOTCOUNT(count) do { \
        if (solution->roots == NULL) \
        { \
            solution->roots_length = (count); \
            return SQSV_RETURN_CODE_OK; \
        } \
        if (solution->roots_length > (count)) \
        { \
            solution->roots_length = (count); \
        } \
    } while (0)
#define SETROOT(id, root) do { if (solution->roots_length > (id)) { solution->roots[(id)] = (root); } } while (0)

static uint64_t solve_constant_equation(double *coefficients, size_t coefficients_length, struct sqsv_solution_t *solution)
{
    (void)coefficients_length;
    
    if (fabs(coefficients[0]) < solution->epsilon)
    {
        return SQSV_ERROR_CODE_INFINITE_SOLUTIONS;
    }

    solution->roots_length = 0;
    return SQSV_RETURN_CODE_OK;
}


static uint64_t solve_linear_equation(double *coefficients, size_t coefficients_length, struct sqsv_solution_t *solution)
{
    (void)coefficients_length;

    if (fabs(coefficients[1]) < solution->epsilon)
    {
        return solve_constant_equation(coefficients, coefficients_length, solution);
    }

    SETROOTCOUNT(1);
    SETROOT(0, -coefficients[0] / coefficients[1]);
    
    return SQSV_RETURN_CODE_OK;
}

static uint64_t solve_square_equation(double *coefficients, size_t coefficients_length, struct sqsv_solution_t *solution)
{
    (void)coefficients_length;

    if (fabs(coefficients[2]) < solution->epsilon)
    {
        return solve_linear_equation(coefficients, coefficients_length, solution);
    }

    double d = coefficients[1] * coefficients[1] - 4.0 * coefficients[0] * coefficients[2];
    
    if (d <= -solution->epsilon)
    {
        solution->roots_length = 0;
        return SQSV_RETURN_CODE_OK;
    }
    
    /* adjust for sqrt call, to don't get nan. */
    if (d < 0.0)
    {
        d = 0.0;
    }

    double sqrt_d = sqrt(d);

    if (fabs(d) < solution->epsilon)
    {
        SETROOTCOUNT(1);    
        SETROOT(0, -coefficients[1] / coefficients[2] / 2.0);
        
        return SQSV_RETURN_CODE_OK;
    }
    
    SETROOTCOUNT(2);
    SETROOT(0, (-coefficients[1] - sqrt_d) / coefficients[2] / 2.0);
    SETROOT(1, (-coefficients[1] + sqrt_d) / coefficients[2] / 2.0);
    
    return SQSV_RETURN_CODE_OK;
}

#undef SETROOT

uint64_t sqsv_solve_equation(struct sqsv_equation_t *equation, struct sqsv_solution_t *solution)
{
    if (equation == NULL || 
        equation->coefficients == NULL || 
        solution == NULL)
    {
        return SQSV_ERROR_CODE_INVALID_ARGUMENT;
    }

    if (!(solution->flags & SQSV_SOLUTION_FLAG_CUSTOM_EPSILON))
    {
        solution->epsilon = 1e-8;
    }
    else if (solution->epsilon < 0.0)
    {
        return SQSV_ERROR_CODE_INVALID_ARGUMENT;
    }
    
    switch (equation->coefficients_length)
    {
        case 0:
            return SQSV_ERROR_CODE_INFINITE_SOLUTIONS;
        case 1:
            return solve_constant_equation(equation->coefficients, equation->coefficients_length, solution);
        case 2:
            return solve_linear_equation(equation->coefficients, equation->coefficients_length, solution);
        case 3:
            return solve_square_equation(equation->coefficients, equation->coefficients_length, solution);
        default:
            return SQSV_ERROR_CODE_INVALID_EQUATION_POWER;
    }
}

