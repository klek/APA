################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CC3200_LAUNCHXL.obj: ../CC3200_LAUNCHXL.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="CC3200_LAUNCHXL.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

CDIO_CC3200.obj: ../CDIO_CC3200.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="CDIO_CC3200.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

build-1658382191: ../CDIO_CC3200.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/tirex-content/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/packages;C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/tidrivers_cc32xx_2_16_00_08/packages;C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/bios_6_45_01_29/packages;C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/ns_1_11_00_10/packages;C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/uia_2_00_05_50/packages;C:/ti/ccsv62/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4 -p ti.platforms.simplelink:CC3200 -r release -c "C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=vfplib -me --include_path=\"C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200\" --include_path=\"C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0\" --include_path=\"C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc\" --include_path=\"C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib\" --include_path=\"C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include\" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-1658382191
configPkg/compiler.opt: build-1658382191
configPkg/: build-1658382191

gpio_if.obj: ../gpio_if.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="gpio_if.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

rom_pin_mux_config.obj: ../rom_pin_mux_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="rom_pin_mux_config.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


