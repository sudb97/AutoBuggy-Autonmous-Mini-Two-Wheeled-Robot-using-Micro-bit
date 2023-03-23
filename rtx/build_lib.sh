# Path to tool-chain
ARMGCC=/opt/arm-gcc

# Path to BSP library
BSPDIR=.

# Tool-chain programs
CC=$ARMGCC/bin/arm-none-eabi-gcc
LD=$ARMGCC/bin/arm-none-eabi-ld
AR=$ARMGCC/bin/arm-none-eabi-ar
SIZE=$ARMGCC/bin/arm-none-eabi-size
OBJCOPY=$ARMGCC/bin/arm-none-eabi-objcopy

# ARM architecture specific compiler flags
ARCH="-mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16"

# Keep every function in a separate section, this allows linker to discard unused ones
OPT="-ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin"

# Compiler flags
CFLAGS="-O3 -g3 -Wall -Werror $ARCH $OPT"

# Compile the source files
echo Compiling all .c
$CC -c $CFLAGS *.c

$CC -c $CFLAGS irq_armv7m.S

# Link the object code to form exectuable program
echo Generating librtx.a
/bin/rm -f librtx.a
$AR -r librtx.a *.o irq_armv7m.S
/bin/rm -f *.o 
