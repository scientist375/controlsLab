#include "F2837XD_device.h"
#include "math.h"
#define pi 3.14159


interrupt void timerISR(void);

/**
 * main.c
 */
Uint32 counter = 0;
Uint32 subCount = 0;
float32 voltValue = 0;
int32 output = 0;
float32 x[2000];
float32 y[2000];
Uint32 ndCount = 0;
int main(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0x0068;
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1; //Setting up external oscillator
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0; //Disables PLL
    ClkCfgRegs.SYSPLLMULT.bit.IMULT = 20; //Setting multiplication factor to 20
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0b000000; //Setting division factor to 1
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1); //Waits for PLL clock to lock
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1; //Activates the PLL, forces XTAL to go through PLL

    //Now we have a cpu clock of freuqency 200MHz

    //Set the GPI00 pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0b01;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0b00;
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;

    //Set the GPI01 pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0b01;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0b00;
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;

    //Set the GPI02 pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0b01;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 0b00;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;

    //Set the GPI03 pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0b01;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0b00;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;

    //Set the GPI025 pin to enable EN_GATE
    GpioCtrlRegs.GPDMUX2.bit.GPIO124 = 0b00;
    GpioCtrlRegs.GPDGMUX2.bit.GPIO124 = 0b00;
    GpioCtrlRegs.GPDDIR.bit.GPIO124 = 1;
    GpioCtrlRegs.GPDPUD.bit.GPIO124 = 1;

    GpioDataRegs.GPDSET.bit.GPIO124 = 1;

    //Setting PWM clock
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 0x1;
    //Enabling module clocks for EPWM1 and EPWM2
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    asm(" NOP");
    asm(" NOP");
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    asm(" NOP");
    asm(" NOP");

    //Setting carrier wave to be triangular
    EPwm1Regs.TBCTL.bit.CTRMODE = 0b10;
    EPwm2Regs.TBCTL.bit.CTRMODE = 0b10;

    //Setting the frequency of carrier wave to be 2000 Hz
    EPwm1Regs.TBPRD = 2000;
    EPwm2Regs.TBPRD = 2000;
    //Setting frequency of time based clock of PWM module
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm2Regs.TBCTL.bit.CLKDIV = 0;

    //Toggle output pins of EPWM module
    EPwm1Regs.AQCTLA.bit.CAD = 0b10; //Force output high when TBCTR < carrier wave
    EPwm1Regs.AQCTLA.bit.CAU = 0b01; //Force output low when TBCTR > carrier wave
    EPwm2Regs.AQCTLB.bit.CAD = 0b10; //Force output high when TBCTR < carrier wave
    EPwm2Regs.AQCTLB.bit.CAU = 0b01; //Force output low when TBCTR > carrier wave

    EPwm1Regs.AQCTLB.bit.CAD = 0b01; //Force output low when TBCTR < carrier wave
    EPwm1Regs.AQCTLB.bit.CAU = 0b10; //Force output high when TBCTR > carrier wave
    EPwm2Regs.AQCTLA.bit.CAD = 0b01; //Force output low when TBCTR < carrier wave
    EPwm2Regs.AQCTLA.bit.CAU = 0b10; //Force output high when TBCTR > carrier wave

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;

    //Setting up QEP
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO20 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO20 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 1;

    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1;

    CpuSysRegs.PCLKCR4.bit.EQEP1 = 1;
    asm(" NOP");
    asm(" NOP");

    EQep1Regs.QPOSMAX = 0xFFFFFFFF;
    EQep1Regs.QEPCTL.bit.QPEN = 1;
    EQep1Regs.QEPCTL.bit.SWI = 1;

    CpuTimer0Regs.PRD.all = 199999;

    CpuTimer0Regs.TCR.bit.TSS = 1; //Turns timer off
    CpuTimer0Regs.TCR.bit.TRB = 1; //Reloads Timer
    CpuTimer0Regs.TCR.bit.TIE = 1; //Enables interrupt
    CpuTimer0Regs.TCR.bit.TSS = 0; //Starts timer

    //Now we have a timer_0 with frequency 1KHz


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
        counter = counter + 1;
        if (counter == 200) {
            subCount = subCount + 1;
            counter = 0;
            if(subCount == 4) {
                subCount = 0;
            }
        }
        if (subCount == 0) {
            voltValue = 20;
        }
        else if (subCount == 1) {
            voltValue = 0;
        }
        else if (subCount == 2) {
            voltValue = -20;
        }
        if (subCount == 3){
            voltValue = 0;
        }

        float32 duty = 0.5*(1+(voltValue/24))*2000;
        Uint16 dummy = (Uint16)(duty);
        EPwm1Regs.CMPA.bit.CMPA = dummy;
        EPwm2Regs.CMPA.bit.CMPA = dummy;

        output = EQep1Regs.QPOSCNT;
        float32 ang = (float32)(2*pi/1000)*output;
        if(ndCount < 2000) {
            x[ndCount] = voltValue;
            y[ndCount] = ang;
            ndCount = ndCount +1;
        }
        PieCtrlRegs.PIEACK.all = 1;
    }


