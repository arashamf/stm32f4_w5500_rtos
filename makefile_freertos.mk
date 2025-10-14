
# define freertos dir
FREERTOS_DIR = $(ROOT_DIR)/Middlewares/Third_Party/FreeRTOS/Source

# source director
FREERTOS_SRC_DIR     = $(FREERTOS_DIR)
FREERTOS_ARM_CM4_DIR = $(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM4F
FREERTOS_MemMang_DIR = $(FREERTOS_SRC_DIR)/portable/MemMang

FREERTOS_INC_DIR     		= $(FREERTOS_DIR)/include
FREERTOS_ARM_CM4_INC_DIR 	= $(FREERTOS_ARM_CM4_DIR)

# add freertos source
C_SOURCES  += $(FREERTOS_SRC_DIR)/freertos.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/croutine.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/event_groups.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/list.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/queue.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/stream_buffer.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/tasks.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/timers.c
C_SOURCES  += $(FREERTOS_MemMang_DIR)/heap_2.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM4F/port.c
#C_SOURCES  += $(FREERTOS_SRC_DIR)/CMSIS_RTOS/cmsis_os.c 

# include directories
C_INCLUDES += $(FREERTOS_INC_DIR)
C_INCLUDES += $(FREERTOS_ARM_CM4_INC_DIR)
#C_INCLUDES += $(FREERTOS_DIR)/CMSIS_RTOS

