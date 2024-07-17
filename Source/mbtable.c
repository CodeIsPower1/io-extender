#include <stdint.h>

#include "mbtable.h"


static uint16_t mbtable[IO_STATE_QTY];

void mbtable_set(uint16_t raddr,uint16_t value) {
    if(raddr < IO_STATE_QTY) {
        mbtable[raddr] = value;
    }
}

int mbtable_get(uint16_t raddr,uint16_t *value) {
    if(raddr < IO_STATE_QTY) {
        *value = mbtable[raddr];
        return 1;
    }
    return 0;
}