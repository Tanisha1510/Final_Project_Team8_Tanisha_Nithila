################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/EEPROM.c \
../src/Nvm.c \
../src/Nvm_Test.c \
../src/OLED_Display.c \
../src/Oled_asci.c \
../src/main.c 

OBJS += \
./src/EEPROM.o \
./src/Nvm.o \
./src/Nvm_Test.o \
./src/OLED_Display.o \
./src/Oled_asci.o \
./src/main.o 

C_DEPS += \
./src/EEPROM.d \
./src/Nvm.d \
./src/Nvm_Test.d \
./src/OLED_Display.d \
./src/Oled_asci.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/EEPROM.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


