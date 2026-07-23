################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../empty.syscfg 

C_SRCS += \
../Delay.c \
../NRF24L01.c \
../OLED.c \
../OLED_Data.c \
./ti_msp_dl_config.c \
C:/TI/mspm0_sdk_2_11_00_07/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g351x_ticlang.c \
../main.c 

GEN_CMDS += \
./device_linker.cmd 

GEN_FILES += \
./device_linker.cmd \
./device.opt \
./ti_msp_dl_config.c 

C_DEPS += \
./Delay.d \
./NRF24L01.d \
./OLED.d \
./OLED_Data.d \
./ti_msp_dl_config.d \
./startup_mspm0g351x_ticlang.d \
./main.d 

GEN_OPTS += \
./device.opt 

OBJS += \
./Delay.o \
./NRF24L01.o \
./OLED.o \
./OLED_Data.o \
./ti_msp_dl_config.o \
./startup_mspm0g351x_ticlang.o \
./main.o 

GEN_MISC_FILES += \
./device.cmd.genlibs \
./ti_msp_dl_config.h \
./Event.dot 

OBJS__QUOTED += \
"Delay.o" \
"NRF24L01.o" \
"OLED.o" \
"OLED_Data.o" \
"ti_msp_dl_config.o" \
"startup_mspm0g351x_ticlang.o" \
"main.o" 

GEN_MISC_FILES__QUOTED += \
"device.cmd.genlibs" \
"ti_msp_dl_config.h" \
"Event.dot" 

C_DEPS__QUOTED += \
"Delay.d" \
"NRF24L01.d" \
"OLED.d" \
"OLED_Data.d" \
"ti_msp_dl_config.d" \
"startup_mspm0g351x_ticlang.d" \
"main.d" 

GEN_FILES__QUOTED += \
"device_linker.cmd" \
"device.opt" \
"ti_msp_dl_config.c" 

C_SRCS__QUOTED += \
"../Delay.c" \
"../NRF24L01.c" \
"../OLED.c" \
"../OLED_Data.c" \
"./ti_msp_dl_config.c" \
"C:/TI/mspm0_sdk_2_11_00_07/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g351x_ticlang.c" \
"../main.c" 

SYSCFG_SRCS__QUOTED += \
"../empty.syscfg" 


