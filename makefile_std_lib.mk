
# define lib dir
STM32F4_LIB_DIR  	= 	$(ROOT_DIR)/Drivers
STM32F4_STD_PATH  	= 	$(STM32F4_LIB_DIR)/STM32F4xx_HAL_Driver
STM32F4_CMSIS_PATH 	=  	$(STM32F4_LIB_DIR)/CMSIS

# source director
STM32F4_SRC_STDLIB     			= $(STM32F4_STD_PATH)/Src

# include director
STM32F4_INC_STDLIB     			= $(STM32F4_STD_PATH)/Inc
STM32F4_INC_STDLIB_LEGACY     	= $(STM32F4_STD_PATH)/Inc/Legacy
STM32F4_CORE_INC_DIR    		= $(STM32F4_CMSIS_PATH)/Include
STM32F4_DEVICE_INC_DIR  		= $(STM32F4_CMSIS_PATH)/Device/ST/STM32F4xx/Include

# startup
ASM_SOURCES  += $(STM32F4_LIB_DIR)/startup_stm32f407xx.s

# CMSIS
#C_SOURCES  += $(STM32F4_CORE_DIR)/sysmem.c
#C_SOURCES  += $(STM32F4_SRC_DEVICE_DIR)/syscalls.c
#C_SOURCES  += $(STM32F4_SRC_DEVICE_DIR)/system_stm32f4xx.c

# use libraries, please add or remove when you use or remove it.
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_tim.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_tim_ex.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_rcc.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_rcc_ex.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_flash.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_flash_ex.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_flash_ramfunc.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_gpio.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_dma_ex.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_dma.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_pwr.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_pwr_ex.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_cortex.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_exti.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_hal_spi.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_ll_rcc.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_ll_utils.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_ll_exti.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_ll_usart.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_ll_gpio.c 
C_SOURCES  += $(STM32F4_SRC_STDLIB)/stm32f4xx_ll_dma.c

# include directories
C_INCLUDES += $(STM32F4_INC_STDLIB)
C_INCLUDES += $(STM32F4_INC_STDLIB_LEGACY)
C_INCLUDES += $(STM32F4_CORE_INC_DIR)
C_INCLUDES += $(STM32F4_DEVICE_INC_DIR)

