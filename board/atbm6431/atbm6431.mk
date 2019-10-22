NAME := board_atbm6431

$(NAME)_TYPE := kernel
MODULE             := 1062
HOST_ARCH          := ANDES_N9
HOST_MCU_FAMILY    := atbm6431
SUPPORT_BINS         := no


# todo: remove these after rhino/lwip ready

define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef
CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ATBM6431
CONFIG_SYSINFO_DEVICE_NAME   := ATBM6431

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

#for activation handle
GLOBAL_CFLAGS += -DBOARD_ATBM6431 -DALI_ALGO_AES_ROM_TABLES

# OTA Board config
# 0:OTA_RECOVERY_TYPE_DIRECT 1:OTA_RECOVERY_TYPE_ABBACK 2:OTA_RECOVERY_TYPE_ABBOOT
#GLOBAL_CFLAGS += -DAOS_OTA_RECOVERY_TYPE=1
#GLOBAL_CFLAGS += -DAOS_OTA_BANK_DUAL 
################################################################
# kv
################################################################
GLOBAL_DEFINES += KV_CONFIG_PARTITION=HAL_PARTITION_PARAMETER_3
GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=8192
GLOBAL_DEFINES += DEBUG_CONFIG_ERRDUMP=0



GLOBAL_INCLUDES += . \
                   config/ \
                   startup

$(NAME)_SOURCES += config/partition_conf.c \
                   startup/board.c \
                   startup/startup.c\
                   startup/atbm_hal.c\
                   startup/start.S

EXTRA_TARGET_MAKEFILES += $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

GLOBAL_DEFINES += LOCAL_PORT_ENHANCED_RAND WITH_VENDOR_LWIP
