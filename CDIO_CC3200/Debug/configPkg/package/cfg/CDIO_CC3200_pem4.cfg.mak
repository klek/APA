# invoke SourceDir generated makefile for CDIO_CC3200.pem4
CDIO_CC3200.pem4: .libraries,CDIO_CC3200.pem4
.libraries,CDIO_CC3200.pem4: package/cfg/CDIO_CC3200_pem4.xdl
	$(MAKE) -f C:\Users\Gabriel\workspace_v6_2\CDIO_CC3200/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Gabriel\workspace_v6_2\CDIO_CC3200/src/makefile.libs clean

