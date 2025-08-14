#ifndef SQRT_SOLVER
#define SQRT_SOLVER

#include "inttypes.h"


enum sqsv_equation_type
{
    SQSV_EQUATION_TYPE_SQUARE,
    SQSV_EQUATION_TYPE_MAX,
};


struct sqsv_equation_t
{
    enum sqsv_equation_type type;
    double *coefficients;
};


double sqsv_solve_equation(struct equation_t *equation, struct roots_t *roots);


#endif
