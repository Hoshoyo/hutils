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
    HoBigInt n   = hobig_new_dec("819745987109487895179874092387498132897219172095798127985798127985791279579821789578109278049708129374213984782139479218373489127458948150747219", 0);
    HoBigInt pow = hobig_new_dec("1290859827498321747329174872318947812783470823978492137849712394789012374923874921074924198783950798472098372176371283", 0);
    HoBigInt mod = hobig_new_dec("2309841920837126183476974921749195092804918098102380218501804905842134214321412343444441412840912038012839081209858", 0);

    double s = os_time_us();
    HoBigInt r = hobig_int_mod_div(&n, &pow, &mod);
    printf("Total:  %f ms\n", 0.001 * (os_time_us() - s));

    print_time_slots();

    hobig_int_print(r);
    printf("\n");

    hobig_free(n);
    hobig_free(pow);
    hobig_free(mod);
    hobig_free(r);

}

int main() {
    test_hobig_mod_div();

    return 0;
}