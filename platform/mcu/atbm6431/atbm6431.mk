NAME := atbm6431

HOST_OPENOCD := atbm6431

$(NAME)_TYPE := kernel 

$(NAME)_COMPONENTS := framework.common yloop modules.fs.kv libc
$(NAME)_COMPONENTS += protocols.net alicrypto hal


use_private_lwip := 1

ATBM_INC_PATH    := bsp/include
GLOBAL_INCLUDES += $(ATBM_INC_PATH)
GLOBAL_INCLUDES += bsp/src/at
GLOBAL_INCLUDES += hal bsp
GLOBAL_INCLUDES += $(ATBM_INC_PATH)/rtos-freertos/hal/
GLOBAL_INCLUDES += $(ATBM_INC_PATH)/rtos-comm/driver/wifi/mod_platform/export/
GLOBAL_INCLUDES += $(ATBM_INC_PATH)/rtos-comm/include/
GLOBAL_INCLUDES += $(ATBM_INC_PATH)/rtos-comm/include/sys/
GLOBAL_INCLUDES += bsp/lwip/include
GLOBAL_INCLUDES += bsp/lwip/include/lwip
GLOBAL_INCLUDES += bsp/lwip/port/include
GLOBAL_INCLUDES += bsp/src/driver
GLOBAL_CFLAGS    += -fno-builtin -nostartfiles -static -Wl,--gc-sections  -ffunction-sections -fdata-sections -funroll-loops -nostdlib \
										 -Wl,--start-group -lgloss -lgcc -lc -lstdc++ -lm -nostdlib -nostartfiles  \
										-fno-builtin -fomit-frame-pointer -funroll-loops  -nostdlib  \
										-fno-strict-aliasing  -DUSE_HI_INTERNAL=1 -DIOT_PLATFORM  -Wl,--gc-sections   -fdata-sections  \
										-DCHIP_ATHENAB=1 -DCHIP_ARES=2 -DCHIP_ARES_B=3 -DCHIP_TYPE=3 \
										-Wall -DNO_GETENV -DFPM_DEFAULT -DHAVE_MALLOC   \
										-DLOAD_MODE=1 -D__ATBM_6421__ -DCONFIG_NO_NDS32_EXT_EX9 -Wl,--mno-ex9  -DANDES -DCFG_AE210P -DCFG_MAKEFILE  \
										-Wall -fno-builtin -fomit-frame-pointer  -fno-strict-aliasing -ffunction-sections   -Wl,--gc-sections   -fdata-sections   \
										-DCACHE_AHB=1 -DCACHE_64K=1  -DFLASH_USE_2MBYTE_SIZE -mcmodel=large 	-DCONFIG_MORE_MEMORY=1	-std=gnu99  -fgnu89-inline -Wno-missing-braces -Wno-unused-but-set-variable -Wno-unused-variable

										
GLOBAL_ASMFLAGS    +=  -Wall  -fno-builtin -nostartfiles -static -Wl,--gc-sections  -ffunction-sections -fdata-sections -funroll-loops -nostdlib \
										-Wl,--start-group -lgloss -lgcc -lc -lstdc++ -lm -nostdlib -nostartfiles   \
										-DUSE_HI_INTERNAL=1 -DIOT_PLATFORM 	-DCHIP_ATHENAB=1 -DCHIP_ARES=2 -DCHIP_ARES_B=3 -DCHIP_TYPE=3 \
										-Wall -DNO_GETENV -DFPM_DEFAULT -DHAVE_MALLOC   \
										-DLOAD_MODE=1 -D__ATBM_6421__ -DCONFIG_NO_NDS32_EXT_EX9 -Wl,--mno-ex9  -DANDES -DCFG_AE210P -DCFG_MAKEFILE  \
										-Wall -fno-builtin -fomit-frame-pointer  -fno-strict-aliasing -ffunction-sections   -Wl,--gc-sections   -fdata-sections  \
										-DCACHE_AHB=1 -DCACHE_64K=1 -DFLASH_USE_2MBYTE_SIZE 	-mcmodel=large  
										
