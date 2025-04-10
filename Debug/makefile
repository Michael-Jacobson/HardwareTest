################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := HardwareTest
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: HardwareTest

# Tool invocations
HardwareTest: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS) /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libadt_g729ab_cortexa55_v3_06_06.a /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libaecg4_adt_cortexa55_v6_01_07b.a /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libaes_adt_cortexa55_v4_00_02.a /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libagc_adt_cortexa55_v3_09_00.a /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libg168_adt_cortexa55_v11_19_11.a /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libnr2_adt_cortexa55_v3_04_02.a /home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libvadcng_adt_cortexa55_v5_04_01.a
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	aarch64-poky-linux-gcc -L"/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib" --sysroot=/opt/poky/3.1.31/sysroots/aarch64-poky-linux -o "HardwareTest" $(OBJS) $(USER_OBJS) $(LIBS) "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libadt_g729ab_cortexa55_v3_06_06.a" "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libaecg4_adt_cortexa55_v6_01_07b.a" "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libaes_adt_cortexa55_v4_00_02.a" "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libagc_adt_cortexa55_v3_09_00.a" "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libg168_adt_cortexa55_v11_19_11.a" "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libnr2_adt_cortexa55_v3_04_02.a" "/home/michaeljacobson/eclipse-workspace/HardwareTest/Lib/libvadcng_adt_cortexa55_v5_04_01.a" -lpthread -lm -lasound
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) HardwareTest
	-@echo ' '

post-build:
	-@echo 'Copy To Target'
	-scp -o HostKeyAlgorithms=ssh-rsa HardwareTest root@192.168.44.164:/home/root
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
