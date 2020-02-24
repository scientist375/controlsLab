#include "F2837XD_device.h"
#include "math.h"


interrupt void timerISR(void);

/**
 * main.c
 */
Uint32 counter = 0;
int main(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0x0068;
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1; //Setting up external oscillator
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0; //Disables PLL
    ClkCfgRegs.SYSPLLMULT.bit.IMULT = 20; //Setting multiplication factor to 20
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0b000001; //Setting division factor to 2
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1); //Waits for PLL clock to lock
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1; //Activates the PLL, forces XTAL to go through PLL

    //Setting the GPIO34 pin as a normal input output pin
    GpioCtrlRegs.GPBMUX1.bit.GPIO34  = 00;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34  = 01;
    //Set the GPIO34 pin as an output pin
    GpioCtrlRegs.GPBDIR.bit.GPIO34  = 1;
    //Disables pullup resistor
    GpioCtrlRegs.GPBPUD.bit.GPIO34  = 1;

    //Setting the GPIO31 pin as a normal input output pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO31  = 00;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31  = 01;
    //Set the GPIO31 pin as an output pin
    GpioCtrlRegs.GPADIR.bit.GPIO31  = 1;
    //Disables pullup resistor
    GpioCtrlRegs.GPAPUD.bit.GPIO31  = 1;

    GpioDataRegs.GPASET.bit.GPIO31 = 1; //Turn both of them off
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;

    CpuTimer0Regs.PRD.all = 49999999;

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

        if(counter == 4){
        counter = 0;
        }


        if(counter == 0){
        GpioDataRegs.GPASET.bit.GPIO31 = 1;
        GpioDataRegs.GPBSET.bit.GPIO34 = 1;
        }
        else if(counter == 1){
        GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
        GpioDataRegs.GPBSET.bit.GPIO34 = 1;
        }
        else if(counter == 2){
        GpioDataRegs.GPASET.bit.GPIO31 = 1;
        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
        }
        else if(counter == 3){
        GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
        }
        counter = counter + 1;
        PieCtrlRegs.PIEACK.all = M_INT1;
    }

