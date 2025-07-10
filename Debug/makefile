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
HardwareTest: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	aarch64-poky-linux-gcc --sysroot=/opt/poky/3.1.33/sysroots/aarch64-poky-linux -o "HardwareTest" $(OBJS) $(USER_OBJS) $(LIBS) -lpthread -lasound
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) HardwareTest
	-@echo ' '

post-build:
	-@echo 'Copy To Target'
	-scp -o HostKeyAlgorithms=ssh-rsa HardwareTest root@192.168.44.202:/home/root
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
