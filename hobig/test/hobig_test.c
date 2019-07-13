#include <stdio.h>
#include "../hobig.c"

extern u64 div_word(u64 dividend_high, u64 dividend_low, u64 divisor, u64* out_remainder);
extern u64 mul_word(u64 val1, u64 val2, u64* higher);
extern u64 add_u64(u64 x, u64 y, u64 carry, u64* result);
extern u64 sub_u64(u64 x, u64 y, u64 carry, u64* result);

void mulAddWWW_g(u64 x, u64 y, u64 c, u64* z1, u64* z0) {
    u64 zz0 = mul_word(x, y, z1);
    *z0 = zz0 + c;
	if (*z0 < zz0) {
		(*z1)++;
	}
}

u64 mulAddVWW(HoBigInt z, HoBigInt x, u64 y, u64 r) {
	u64 c = r;
	for (int i = 0; i < array_length(z.value); ++i) {
        mulAddWWW_g(x.value[i], y, c, &c, &z.value[i]);
	}
    return c;
}

u64 addVV(u64* z, u64 zlen, u64* x, HoBigInt y) {
    u64 c = 0;
    for (u64 i = 0; i < zlen; ++i) {
        c = add_u64(x[i], y.value[i], c, &z[i]);
    }
    return c;
}

// The resulting carry c is either 0 or 1.
u64 subVV(u64* z, u64 zlen, u64* x, HoBigInt y) {
    u64 c = 0;
    for (u64 i = 0; i < zlen; ++i) {
        c = sub_u64(x[i], y.value[i], c, &z[i]);
    }
    return c;
}

int greater_than(u64 x1, u64 x2, u64 y1, u64 y2) {
	return ((x1 > y1) || (x1 == y1 && x2 > y2));
}

void hobig_int_normalize(HoBigInt* n) {
    int i = 1;
    for(; i < array_length(n->value) && n->value[array_length(n->value) - i] == 0; ++i);
    i--;
    array_length(n->value) -= i;
}


/*
    Adapted from Golang's implementation of divLarge
    https://golang.org/src/math/big/nat.go#L687

    Knuth, Volume 2, section 4.3.1, Algorithm D.
    Preconditions:
       array_length(v) >= 2
       array_lengthlen(u) >= array_length(v)
       u must not alias z
 */
HoBigInt_DivResult 
hobig_int_div_knuth(HoBigInt* u, HoBigInt* v) {
    TIME_COUNT();
    HoBigInt_DivResult result = {0};

    switch(hobig_int_compare_absolute(u, v)) {
        case -1: {
            result.quotient = hobig_int_new(0);
            result.remainder = hobig_int_copy(*u);
            return result;
        }
        case 0: {
            result.quotient = hobig_int_new(1);
            result.remainder = hobig_int_new(0);
            return result;
        }
        case 1: break;
    }

	int n = (int)array_length(v->value);
	int m = (int)array_length(u->value) - n;

	// D1
	int shift = hobig_int_leading_zeros_count(*v);

	HoBigInt v0 = hobig_int_copy(*v);
    hobig_int_shl(&v0, shift);

    HoBigInt u0 = hobig_int_copy(*u);
    array_push(u0.value, 0); // allocate 1 more word
    hobig_int_shl(&u0, shift);

    // Final quotient
    HoBigInt q = hobig_int_make(m + 1);
    array_length(q.value) = m + 1;

    HoBigInt qhatv = hobig_int_make(n + 1);
    array_length(qhatv.value) = n + 1;

    // D2
    u64 vn1 = v0.value[n - 1];

    for (int j = m; j >= 0; --j) {
		// D3
		u64 qhat = 0;
        u64 ujn = u0.value[j + n];
		if (ujn != vn1) {
            u64 rhat = 0;
            qhat = div_word(ujn, u0.value[j+n-1], vn1, &rhat);

			// x1 | x2 = q̂v_{n-2}
			u64 vn2 = v0.value[n-2];
            u64 x1 = 0;
            u64 x2 = mul_word(qhat, vn2, &x1);

			// test if q̂v_{n-2} > br̂ + u_{j+n-2}
			u64 ujn2 = u0.value[j+n-2];

            while (greater_than(x1, x2, rhat, ujn2)) {
				qhat--;
				u64 prevRhat = rhat;
				rhat += vn1;
				// v[n-1] >= 0, so this tests for overflow.
				if (rhat < prevRhat) {
					break;
				}
                x2 = mul_word(qhat, vn2, &x1);
			}
		}

		// D4.
        u64 prev_qhatlen = array_length(qhatv.value);
        array_length(qhatv.value) = n;
		qhatv.value[n] = mulAddVWW(qhatv, v0, qhat, 0);
        array_length(qhatv.value) = prev_qhatlen;

        u64 c = subVV(u0.value + j, array_length(qhatv.value), u0.value + j, qhatv);
		if (c != 0) {
            c = addVV(u0.value + j, n, u0.value + j, v0);
			u0.value[j+n] += c;
			qhat--;
		}

		q.value[j] = qhat;
	}

    hobig_free(qhatv);

    // normalize q
    hobig_int_normalize(&q);

    hobig_int_shr(&u0, shift);
    hobig_int_normalize(&u0);

    result.quotient = q;
    result.remainder = u0;

    TIME_END(TIME_SLOT_DIVIDE);
    return result;
}

u64 addMulVVW(u64* z, u64 zlen, HoBigInt x, u64 y) {
    u64 c = 0;

    for (u64 i = 0; i < zlen; ++i) {
        u64 z1 = 0, z0 = 0;
        mulAddWWW_g(x.value[i], y, z[i], &z1, &z0);
        c = add_u64(z0, c, 0, &z[i]);
		c += z1;
	}

	return c;
}