GLOBAL_LDFLAGS   += -Wl,--gc-sections,--undefine=__rtos_signature_freertos_v8_0_0  -ffunction-sections -fdata-sections  -DCACHE_64K=1 \
										./platform/mcu/atbm6431/bsp/lib/libcom.a \
										-Xlinker "-("  \
										./platform/mcu/atbm6431/bsp/lib/libapi.a ./platform/mcu/atbm6431/bsp/lib/libapp.a \
										./platform/mcu/atbm6431/bsp/lib/libwifi.a \
										./platform/mcu/atbm6431/bsp/lib/libdev.a \
										./platform/mcu/atbm6431/bsp/lib/libbsp.a\
										./platform/mcu/atbm6431/bsp/lib/liblwipapps.a 	./platform/mcu/atbm6431/bsp/lib/libuartcli.a\
										./platform/mcu/atbm6431/bsp/lib/libatcmdcom.a \
										 -Xlinker "-)"   -fno-builtin -nostartfiles  -Wl,--mno-ex9

GLOBAL_LDS_FILES += platform/mcu/atbm6431/bsp/ld/altobeam_load_aresB_cache_ahb_64K_start_itcm32K.ld.S
#GLOBAL_LDFLAGS   += -Lplatform/mcu/atbm6431/bsp/ld

GLOBAL_DEFINES += CONFIG_ATBM_LWIP COAP_WITH_YLOOP  AOS_COMP_CLI  CONFIG_AOS_KV_BUFFER_SIZE=8192 CONFIG_PRINT_HEAP 
GLOBAL_DEFINES += MBEDTLS_AES_ROM_TABLES
GLOBAL_DEFINES += ATBM6431
GLOBAL_DEFINES +=  MBEDTLS_AES_SETKEY_ENC_ALT MBEDTLS_AES_SETKEY_DEC_ALT MBEDTLS_AES_ENCRYPT_ALT MBEDTLS_AES_DECRYPT_ALT
#memory
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
GLOBAL_DEFINES += AWSS_DISABLE_ENROLLEE
GLOBAL_DEFINES += COAP_OBSERVE_CLIENT_DISABLE

#
#$(NAME)_PREBUILT_LIBRARY := bsp/lib/libapi.a
#$(NAME)_PREBUILT_LIBRARY += bsp/lib/libapp.a
#$(NAME)_PREBUILT_LIBRARY += bsp/lib/libwifi.a
#$(NAME)_PREBUILT_LIBRARY += bsp/lib/libatcmd.a
#$(NAME)_PREBUILT_LIBRARY += bsp/lib/libcom.a
#$(NAME)_PREBUILT_LIBRARY += bsp/lib/libdev.a
#$(NAME)_PREBUILT_LIBRARY += bsp/lib/libbsp.a


$(NAME)_SOURCES += aos/hook_impl.c
$(NAME)_SOURCES += aos/soc_impl.c
$(NAME)_SOURCES += aos/syscall.c
$(NAME)_SOURCES += hal/uart.c
$(NAME)_SOURCES += hal/flash_port.c
$(NAME)_SOURCES += hal/wifi_port.c
$(NAME)_SOURCES += hal/cipher_port.c

ifeq (y,$(FEATURE_OTA_ENABLED))
$(NAME)_SOURCES += hal/ota_port.c
endif

#$(NAME)_CFLAGS := -std=gnu99

ifneq ($(wifi),0)
$(NAME)_CFLAGS += -DENABLE_WIFI
endif

$(NAME)_CFLAGS += -Iplatform/mcu/atbm6431/bsp/include
$(NAME)_CFLAGS += -Iplatform/mcu/atbm6431/bsp/include/rtos-comm/include
$(NAME)_CFLAGS += -Iplatform/mcu/atbm6431/bsp/src/at
$(NAME)_CFLAGS += -Iplatform/mcu/atbm6431/bsp/include/rtos-freertos/hal/

$(NAME)_COMPONENTS       += rhino platform/arch/andes/N9


include platform/mcu/atbm6431/bsp/lwip/aos.mk
#include platform/mcu/atbm6431/bsp/lwip_aos_2.0.1/aos.mk

