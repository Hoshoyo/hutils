#include <stdio.h>
#include "../hobig.c"

void test_gcd() {
    HoBigInt a = hobig_new_dec("819745987109487895179874092387498132897219172095798127985798127985791279579", 0);
    HoBigInt b = hobig_new_dec("90219309102930192093021322", 0);
    
    hobig_int_mul(&a, &b);

    HoBigInt gcd = hobig_int_gcd(&a, &b);

    hobig_int_print(gcd);
}

void miller_rabin_test() {
    u64 primes[] = {
        179424887,	179425357,	179425819,	179426321,
        179424893,	179425373,	179425823,	179426323,
        179424899,	179425399,	179425849,	179426333,
        179424907,	179425403,	179425859,	179426339,
        179424911,	179425423,	179425867,	179426341,
        179424929,	179425447,	179425879,	179426353,
        179424937,	179425453,	179425889,	179426363,
        179424941,	179425457,	179425907,	179426369,
        179424977,	179425517,	179425943,	179426407,
        179424989,	179425529,	179425993,	179426447,
        179425003,	179425537,	179426003,	179426453,
        179425019,	179425559,	179426029,	179426491,
        179425027,	179425579,	179426081,	179426549,
    };

    // All primes
    for(int i = 0; i < sizeof(primes) / sizeof(u64); ++i) {
        HoBigInt a = hobig_int_new(primes[i]);
        hobig_int_print(a);
        
        int prime = hobig_is_prime(&a, 2);

        if(prime) {
            printf(" is probably prime\n");
        } else {
            printf(" is composite\n");
        }
    }

    HoBigInt t = hobig_random_possible_prime(512);
    int prime = miller_rabin_probably_prime(&t, 20);
    hobig_int_print(t);
    printf(" is probably prime (%d)\n", hobig_int_bitcount(&t));
}

void test_random() {
    HoBigInt max = hobig_new_dec("210391828052150182974921874891649786132789", 0);
    hobig_int_print(max);
    printf("\n\n");
    for(int i = 0; i < 10; ++i) {
        HoBigInt random = hobig_random(&max);
        hobig_int_print(random);
        printf("\n");
    }
}

void test_random_prime() {
    HoBigInt p = hobig_new_dec("102985109289408084018290847480515927498732847912379471293498172359013956910", 0);
    HoBigInt two = hobig_int_new(2);
    //for(int i = 0; i < 1000; ++i) {
    while(1) {
        hobig_int_add(&p, &two);
        if(hobig_is_prime(&p, 20)) {
            hobig_int_print(p);
            printf(" is prime\n");
            break;
        }
    }

    // Product of small primes = 16294579238595022365
}

void test_prime_generate() {
    HoBigInt prime = hobig_random_possible_prime(1024);
    hobig_int_print(prime);
}

void test_hobig_mod_div() {
    // Result = 1
    //HoBigInt n   = hobig_int_new(5);
    //HoBigInt pow = hobig_int_new(117);
    //HoBigInt mod = hobig_int_new(19);

    //HoBigInt n   = hobig_new_dec("8197459871094878951798740923874981", 0);
    //HoBigInt pow = hobig_new_dec("4198783950798472098372176371283", 0);
    //HoBigInt mod = hobig_new_dec("23098419208371261834769749217491", 0);
    
    //HoBigInt n   = hobig_new_dec("819745987109487895179874092387498132897219172095798127985798127985791279579821789578109278049708129374213984782139479218373489127458948150747219", 0);
    //HoBigInt pow = hobig_new_dec("1290859827498321747329174872318947812783470823978492137849712394789012374923874921074924198783950798472098372176371283", 0);
    //HoBigInt mod = hobig_new_dec("2309841920837126183476974921749195092804918098102380218501804905842134214321412343444441412840912038012839081209858", 0);

    //HoBigInt n   = hobig_int_new(2);
    //HoBigInt pow = hobig_int_new(5);
    //HoBigInt mod = hobig_int_new(14);

    HoBigInt n = hobig_new_dec("29153851883015721793957016276823707908502546851066569663763022871534335275262834996120957463949796085531354283741826106064909161161403881357762320676812704007600709572646827983599923756424101559683786293909314401522650886163823078780521109729533933900092598823110829671984608032400217893813524211529622224896", 0);
    HoBigInt pow = hobig_new_dec("38348114574017990776970221895888758125359881785102863001180098672193531669722059898632879126233526339474849751789627847340787177079180445748089546804556664944299998481316466483678252218973249411450363148661629974241020449996472230629808041850583845700788409979147599992637143165565121580810941804505957320449", 0);
    HoBigInt mod = hobig_new_dec("153392458296071963107880887583555032501439527140411452004720394688774126678888239594531516504934105357899399007158511389363148708316721782992358187218226659777199993925265865934713008875892997645801452594646519896964081799985888922519232167402335382803153639916590399970548572662260486323243767218023829281797", 0);

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

int main() {
    test_hobig_mod_div();
    //test_gcd();
    //miller_rabin_test();
    //test_random();
    //test_random_prime();
    //test_prime_generate();

    return 0;
}