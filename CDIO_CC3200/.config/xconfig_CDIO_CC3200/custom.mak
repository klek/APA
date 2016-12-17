## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4 linker.cmd package/cfg/CDIO_CC3200_pem4.oem4

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/CDIO_CC3200_pem4.xdl
	$(SED) 's"^\"\(package/cfg/CDIO_CC3200_pem4cfg.cmd\)\"$""\"C:/Users/Gabriel/Documents/GitHub/APA/CDIO_CC3200/.config/xconfig_CDIO_CC3200/\1\""' package/cfg/CDIO_CC3200_pem4.xdl > $@
	-$(SETDATE) -r:max package/cfg/CDIO_CC3200_pem4.h compiler.opt compiler.opt.defs