HoBigInt 
hobig_int_mul_fast(HoBigInt* x, HoBigInt* y) {
    TIME_COUNT();
    HoBigInt z = hobig_int_make(array_length(x->value) + array_length(y->value));
    array_length(z.value) = array_length(x->value) + array_length(y->value);
	for (u64 i = 0; i < array_length(y->value); ++i) {
        u64 d = y->value[i];
		if (d != 0) {
            z.value[array_length(x->value) + i] = addMulVVW(z.value + i, array_length(x->value), *x, d);
		}
	}

    TIME_END(TIME_SLOT_MULTIPLY);
	return z;
}

void test_hobig_div_knuth() {
    HoBigInt mod = hobig_int_new_decimal("21666702504857787886793638841817777498121126134302089344545705387038264104638108701946962254644439789069544779112431030802536249429364456978613012374820510911065695907417928646473918616160720523654431377042623126887404123197411400160326547299171020909417625243727890796840091859929623156916296551226209681481981576650632187297874777769601685623279503714849175951825203298008687580589518639830171068192340537060207061960546585056640181301195828416144172496217014188591013136765772758965747057573803612359717627543036296188888932613513340616975176414812691772576660438518958126167597118680937361950807754817355538540621", 0);
	HoBigInt pow = hobig_int_new_decimal("1283012387129873891273987129873981278937128979847981278947189274981729731928783921832173721938127937812977423912749823749823719473216746271364238174687324862178364217834821634763278468721364762178346721364872643786242873468264876238764826147236846321487", 0);
    
    //HoBigInt_DivResult r = hobig_int_div_knuth(&mod, &pow);

    //hobig_int_print(r.quotient);
    //printf("\n\n");
    //hobig_int_print(r.remainder);

    HoBigInt r = hobig_int_mul_fast(&mod, &pow);
    //hobig_int_mul(&mod, &pow);

    hobig_int_print(mod);
    printf("\n");

    return;
}

HoBigInt
hobig_int_mod_div_fast2(HoBigInt* n, HoBigInt* exp, HoBigInt* m) {
    HoBigInt answer = hobig_int_new(1);
    HoBigInt two = hobig_int_new(2);

    HoBigInt_DivResult r = hobig_int_div_knuth(n, m);

    HoBigInt base = r.remainder;
    hobig_free(r.quotient);

    HoBigInt e = hobig_int_copy(*exp);

	while (e.value[0] > 0) {
		if ((e.value[0] & 1) == 1) {
            //hobig_int_mul(&answer, &base);
            HoBigInt nansw = hobig_int_mul_fast(&answer, &base);
            hobig_free(answer);
            answer = nansw;
            
            HoBigInt_DivResult r = hobig_int_div_knuth(&answer, m);
            hobig_free(r.quotient);
			answer = r.remainder;
		}

        hobig_int_shr(&e, 1);

        //hobig_int_mul(&base, &base);
        HoBigInt sqbase = hobig_int_mul_fast(&base, &base);
        hobig_free(base);
        base = sqbase;

        HoBigInt_DivResult bb = hobig_int_div_knuth(&base, m);
        hobig_free(bb.quotient);
        hobig_free(base);
        base = bb.remainder;
	}

    return answer;
}

void test_hobig_fast_mod_exp() {
    HoBigInt b = hobig_int_new_decimal("152803812098390829305182759820218321730210989012805102740217037210371207401275871204012839012830921401273587123870312709327104780217581720707210372103710273120938102381207312879489217492794723897489217894729173481239567122739871287391739128390812098498231048012803950980921802047312658914689896492369812675182", 0);
	HoBigInt m = hobig_int_new_decimal("211092180492380942037432891589298127938471024718969698417582134797651659287981798469136566126736219957289817491270759926192412073217401712123213200200001020138921390128097571258197575757278758527582787381273812371204091275070270412739012830912830918208510971070120398120937120740170270321730912709371027508171098340981203412742173489271349872317498217394792138742139847213985621797498721897391273203808128401234791283759872974817289371892739656712924871208391289312093801273017287985127508098290183091283098129038092183092183098120127408127308712093809128309128390182980238059810298409128390218308120740217038120938129083120832113211092180492380942037432891589298127938471024718969698417582134797651659287981798469136566126736219957289817491270759926192412073217401712123213200200001020138921390128097571258197575757278758527582787381273812371204091275070270412739012830912830918208510971070120398120937120740170270321730912709371027508171098340981203412742173489271349872317498217394792138742139847213985621797498721897391273203808128401234791283759872974817289371892739656712924871208391289312093801273017287985127508098290183091283098129038092183092183098120127408127308712093809128309128390182980238059810298409128390218308120740217038120938129083120832113", 0);
	HoBigInt e = hobig_int_new_decimal("98739812798478971297351320571257120327507120357127502170934790174901285662981986641274687214218947982172321312381209839021830912748495663612736712351273425143561231294912804921749849857129878927358912365691629362936912639821738291739127289712471928741827391723921737123981273918279749817598127982374918247981221208312749182739789127317232817398179837198274891264912693712837213971297398127983712987391273912379128739", 0);
    HoBigInt r = hobig_int_mod_div_fast2(&b, &e, &m);

    hobig_int_print(r);
    printf("\n");

    print_time_slots();
}


int main(int argc, char** argv) {
    double start = os_time_us();

    //test_hobig_mod_div_fast();
    //test_hobig_div_knuth();
    test_hobig_fast_mod_exp();

    printf("\nelapsed: %.2fms\n", 0.001 * (os_time_us() - start));
    return 0;
}