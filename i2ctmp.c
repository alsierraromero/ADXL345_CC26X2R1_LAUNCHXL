/*
 * Copyright (c) 2018-2022, Texas Instruments Incorporated
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
 *  ======== i2ctmp.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Board.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayUart2.h>

// XDC module Header Files
#include <xdc/std.h>                                                            // XDC "base types" - must be included FIRST
#include <xdc/runtime/Types.h>                                                  // XDC constants/types used in xdc.runtime pkg
//#include <xdc/runtime/Error.h>                                                  // For error handling (e.g. Error block)
#include <xdc/runtime/System.h>                                                 // XDC System functions (e.g. System_abort(), System_printf())
#include <xdc/runtime/Log.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>                                                // Task APIs
#include <ti/sysbios/knl/Semaphore.h>                                           // Semaphore APIs


/* Driver configuration */
#include "ti_drivers_config.h"

/* Function like macros*/
#define SQUARED(x) pow(x,2)

/*Regular macros*/
#define TASKSTACKSIZE 640

/* I2C slave addresses */
#define I2C_ADD_ADXL345                 0x1D                                    //7 bit address with ALT ADDRESS pin high
#define I2C_NUMB_MSG                    6                                         //Max number of bytes in a transaction

/* Registers */
#define BW_RATE_REG                     0x2C
#define POWER_CTL_REG                   0x2D
#define DATA_FORMAT_REG                 0x31
#define DATAX0_REG                      0x32
#define DATAX1_REG                      0x33
#define DATAY0_REG                      0x34
#define DATAY1_REG                      0x35
#define DATAZ0_REG                      0x36
#define DATAZ1_REG                      0x37
#define FIFO_CTL_REG                    0x38
#define FIFO_STATUS_REG                 0x39

/* Configuration Register values */
#define BW_RATE_VALUE                 0x04                      //1.56 Hz
#define POWER_CTL_VALUE               0x08                      //Transition to measurement mode (from stand-by)

#define PI                            3.14159265
#define G_VALUE_2G_SCALE              255
#define OFFSET_Z_AXIS                 (double)-15.5                    //(Plus1G + Minus1G)/2. Check calibration https://www.digikey.com/en/articles/using-an-accelerometer-for-inclination-sensing
#define OFFSET_X_AXIS                 (double) -6
#define OFFSET_Y_AXIS                 (double) 0.5


Display_Handle display;
Semaphore_Handle semGetData;
Timer_Handle    timer0_h;
I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;
int8_t txBuffer[I2C_NUMB_MSG];
int8_t rxBuffer[I2C_NUMB_MSG];

/* Prototypes */
void myTimer_init(void);
//void taskGetDataFxn(void);
void timerFxnCB(Timer_Handle myHandle, int_fast16_t status);
double dataFormatting (int8_t* numb);

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    static double angle, angle1, offsetG;

    /* Call driver init functions */
    GPIO_init();
    Timer_init();
    Display_init();
    I2C_init();
    myTimer_init();

    Display_Params params;
    Display_Params_init(&params);

    /* Open the UART display for output */
    display = Display_open(Display_Type_UART, NULL);

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(I2C_ACCEL, &i2cParams);
    if (i2c == NULL) {
        Display_printf(display, 0, 0, "Error Initializing I2C\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "I2C Initialized!\n");
    }

    /* Accel Config */
    txBuffer[0] = BW_RATE_REG;                                                    //Writing two consecutive registers
    txBuffer[1] = BW_RATE_VALUE;
    txBuffer[2] = POWER_CTL_VALUE;

    i2cTransaction.slaveAddress = I2C_ADD_ADXL345;
    i2cTransaction.writeBuf   = txBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf    = rxBuffer;
    i2cTransaction.readCount  = 0;
    I2C_transfer(i2c, &i2cTransaction);

    //Task_Params taskParams;
    //Task_Params_init(&taskParams);
    //taskParams.priority = 3;
    //Task_create((Task_FuncPtr)taskGetDataFxn, &taskParams, Error_IGNORE);

    semGetData = Semaphore_create(0, NULL, Error_IGNORE);
    if (semGetData == NULL) {
      System_abort("Semaphore Get Data create failed");
    }

    while(1){
        Semaphore_pend(semGetData, BIOS_WAIT_FOREVER);
        txBuffer[0]= DATAX0_REG;
        i2cTransaction.writeCount = 1;
        i2cTransaction.readCount  = I2C_NUMB_MSG;
        I2C_transfer(i2c, &i2cTransaction);
        /*Measuring Z axis offset*/
        //offsetG = (double)dataFormatting(rxBuffer + 2);
        //Display_printf(display, 0, 0, "Offset: %f",offsetG);

        angle = acos((dataFormatting(rxBuffer + 4) - OFFSET_Z_AXIS)/(sqrt(SQUARED(dataFormatting(rxBuffer)-OFFSET_X_AXIS) + SQUARED(dataFormatting(rxBuffer + 2)-OFFSET_Y_AXIS) + SQUARED(dataFormatting(rxBuffer + 4)-OFFSET_Z_AXIS))))*180/PI;
        //angle1= atan(sqrt(SQUARED(dataFormatting(rxBuffer)) + SQUARED(dataFormatting(rxBuffer + 2)))/dataFormatting(rxBuffer + 4)) *180/PI;                                       //the result is the same using this formula
        Display_printf(display, 0, 0, "Angle against horizontal is %f degrees",angle);

    }

    return (NULL);
}

void myTimer_init(void){
    Timer_Params    tparams;
    Timer_Params_init(&tparams);
    tparams.periodUnits = Timer_PERIOD_HZ;
    tparams.period = 1;
    tparams.timerMode  = Timer_CONTINUOUS_CALLBACK;
    tparams.timerCallback = timerFxnCB;
    timer0_h = Timer_open(CONFIG_TIMER_0, &tparams);
    Timer_start(timer0_h);
}

void timerFxnCB(Timer_Handle myHandle, int_fast16_t status){
    GPIO_toggle(CONFIG_GPIO_LED_0);
    Semaphore_post(semGetData);
}


/*void taskGetDataFxn(void){
    double angle;
    while(1){
        Semaphore_pend(semGetData, BIOS_WAIT_FOREVER);
        txBuffer[0]= DATAX0_REG;
        i2cTransaction.writeCount = 1;
        i2cTransaction.readCount  = I2C_NUMB_MSG;
        I2C_transfer(i2c, &i2cTransaction);
        angle = dataFormatting(rxBuffer + 4);
        Display_Printf(display, 0, 0, "Accel Z axis reading %f",angle);
    }
}
*/

double dataFormatting (int8_t* numb){
   double ivalue = (double)((numb[1] << 8) | (numb[0] & 0x00FF));
   return(ivalue);
   //return (acos((double)(ivalue/G_VALUE_2G_SCALE))*180/PI);
}

