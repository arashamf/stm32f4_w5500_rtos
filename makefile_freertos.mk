
# define freertos dir
FREERTOS_DIR = $(ROOT_DIR)/FreeRTOS-Kernel
# source director
FREERTOS_SRC_DIR     = $(FREERTOS_DIR)
FREERTOS_INC_DIR     = $(FREERTOS_DIR)/include
FREERTOS_ARM_CM4_DIR = $(FREERTOS_DIR)/portable/GCC/ARM_CM4F
FREERTOS_MemMang_DIR = $(FREERTOS_DIR)/portable/MemMang

# add freertos source
C_SOURCES  += $(FREERTOS_SRC_DIR)/croutine.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/event_groups.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/list.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/queue.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/stream_buffer.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/tasks.c
C_SOURCES  += $(FREERTOS_SRC_DIR)/timers.c
C_SOURCES  += $(FREERTOS_ARM_CM4_DIR)/port.c
C_SOURCES  += $(FREERTOS_MemMang_DIR)/heap_2.c

# include directories
C_INCLUDES += $(FREERTOS_INC_DIR)
C_INCLUDES += $(FREERTOS_ARM_CM4_DIR)


