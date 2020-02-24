#include "F2837XD_device.h"
#include "math.h"

/**
 * main.c
 */
float32 x[5000];
float32 y[5000];
Uint32 fast = 0;
#define pi 3.14159
int main(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0x0068;
    ClkCfgRegs.SYSPLLMULT.all = 0;
    ClkCfgRegs.SYSCLKDIVSEL.all = 0;
    WdRegs.WDCR.all = 0x0028;
    EDIS;
    float32 xDum = 0;
    while(1) {
        EALLOW;
           WdRegs.WDKEY.all = 0x0055;
           WdRegs.WDKEY.all = 0x00AA;
        EDIS;
        while(fast < 5000) {
            EALLOW;
                 WdRegs.WDKEY.all = 0x0055;
                 WdRegs.WDKEY.all = 0x00AA;
            EDIS;
            x[fast] = xDum;
            y[fast] = cos(2*pi*x[fast]);
            fast = fast + 1;
            xDum = (fast -0.001)/5000;
        }
    }
	return 0;
}
