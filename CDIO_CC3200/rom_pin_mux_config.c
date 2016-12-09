//*****************************************************************************
// rom_pin_mux_config.c
//
// configure the device pins for different signals
//
// Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// This file was automatically generated on 7/29/2014 at 9:57:43 AM
// by TI PinMux version 3.0.321

//
//*****************************************************************************

#include "pin_mux_config.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio_if.h" // Added afterwards

//*****************************************************************************
void PinMuxConfig(void)
{
	//
    // Enable Peripheral Clocks
    //
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_UARTA1, PRCM_RUN_MODE_CLK);

	//
    // Configure PIN_07 for UART1 UART1_TX
    //
	MAP_PinTypeUART(PIN_07, PIN_MODE_5);

	//
    // Configure PIN_08 for UART1 UART1_RX
    //
	MAP_PinTypeUART(PIN_08, PIN_MODE_5);

	//
	// Configure PIN_01 for GPIO Output
	//
	MAP_PinTypeGPIO(PIN_01, PIN_MODE_0, false);
	unsigned int GPIO10Port = 0;
	unsigned char GPIO10Pin;
	GPIO_IF_GetPortNPin(10, &GPIO10Port, &GPIO10Pin);
	MAP_GPIODirModeSet(GPIO10Port, GPIO10Pin, GPIO_DIR_MODE_OUT);


	//
	// Configure PIN_02 for GPIO Output
	//
	MAP_PinTypeGPIO(PIN_02, PIN_MODE_0, false);
	unsigned int GPIO11Port = 0;
	unsigned char GPIO11Pin;
	GPIO_IF_GetPortNPin(11, &GPIO11Port, &GPIO11Pin);
	MAP_GPIODirModeSet(GPIO11Port, GPIO11Pin, GPIO_DIR_MODE_OUT);

	//
	// Configure PIN_62 for GPIO Output
	//
	MAP_PinTypeGPIO(PIN_62, PIN_MODE_0, false);
	unsigned int GPIO7Port = 0;
	unsigned char GPIO7Pin;
	GPIO_IF_GetPortNPin(7, &GPIO7Port, &GPIO7Pin);
	MAP_GPIODirModeSet(GPIO7Port, GPIO7Pin, GPIO_DIR_MODE_OUT);


	//
    // Configure PIN_64 for GPIO Output
    //
	//MAP_PinTypeGPIO(PIN_64, PIN_MODE_0, false);
	//MAP_GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

	/*
	//
	// Configure PIN_64 for GPIO Output
	//
	MAP_PinTypeGPIO(PIN_64, PIN_MODE_0, false);
	unsigned int GPIO9Port = 0;
	unsigned char GPIO9Pin;
	GPIO_IF_GetPortNPin(9, &GPIO9Port, &GPIO9Pin);

	MAP_GPIODirModeSet(GPIO9Port, GPIO9Pin, GPIO_DIR_MODE_OUT);
	*/

	//
	// Configure PIN_64 for TimerPWM5 GT_PWM05
	//
	MAP_PinTypeTimer(PIN_64, PIN_MODE_3);


	//
    // Configure PIN_15 for GPIO Input - Switch 4
    //
	//MAP_PinTypeGPIO(PIN_15, PIN_MODE_0, false);
	//MAP_GPIODirModeSet(GPIOA2_BASE, 0x40, GPIO_DIR_MODE_IN);

/*
	//
	// Configure PIN_16 for GPIO Input - Switch 1
	//
	MAP_PinTypeGPIO(PIN_16, PIN_MODE_0, false);
	unsigned int GPIO23Port = 0;
	unsigned char GPIO23Pin;
	GPIO_IF_GetPortNPin(23, &GPIO23Port, &GPIO23Pin);
	MAP_GPIODirModeSet(GPIO23Port, GPIO23Pin, GPIO_DIR_MODE_IN);


	//
	// Configure PIN_17 for GPIO Input - Switch 2
	//
	MAP_PinTypeGPIO(PIN_17, PIN_MODE_0, false);
	unsigned int GPIO24Port = 0;
	unsigned char GPIO24Pin;
	GPIO_IF_GetPortNPin(24, &GPIO24Port, &GPIO24Pin);
	MAP_GPIODirModeSet(GPIO24Port, GPIO24Pin, GPIO_DIR_MODE_IN);

*/
	//
	// Configure PIN_55 for GPIO Input - Switch 3
	//
	MAP_PinTypeGPIO(PIN_55, PIN_MODE_0, false);
	unsigned int GPIO1Port = 0;
	unsigned char GPIO1Pin;
	GPIO_IF_GetPortNPin(1, &GPIO1Port, &GPIO1Pin);
	MAP_GPIODirModeSet(GPIO1Port, GPIO1Pin, GPIO_DIR_MODE_IN);
	// Activate internal pull-down resistor
	MAP_PinConfigSet(PIN_55, PIN_STRENGTH_6MA, PIN_TYPE_STD_PD);




	//
	// Configure PIN_15 for GPIO Input - Switch 4
	//
	MAP_PinTypeGPIO(PIN_15, PIN_MODE_0, false);
	unsigned int GPIO22Port = 0;
	unsigned char GPIO22Pin;
	GPIO_IF_GetPortNPin(22, &GPIO22Port, &GPIO22Pin);
	MAP_GPIODirModeSet(GPIO22Port, GPIO22Pin, GPIO_DIR_MODE_IN);
	// Activate internal pull-down resistor
	//MAP_PinConfigSet(PIN_15, PIN_STRENGTH_4MA, PIN_TYPE_STD_PD);


}
