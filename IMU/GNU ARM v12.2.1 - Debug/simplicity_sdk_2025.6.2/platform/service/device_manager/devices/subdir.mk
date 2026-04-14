################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.c 

OBJS += \
./simplicity_sdk_2025.6.2/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.o 

C_DEPS += \
./simplicity_sdk_2025.6.2/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.d 


# Each subdirectory must supply rules for building sources it contributes
simplicity_sdk_2025.6.2/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.o: /Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.c simplicity_sdk_2025.6.2/platform/service/device_manager/devices/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c18 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32MG24B310F1536IM48=1' '-DSL_CODE_COMPONENT_SYSTEM=system' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DHFXO_FREQ=39000000' '-DSL_BOARD_NAME="BRD2601B"' '-DSL_BOARD_REV="A01"' '-DSL_CODE_COMPONENT_CLOCK_MANAGER=clock_manager' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DSL_CODE_COMPONENT_DEVICE_PERIPHERAL=device_peripheral' '-DSL_CODE_COMPONENT_GPIO=gpio' '-DSL_CODE_COMPONENT_HAL_COMMON=hal_common' '-DSL_CODE_COMPONENT_HAL_GPIO=hal_gpio' '-DSL_CODE_COMPONENT_HAL_SYSRTC=hal_sysrtc' '-DSL_CODE_COMPONENT_INTERRUPT_MANAGER=interrupt_manager' '-DCMSIS_NVIC_VIRTUAL=1' '-DCMSIS_NVIC_VIRTUAL_HEADER_FILE="cmsis_nvic_virtual.h"' '-DSL_CODE_COMPONENT_CORE=core' '-DSL_CODE_COMPONENT_SLEEPTIMER=sleeptimer' -I"/Users/zacharythornton/SimplicityStudio/v5_workspace/IMU/config" -I"/Users/zacharythornton/SimplicityStudio/v5_workspace/IMU/autogen" -I"/Users/zacharythornton/SimplicityStudio/v5_workspace/IMU" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/Device/SiliconLabs/EFR32MG24/Include" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/common/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//hardware/board/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/clock_manager/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/clock_manager/src" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/CMSIS/Core/Include" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/device_manager/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/device_init/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/emlib/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/driver/gpio/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/peripheral/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/interrupt_manager/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/interrupt_manager/src" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/interrupt_manager/inc/arm" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/memory_manager/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_eusart" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/sl_main/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/sl_main/src" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/sleeptimer/inc" -I"/Users/zacharythornton/SimplicityStudio/SDKs/simplicity_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mcmse -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-lto --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"simplicity_sdk_2025.6.2/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


