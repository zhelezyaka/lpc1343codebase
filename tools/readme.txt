This folder contains a number of tools that may be useful when developing with
the LPC1343 Reference Board

imgconv - This Windows-based application converts PNG, JPG or BMP images to an
          RGB565 binary format that is understood by the LPC1343 Code Base.
          Using this utility, you can create bitmap images of different sizes
          and load them directly from the SD card reader.
          
          The generated binary images have the following format:
          
          U16 width, U16 height, U16 bit depth (always '1' at present),
          followed by one 16-bit field for every RGB565 pixel until EOF.  All
          pixels are arranged left to right, top to bottom.
          
          The source for and project files for Visual Studio 2010 are
          included in /src

lpcrc   - This utility fixes the CRC of any .bin files generated with GCC
          from the command line.  You must use this utility to patch the
          any compiled firmware that will be deployed with the USB bootloader.

          The GCC src is included in the root folder and should build on any
          platform where a native GCC toolchain is available.  A pre-compiled
          windows binary is included in /bin for convenience sake, as well as
          in the root folder of the LPC1343 Code Base.
          