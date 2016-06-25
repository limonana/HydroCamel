################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CamerasController.cpp \
../FrontCamera.cpp 

OBJS += \
./CamerasController.o \
./FrontCamera.o 

CPP_DEPS += \
./CamerasController.d \
./FrontCamera.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/opencv -I/usr/local/VRmagic/include -I/usr/local/include/vm_lib -O0 -g3 -Wall -c -fmessage-length=0 -pipe -O2 -fmessage-length=0 -fPIC -Wno-deprecated -Wall -W -Wno-unused -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


