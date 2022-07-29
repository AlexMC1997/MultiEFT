#include "./include/EFTFloat.h"
#include <stdio.h>

void test1()
{
    puts("Test 1");
    double a = 3.0;
    shadow_init(&a);
    double b = 0.00000000000000003;
    shadow_init(&b);
    double c = 0.0;
    shadow_sum(&a, &b, &c);
    double d = 0.0;
    shadow_sum(&b, &c, &d);
    shadow_print(&d);
}

void test2()
{
    puts("Test 2");
    double a = 3.0;
    shadow_init(&a);
    double b = 0.00000000000000003;
    shadow_init(&b);
    double c = 0.0;
    shadow_sum(&a, &b, &c);
    double d = 0.0;
    shadow_sum(&b, &c, &d);
    double a2 = 2.214182482184218;
    shadow_init(&a2);
    double a3 = 0.0;
    shadow_init(&a3);
    shadow_sum(&a2, &d, &a3);
    shadow_print(&a3);
}

void test3()
{
    puts("Test 3");
    double a = 3.0;
    shadow_init(&a);
    double b = 0.00000000000000003;
    shadow_init(&b);
    double c = 0.0;
    shadow_sum(&a, &b, &c);
    double d = 0.0;
    shadow_sum(&b, &c, &d);
    double a2 = 2.214182482184218;
    shadow_init(&a2);
    double a3 = 0.0;
    shadow_init(&a3);
    shadow_sum(&a2, &d, &a3);
    double a4 = 2.0;
    shadow_init(&a4);
    double a5 = 0.0;
    shadow_init(&a5);
    shadow_prod(&a4, &a3, &a5);
    shadow_print(&a5);
}

void test4()
{
    puts("Test 4");
    double a0 = 3.0;
    shadow_init(&a0);
    double a1 = 3.0;
    shadow_init(&a1);
    double a2 = 0;
    shadow_init(&a2);
    double a3 = 0;
    shadow_init(&a3);
    double a4 = 0;
    shadow_init(&a4);
    double a5 = 0;
    shadow_init(&a5);
    shadow_prod(&a0, &a1, &a2);
    shadow_prod(&a2, &a1, &a3);
    shadow_prod(&a3, &a1, &a4);
    shadow_prod(&a4, &a1, &a5);
    shadow_print(&a5);
}

int main(int argc, char* argv[])
{
    test1();
    puts("");
    test2();
    puts("");
    test3();
    puts("");
    test4();
}