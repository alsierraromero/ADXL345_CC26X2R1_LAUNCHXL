## Example Summary

Sample application that calculates angle against the vertical using ADXL345 accelerometer sensor.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_GPIO_LED_0` - Indicator LED
* `I2C_ACCEL`          - I2C bus used to communicate with the sensor.

## CCS, Board Resources 

- DK: CC26X2R1_LAUNCHXL
- TIRTOS7 
- Compiler: ticlang
- In the main_tirtos.c file the main function is located. Inside the main function the mainThread is created using POSIX's pthread_create(). The tasks need to be created inside the mainThread using regular TI Taks_construct() or Task_create()
- Display related functions need to be used in the mainThread () function, otherwise the program won't be able to run properly 

## Example Usage

* Example output is generated through use of Display driver APIs. Refer to the
Display driver documentation found in the SimpleLink MCU SDK User's Guide.

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
 Homepage"), etc.) to the appropriate COM port.
 * The COM port can be determined via Device Manager in Windows or via
 `ls /dev/tty*` in Linux.

The connection will have the following settings:
```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```

