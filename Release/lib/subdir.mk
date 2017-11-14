################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/signal.cpp 

OBJS += \
./lib/signal.o 

CPP_DEPS += \
./lib/signal.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o: ../lib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++14 -I/home/jouven/mylibs/include -O2 -c -fmessage-length=0 -mtune=sandybridge -flto=jobserver -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


