#include "F2837XD_device.h"
#include "math.h"


interrupt void timerISR(void);

/**
 * main.c
 */
Uint32 test = 0;
int main(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0x0068;
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1; //Setting up external oscillator
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0; //Disables PLL
    ClkCfgRegs.SYSPLLMULT.bit.IMULT = 2; //Setting multiplication factor to 10
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0b000000; //Setting division factor to 6
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1); //Waits for PLL clock to lock
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1; //Activates the PLL, forces XTAL to go through PLL

    //Set the GPI0 pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 01;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;

    CpuTimer0Regs.PRD.all = 9999;

    CpuTimer0Regs.TCR.bit.TSS = 1; //Turns timer off
    CpuTimer0Regs.TCR.bit.TRB = 1; //Reloads Timer
    CpuTimer0Regs.TCR.bit.TIE = 1; //Enables interrupt
    CpuTimer0Regs.TCR.bit.TSS = 0; //Starts timer


    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieVectTable.TIMER0_INT = &timerISR;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    IER = 1;
    EINT;
    WdRegs.WDCR.all = 0x0028;
    EDIS;
    while(1) {
            EALLOW;
            WdRegs.WDKEY.all = 0x0055;
            WdRegs.WDKEY.all = 0x00AA;
            EDIS;

    }
    return 0;
}

    interrupt void timerISR(void)
    {

        GpioDataRegs.GPATOGGLE.bit.GPIO6 = 1;
        test = GpioDataRegs.GPADAT.bit.GPIO6;
        PieCtrlRegs.PIEACK.all = M_INT1;
    }

