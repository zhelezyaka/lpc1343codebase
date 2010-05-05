If you are using Windows 7 as a build environment, you will need to make a small modification to the make file in order to be able to compile.  Unfortunately, the 'ECHO' command in Windows 7 includes the quotations marks in it's output, such that

@echo "test"

Will output:

"test"

and not:

test

as it would on Linux or most other operating systems.  This introduces a number of problems and compatability issues between Windows 7 and Linux-based operating systems.  To build under Windows 7, lines 83-86 in the make file will need to be changed from:

-@echo "MEMORY {"\
       "  flash(rx): ORIGIN = 0x00000000, LENGTH = $(FLASH)"\
       "  sram(rwx): ORIGIN = 0x10000000+$(SRAM_USB), LENGTH = $(SRAM)-$(SRAM_USB) }"\
       "INCLUDE $(LD_SCRIPT)" > $(LD_TEMP)

to the following, all on one line:

-@echo MEMORY {flash(rx): ORIGIN = 0x00000000, LENGTH = $(FLASH) sram(rwx): ORIGIN = 0x10000000+$(SRAM_USB), LENGTH = $(SRAM)-$(SRAM_USB) } INCLUDE $(LD_SCRIPT) > $(LD_TEMP)

