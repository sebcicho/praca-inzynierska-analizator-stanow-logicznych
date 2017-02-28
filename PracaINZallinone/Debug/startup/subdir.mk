################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../startup/startup_stm32f429_439xx.S 

OBJS += \
./startup/startup_stm32f429_439xx.o 

S_UPPER_DEPS += \
./startup/startup_stm32f429_439xx.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F429ZITx -DSTM32F429I_DISCO -DSTM32F4 -DSTM32 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F429_439xx -I"C:/Users/Seba/Desktop/inz/stm32f429i-disco_stdperiph_lib" -I"C:/Users/Seba/Desktop/inz/PracaINZallinone/inc" -I"C:/Users/Seba/Desktop/inz/stm32f429i-disco_stdperiph_lib/CMSIS/core" -I"C:/Users/Seba/Desktop/inz/stm32f429i-disco_stdperiph_lib/CMSIS/device" -I"C:/Users/Seba/Desktop/inz/stm32f429i-disco_stdperiph_lib/StdPeriph_Driver/inc" -I"C:/Users/Seba/Desktop/inz/stm32f429i-disco_stdperiph_lib/Utilities/Common" -I"C:/Users/Seba/Desktop/inz/stm32f429i-disco_stdperiph_lib/Utilities/STM32F429I-Discovery" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


