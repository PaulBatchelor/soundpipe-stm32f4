SRCS= main.c stm32f4xx_it.c system_stm32f4xx.c waveplayer.c 

# Newlib files are located. 
LIBCPATH=~/playground/stm32/arm-toolchain/arm-none-eabi/newlib/include
LIBCLIB=~/playground/stm32/arm-toolchain/arm-none-eabi/newlib/lib

PROJ_NAME=fm

# Where the STM firmware files are located
STM_COMMON=..

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g -Os -Wall -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -I.

CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc
CFLAGS += -DUSE_STDPERIPH_DRIVER -DUSE_DEFAULT_TIMEOUT_CALLBACK

CFLAGS += -I../inc -DMEDIA_IntFLASH
CFLAGS += -I$(STM_COMMON)/Utilities/Third_Party/fat_fs/inc
CFLAGS += -I$(STM_COMMON)/Libraries/STM32_USB_HOST_Library/Class/MSC/inc/
CFLAGS += -I$(STM_COMMON)/Libraries/STM32_USB_HOST_Library/Core/inc/
CFLAGS += -I$(STM_COMMON)/Libraries/STM32_USB_OTG_Driver/inc/
CFLAGS += #-DUSE_USB_OTG_FS #-DMEDIA_USB_KEY
CFLAGS += --specs=nosys.specs 
CFLAGS += -DNO_LIBSNDFILE 


STM_START = $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s 
SRCS += $(STM_COMMON)/Utilities/STM32F4-Discovery/stm32f4_discovery_audio_codec.c
SRCS += $(STM_COMMON)/Utilities/STM32F4-Discovery/stm32f4_discovery.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
OBJS = $(SRCS:.c=.o)

.PHONY: proj

all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS)
	$(CC) -L$(LIBCLIB) -L. $(CFLAGS) $(OBJS) $(STM_START) -lsoundpipe -lm -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

%.o : %.c
	$(CC) -c $(CFLAGS) $^ -o $@ 

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin
	rm -f $(OBJS)

# Flash the STM32F4
burn: proj
	st-flash write $(PROJ_NAME).bin 0x8000000
