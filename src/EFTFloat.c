#include "./include/EFTFloat.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

static EFTFloat g_shadow_mem[SHADOW_SZ];
static uint64_t g_tm = 0;

FPair two_f_add(double a, double b) 
{
    FPair res;
    float h = a + b;
    float bv = h - a;
    float av = h - bv;
    float br = b - bv;
    float ar = a - av;
    res.l = br + ar;
    res.h = h;
    return res;
}

FPair two_f_mul(double a, double b)
{
    FPair res;
    res.h = a * b;
    res.l = fma(a, b, -res.h);
    return res;
}

//Needs testing
void err_compress(double* err, size_t prec)
{
    FPair Q;
    double g[prec];
    Q.h = err[prec - 1];
    size_t bot = prec - 1;
    for (ssize_t i = prec - 1; i >= 0; i--) {
        Q = two_f_add(Q.h, err[i]);
        if (Q.l != 0) {
            g[bot] = Q.h;
            bot--;
            Q.h = Q.l;
        }
    }
    g[bot] = Q.h;
    size_t top = 0;
    for (size_t i = bot + 1; i < prec; i++) {
        Q = two_f_add(g[i], Q.h);
        if (Q.l != 0) {
            g[top] = Q.h;
            top++;
        }
    }
    g[top] = Q.h;
    memset(err, 0, prec * sizeof(double));
    memcpy(err, g, top * sizeof(double));
}

EFTErr f_add(EFTErr r, double b)
{
    EFTErr res;
    FPair t;
    t.l = b;
    for (size_t i = 0; i < PRECISION; i++) {
        t = two_f_add(r.terms[i], t.l);
        res.terms[i] = t.h;
    }
    // err_compress(res.terms, PRECISION);
    for (size_t i = 0; i < PRECISION; i++) {
        if (res.terms[i] == 0) {
            res.terms[i] = t.l;
            break;
        }
    }
    return res;
}

EFTErr r_add(EFTErr a, EFTErr b)
{
    EFTErr r = a;
    for (size_t i = 0; i < PRECISION; i++)
        r = f_add(r, b.terms[i]);
    return r;
}

EFTErr f_mul(EFTErr r, double b)
{
    EFTErr res;
    double tmp[PRECISION * 2];
    memset(tmp, 0, 2 * PRECISION * sizeof(double));
    FPair Q = two_f_mul(r.terms[0], b);
    tmp[0] = Q.l;
    for (size_t i = 1; i < PRECISION; i++) {
        FPair T = two_f_mul(r.terms[i], b);
        Q = two_f_add(Q.h, T.l);
        tmp[(2 * i) - 1] = Q.l;
        Q = two_f_add(T.h, Q.h);
        tmp[2 * i] = Q.l;
    }  
    tmp[2 * PRECISION - 1] = Q.h;
    // err_compress(tmp, PRECISION * 2);
    size_t top = 0;
    for (size_t i = 0; i < PRECISION * 2; i++) {
        if (tmp[i] != 0)
            top = i;
    }
    if (top > PRECISION - 1)
        memcpy(res.terms, tmp + top - PRECISION + 1, PRECISION * sizeof(double));
    else
        memcpy(res.terms, tmp, PRECISION * sizeof(double));
    return res;
}

EFTErr r_mul(EFTErr a, EFTErr b)
{
    EFTErr terms[PRECISION];
    for (size_t i = 0; i < PRECISION; i++) 
        terms[i] = f_mul(a, b.terms[i]);
    EFTErr res = terms[0];
    for (size_t i = 1; i < PRECISION; i++) 
        res = r_add(res, terms[i]);
}

EFTFloat eft_add(EFTFloat a, EFTFloat b)
{
    EFTFloat res;
    FPair tmp = two_f_add(a.val, b.val);
    res.val = tmp.h;
    res.err = f_add(a.err, tmp.l);
    res.err = r_add(res.err, b.err);
    return res;
}

EFTFloat eft_mul(EFTFloat a, EFTFloat b)
{
    EFTFloat res;
    FPair tmp = two_f_mul(a.val, b.val);
    res.val = tmp.h;
    res.err = r_add(f_mul(a.err, b.val), f_mul(b.err, a.val));
    res.err = r_add(res.err, r_mul(b.err, a.err));
    res.err = f_add(res.err, tmp.l);
    return res;
}

void shadow_init(double* a)
{
    EFTFloat eft_a;
    memset(&eft_a, 0, sizeof(EFTFloat));
    eft_a.tm = g_tm;
    eft_a.val = *a;
    g_tm++;
    shadow_write(eft_a, a);
}

EFTFloat shadow_read(double* addr)
{
    return g_shadow_mem[(((size_t)addr) >> 3) & (SHADOW_SZ - 1)];
}

void shadow_write(EFTFloat val, double* addr)
{
    g_shadow_mem[(((size_t)addr) >> 3) & (SHADOW_SZ - 1)] = val;
}

void shadow_sum(double* a, double* b, double* c)
{
    EFTFloat eft_a = shadow_read(a);
    EFTFloat eft_b = shadow_read(b);
    EFTFloat res = eft_add(eft_a, eft_b);
    res.tm = g_tm;
    g_tm++;
    res.op1 = a;
    res.op2 = b;
    shadow_write(res, c);
    *c = res.val;
}

void shadow_prod(double* a, double* b, double* c)
{
    EFTFloat eft_a = shadow_read(a);
    EFTFloat eft_b = shadow_read(b);
    EFTFloat res = eft_mul(eft_a, eft_b);
    res.tm = g_tm;
    g_tm++;
    res.op1 = a;
    res.op2 = b;
    shadow_write(res, c);
    *c = res.val;
}

void eft_print(EFTFloat eft)
{
    printf("Value: %.100f\n", eft.val);
    printf("Error: %.100f", eft.err.terms[0]);
    for (size_t i = 1; i < PRECISION; i++) {
        if (eft.err.terms[i] == 0)
            continue;
        printf(" + %.100f", eft.err.terms[i]);
    }  
    EFTFloat op1 = shadow_read(eft.op1);
    EFTFloat op2 = shadow_read(eft.op2);
    printf("\nTimestamp: %lu\nOP1 ~ %.100f + %.100f\nOP2 ~ %.100f + %.100f\n", eft.tm, op1.val, op1.err.terms[0], op2.val, op2.err.terms[0]);
}

void shadow_print(double* a)
{
    eft_print(shadow_read(a));
}