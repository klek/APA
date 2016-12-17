#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.arm.elf.M4{1,0,15.12,3
#
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/CDIO_CC3200_pem4.oem4.dep
package/cfg/CDIO_CC3200_pem4.oem4.dep: ;
endif

package/cfg/CDIO_CC3200_pem4.oem4: | .interfaces
package/cfg/CDIO_CC3200_pem4.oem4: package/cfg/CDIO_CC3200_pem4.c package/cfg/CDIO_CC3200_pem4.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem4 $< ...
	$(ti.targets.arm.elf.M4.rootDir)/bin/armcl -c  -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/tidrivers_cc32xx_2_16_00_08/packages/ti/mw/" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/oslib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/netapps" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=__SL__ --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi   -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M4 --float_support=vfplib --abi=eabi -eo.oem4 -ea.sem4   -Dxdc_cfg__xheader__='"configPkg/package/cfg/CDIO_CC3200_pem4.h"'  -Dxdc_target_name__=M4 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_15_12_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4.rootDir)/include/rts -I$(ti.targets.arm.elf.M4.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem4 $< -C   -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/tidrivers_cc32xx_2_16_00_08/packages/ti/mw/" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/oslib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/netapps" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=__SL__ --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi   -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M4 --float_support=vfplib --abi=eabi -eo.oem4 -ea.sem4   -Dxdc_cfg__xheader__='"configPkg/package/cfg/CDIO_CC3200_pem4.h"'  -Dxdc_target_name__=M4 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_15_12_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4.rootDir)/include/rts -I$(ti.targets.arm.elf.M4.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/CDIO_CC3200_pem4.oem4: export C_DIR=
package/cfg/CDIO_CC3200_pem4.oem4: PATH:=$(ti.targets.arm.elf.M4.rootDir)/bin/;$(PATH)
package/cfg/CDIO_CC3200_pem4.oem4: Path:=$(ti.targets.arm.elf.M4.rootDir)/bin/;$(PATH)

package/cfg/CDIO_CC3200_pem4.sem4: | .interfaces
package/cfg/CDIO_CC3200_pem4.sem4: package/cfg/CDIO_CC3200_pem4.c package/cfg/CDIO_CC3200_pem4.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem4 -n $< ...
	$(ti.targets.arm.elf.M4.rootDir)/bin/armcl -c -n -s --symdebug:none -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/tidrivers_cc32xx_2_16_00_08/packages/ti/mw/" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/oslib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/netapps" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=__SL__ --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi   -qq -pdsw225 --endian=little -mv7M4 --float_support=vfplib --abi=eabi -eo.oem4 -ea.sem4   -Dxdc_cfg__xheader__='"configPkg/package/cfg/CDIO_CC3200_pem4.h"'  -Dxdc_target_name__=M4 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_15_12_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4.rootDir)/include/rts -I$(ti.targets.arm.elf.M4.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem4 $< -C  -n -s --symdebug:none -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/driverlib" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/tidrivers_cc32xx_2_16_00_08/packages/ti/mw/" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/oslib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/example/common" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/netapps" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0" --include_path="C:/ti/tirex-content/tirtos_cc32xx_2_16_00_08/products/CC3200_driverlib_1.1.0/inc" --include_path="C:/ti/ccsv62/ccsv6/tools/compiler/arm_15.12.3.LTS/include" -g --define=cc3200 --define=__SL__ --define=PART_CC3200 --define=ccs --define=CCWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi   -qq -pdsw225 --endian=little -mv7M4 --float_support=vfplib --abi=eabi -eo.oem4 -ea.sem4   -Dxdc_cfg__xheader__='"configPkg/package/cfg/CDIO_CC3200_pem4.h"'  -Dxdc_target_name__=M4 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_15_12_3 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4.rootDir)/include/rts -I$(ti.targets.arm.elf.M4.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/CDIO_CC3200_pem4.sem4: export C_DIR=
package/cfg/CDIO_CC3200_pem4.sem4: PATH:=$(ti.targets.arm.elf.M4.rootDir)/bin/;$(PATH)
package/cfg/CDIO_CC3200_pem4.sem4: Path:=$(ti.targets.arm.elf.M4.rootDir)/bin/;$(PATH)

clean,em4 ::
	-$(RM) package/cfg/CDIO_CC3200_pem4.oem4
	-$(RM) package/cfg/CDIO_CC3200_pem4.sem4

CDIO_CC3200.pem4: package/cfg/CDIO_CC3200_pem4.oem4 package/cfg/CDIO_CC3200_pem4.mak

clean::
	-$(RM) package/cfg/CDIO_CC3200_pem4.mak