#include "permutations.h"

void ascon_p12(ascon_state_t *s) {

    STATE_VARS();
    s0 = s->x0;
    s1 = s->x1;
    s2 = s->x2;
    s3 = s->x3;
    s4 = s->x4;

    for (int i = P12_CONST; i >= 0x4b; i -= 0xf) {
      ROUND(i);
    }

    s->x0 = s0;
    s->x1 = s1;
    s->x2 = s2;
    s->x3 = s3;
    s->x4 = s4;

}

void ascon_p6(ascon_state_t *s) {

    STATE_VARS();

    s0 = s->x0;
    s1 = s->x1;
    s2 = s->x2;
    s3 = s->x3;
    s4 = s->x4;

    for (int i = P6_CONST; i >= 0x4b; i -= 0xf) {
      ROUND(i);
    }

    s->x0 = s0;
    s->x1 = s1;
    s->x2 = s2;
    s->x3 = s3;
    s->x4 = s4;



}