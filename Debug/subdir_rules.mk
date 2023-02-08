################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1210/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang" -I"C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/source/ti/posix/ticlang" -DDeviceFamily_CC26X2 -gdwarf-3 -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1282012831: ../i2ctmp.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1210/ccs/utils/sysconfig_1.14.0/sysconfig_cli.bat" -s "C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/.metadata/product.json" --script "C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang/i2ctmp.syscfg" -o "syscfg" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-1282012831 ../i2ctmp.syscfg
syscfg/ti_drivers_config.c: build-1282012831
syscfg/ti_drivers_config.h: build-1282012831
syscfg/ti_utils_build_linker.cmd.genlibs: build-1282012831
syscfg/syscfg_c.rov.xs: build-1282012831
syscfg/ti_utils_runtime_model.gv: build-1282012831
syscfg/ti_utils_runtime_Makefile: build-1282012831
syscfg/ti_sysbios_config.h: build-1282012831
syscfg/ti_sysbios_config.c: build-1282012831
syscfg/: build-1282012831

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1210/ccs/tools/compiler/ti-cgt-armllvm_2.1.2.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang" -I"C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_01_03/source/ti/posix/ticlang" -DDeviceFamily_CC26X2 -gdwarf-3 -march=armv7e-m -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/AlbertoSierraRomero/workspace_v12/i2ctmp_CC26X2R1_LAUNCHXL_tirtos7_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


