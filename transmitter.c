/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Drivers */
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>
#include <ieee_cmd.h>
#include <transmitter.h>

/* Board Header files */
#include "Board.h"

/***** Defines *****/
#define TX_TASK_STACK_SIZE 1024
#define TX_TASK_PRIORITY   2

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

PIN_Config ledPinTable[] = {
Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
PIN_TERMINATE };

/* UART handle */
UART_Handle uart;
UART_Params uartParams;

#define RF_EVENT_MASK  ( RF_EventLastCmdDone | RF_EventCmdError | \
		RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled | \
		RF_EventRxAborted )

/*---------------------------------------------*/
// Init and set some RF Command Operations
/*---------------------------------------------*/
/***** Variable declarations *****/
static Task_Params txTaskParams;
Task_Struct txTask; /* not static so you can see in ROV */
static uint8_t txTaskStack[TX_TASK_STACK_SIZE];

/* TX Configuration */
#define PAYLOAD_LENGTH      30
#define PACKET_INTERVAL     (uint32_t)(4000000*0.5f) /* Set packet interval to 500ms */

static RF_Object rfObject;
static RF_Handle rfHandle;

uint32_t time;
static uint8_t packet[PAYLOAD_LENGTH];
static uint16_t seqNumber;
static PIN_Handle pinHandle;

char statusmsg[30];
char pktmsg[] = "THIS IS SOME DATA I WANT TO SEE SOMEWHERE ELSE";

/***** Prototypes *****/
static void txTaskFunction(UArg arg0, UArg arg1);

/***** Function definitions *****/
void TxTask_init(PIN_Handle inPinHandle) {
	pinHandle = inPinHandle;

	Task_Params_init(&txTaskParams);
	txTaskParams.stackSize = TX_TASK_STACK_SIZE;
	txTaskParams.priority = TX_TASK_PRIORITY;
	txTaskParams.stack = &txTaskStack;
	txTaskParams.arg0 = (UInt) 1000000;

	Task_construct(&txTask, txTaskFunction, &txTaskParams, NULL);

	PIN_setOutputValue(ledPinHandle, Board_LED1, 1);
}

static void txTaskFunction(UArg arg0, UArg arg1) {
	UART_write(uart, "\fConfiguring Task Params...",
			sizeof("\fConfiguring Task Params..."));
	uint32_t time;
	RF_Params rfParams;
	RF_Params_init(&rfParams);

	ext_packetTXcmd.payloadLen = PAYLOAD_LENGTH;
	ext_packetTXcmd.pPayload = packet;
	ext_packetTXcmd.startTrigger.triggerType = TRIG_ABSTIME;
	ext_packetTXcmd.startTrigger.pastTrig = 1;
	ext_packetTXcmd.startTime = 0;
	UART_write(uart, "Done\n", sizeof("Done\n"));
	UART_write(uart, "\fLaunching Radio...", sizeof("\fLaunching Radio..."));
	/* Request access to the radio */
	rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*) &ext_radiosetup,
			&rfParams);
	UART_write(uart, "Done\n", sizeof("Done\n"));
	UART_write(uart, "\fPosting Freq Synth Config...",
			sizeof("\fPosting Freq Synth Config..."));
	/* Set the frequency */
	RF_postCmd(rfHandle, (RF_Op*) &ext_radiofscmd, RF_PriorityNormal, NULL, 0);
	UART_write(uart, "Done\n", sizeof("Done\n"));
	/* Get current time */
	time = RF_getCurrentTime();
	UART_write(uart, "\fEntering Loop...\n", sizeof("\fEntering Loop...\n"));
	memset(statusmsg, 0, sizeof(statusmsg));
	System_sprintf(statusmsg, "\fTime:  %d\n", time);
	UART_write(uart, statusmsg, sizeof(statusmsg));
	while (1) {
		/* Create packet with incrementing sequence number and random payload */
		packet[0] = (uint8_t) (seqNumber >> 8);
		packet[1] = (uint8_t) (seqNumber++);
		uint8_t i;
		for (i = 2; i < PAYLOAD_LENGTH; i++) {
			packet[i] = rand();
		}

		/* Set absolute TX time to utilize automatic power management */
		time += PACKET_INTERVAL;
		ext_packetTXcmd.startTime = time;
		ext_packetTXcmd.timeStamp = time;
		// ext_packetTXcmd.pNextOp = (RF_Op*)&ext_radiostopcmd;
		/* Send packet */
		UART_write(uart, "\fSending Packet...", sizeof("\fSending Packet..."));
//		RF_CmdHandle txhndl = RF_postCmd(rfHandle, (RF_Op*)&ext_packetTXcmd, RF_PriorityNormal, NULL, 0);
//		RF_EventMask result = RF_pendCmd(txhndl, rfHandle, RF_EVENT_MASK);
		RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*) &ext_packetTXcmd, RF_PriorityNormal, NULL, 0);
		//if (!((result & RF_EventLastCmdDone) | (result & RF_EventTxDone))) {
//		if (!((result & RF_EventTxDone))) {
//			/* Error */
//			UART_write(uart, "ERROR\n", sizeof("ERROR\n"));
//			while (1)
//				;
//		}

		PIN_setOutputValue(pinHandle, Board_LED1,
				!PIN_getOutputValue(Board_LED1));
		UART_write(uart, "Done\n", sizeof("Done\n"));
	}
}

/*
 *  ======== main ========
 */
int main(void) {
	Task_Params taskParams;

	/* Call board init functions */
	Board_initGeneral()
	;
	Board_initUART();

	/* Create a UART with data processing off. */
	UART_Params_init(&uartParams);
	uartParams.writeDataMode = UART_DATA_BINARY;
	uartParams.readDataMode = UART_DATA_TEXT;
	uartParams.readReturnMode = UART_RETURN_NEWLINE;
	uartParams.readEcho = UART_ECHO_OFF;
	uartParams.readTimeout = 100;
	uartParams.baudRate = 115200;
	uart = UART_open(Board_UART0, &uartParams);

	/* Open LED pins */
	ledPinHandle = PIN_open(&ledPinState, ledPinTable);
	if (!ledPinHandle) {
		System_abort("Error initializing board LED pins\n");
	}

	TxTask_init(ledPinHandle);

	System_printf("Starting the example\nSystem provider is set to SysMin. "
			"Halt the target to view any SysMin contents in ROV.\n");
	/* SysMin will only print to the console when you call flush or exit */
	System_flush();

	/* Start BIOS */
	BIOS_start();

	return (0);
}
