/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "hal/soc/soc.h"
 
#include "aos/kernel.h"
#include "atbm_hal.h"
#include "app_flash_param.h"

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX];

void flash_partition_init(void)
{
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_owner       = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_description = "PARAM1";
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_start_addr  = FLASH_USR_DATA_SECTION_ADDR;
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_length      = FLASH_USR_DATA_SECTION_LEN; /* 8k bytes */
    hal_partitions[HAL_PARTITION_PARAMETER_1].partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_APPLICATION].partition_owner       = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_description = "Application";
    hal_partitions[HAL_PARTITION_APPLICATION].partition_start_addr  = FLASH_BOOT_PARAM_SECTION2_ADDR;
    hal_partitions[HAL_PARTITION_APPLICATION].partition_length      = FLASH_CODE1_SECTION_LEN+FLASH_CODE2_SECTION_LEN; /* 8k bytes */
    hal_partitions[HAL_PARTITION_APPLICATION].partition_options     = PAR_OPT_READ_EN ;

	

    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_owner          = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_description    = "OTA Storage";
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_start_addr     = FLASH_CODE1_UPDATA_SECTION_ADDR;
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_length         = FLASH_CODE1_SECTION_LEN+FLASH_CODE2_SECTION_LEN; /* 1000k bytes */
    hal_partitions[HAL_PARTITION_OTA_TEMP].partition_options        = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;

    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_owner       = HAL_FLASH_EMBEDDED;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_description = "PARAM2";
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_start_addr  = FLASH_USR_DATA_SECTION2_ADDR;
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_length      = FLASH_USR_DATA_SECTION2_LEN; /* 8k bytes */
    hal_partitions[HAL_PARTITION_PARAMETER_2].partition_options     = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
}

