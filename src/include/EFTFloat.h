#ifndef _EFTFLOAT_H
#define _EFTFLOAT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SHADOW_SZ (2 << 20)
#define PRECISION 3

typedef struct FPair {
    double h;
    double l;
} FPair;

typedef struct EFTErr {
    double terms[PRECISION];
} EFTErr;

typedef struct EFTFloat {
    EFTErr err;
    double val;
    double* op1;
    double* op2;
    uint64_t tm;
} EFTFloat;

FPair two_f_add(double a, double b);
FPair two_f_mul(double a, double b);

EFTErr f_add(EFTErr r, double b);
EFTErr r_add(EFTErr a, EFTErr b);

EFTErr f_mul(EFTErr r, double b);
EFTErr r_mul(EFTErr a, EFTErr b);

EFTFloat eft_add(EFTFloat a, EFTFloat b);

void shadow_init(double* a);
void shadow_write(EFTFloat val, double* addr);
EFTFloat shadow_read(double* addr);
void shadow_sum(double* a, double* b, double* c);
void shadow_prod(double* a, double* b, double* c);

void eft_print(EFTFloat eft);
void shadow_print(double* a);


#endif