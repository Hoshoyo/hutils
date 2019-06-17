#include <stdio.h>
#include "../hobig.c"

void test_hobig_mod_div() {
    // Result = 1
    // HoBigInt n   = hobig_int_new(5);
    // HoBigInt pow = hobig_int_new(117);
    // HoBigInt mod = hobig_int_new(19);

    //HoBigInt n   = hobig_new_dec("8197459871094878951798740923874981", 0);
    //HoBigInt pow = hobig_new_dec("4198783950798472098372176371283", 0);
    //HoBigInt mod = hobig_new_dec("23098419208371261834769749217491", 0);
    
    //HoBigInt n   = hobig_new_dec("819745987109487895179874092387498132897219172095798127985798127985791279579821789578109278049708129374213984782139479218373489127458948150747219", 0);
    //HoBigInt pow = hobig_new_dec("1290859827498321747329174872318947812783470823978492137849712394789012374923874921074924198783950798472098372176371283", 0);
    //HoBigInt mod = hobig_new_dec("2309841920837126183476974921749195092804918098102380218501804905842134214321412343444441412840912038012839081209858", 0);

    HoBigInt n   = hobig_int_new(2);
    HoBigInt pow = hobig_int_new(5);
    HoBigInt mod = hobig_int_new(14);

    HoBigInt r = hobig_int_mod_div(&n, &pow, &mod);

    hobig_int_print(r);
    printf("\n");

    hobig_free(n);

    HoBigInt r2 = hobig_int_mod_div(&r, &pow, &mod);
    hobig_int_print(r2);
    printf("\n");

    hobig_free(pow);
    hobig_free(mod);
    hobig_free(r);
    hobig_free(r2);
}

void test_gcd() {
    HoBigInt a = hobig_new_dec("819745987109487895179874092387498132897219172095798127985798127985791279579", 0);
    HoBigInt b = hobig_new_dec("90219309102930192093021322", 0);
    
    hobig_int_mul(&a, &b);

    HoBigInt gcd = hobig_int_gcd(&a, &b);

    hobig_int_print(gcd);
}

void miller_rabin_test() {
    //HoBigInt a = hobig_int_new(1223);
    HoBigInt a = hobig_new_dec("179426549", 0);
    hobig_int_print(a);
    
    int prime = miller_rabin_probably_prime(&a);

    if(prime) {
        printf(" is probably prime\n");
    } else {
        printf(" is composite\n");
    }
}

int main() {
    //test_hobig_mod_div();
    //test_gcd();
    miller_rabin_test();

    return 0;
}