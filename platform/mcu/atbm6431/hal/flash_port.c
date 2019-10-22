#include "atbm_hal.h"
#include "hal/soc/soc.h"
//#include "sys/flash.h"
//#include "osal.h"
//#include "wdt/drv_wdt.h"
#include "app_flash_param.h"
#include "errno.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

extern const hal_logic_partition_t hal_partitions[];
hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

static int hal_flash_param_program(uint32_t dst_addr, const void *data, uint32_t size)
{
    uint32_t remaining = size;
    uint32_t fl_addr; 
    uint32_t fl_offset;
    uint32_t len;
	uint32_t src_addr = (uint32_t) data;

	
    do
    {
        fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        fl_offset = dst_addr - fl_addr;
        len = MIN(FLASH_PAGE_SIZE - fl_offset, remaining);
        

        flash_page_program(dst_addr, (uint32_t *)src_addr, len);

		
        {
			dst_addr += len;
			src_addr += len;
			remaining -= len;
        }
    } while(remaining > 0);

    return 0;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
	uint32_t start_addr;
	hal_logic_partition_t *partition_info;
	hal_partition_t real_pno;
	real_pno = pno;

	partition_info = hal_flash_get_info( real_pno );
	start_addr = partition_info->partition_start_addr + *poff;
	if (buf_size == 0) {
		return 0;
	}
	if ((poff == NULL) || (buf == NULL) || ((*poff + buf_size) > partition_info->partition_length)) {
		printf("partition overwrite \r\n");
		// overwrite.
		return EIO;
	}
//	printf("hal_flash_param_write %x size %d,pno[%d]\n",start_addr, buf_size, pno);
	if (0 != hal_flash_param_program(start_addr, buf, buf_size)) {
		printf("FLASH_update failed!\n");
	}
	*poff += buf_size;
	return 0;
}



int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;
    real_pno = pno;

    partition_info = hal_flash_get_info( real_pno );

    if (buf_size == 0) {
        return 0;
    }
    if ((poff == NULL) || (buf == NULL) || ((*poff + buf_size) > partition_info->partition_length)) {
        return EIO;
    }
    start_addr = partition_info->partition_start_addr + *poff;
	//printf("hal_flash_read %x size %d,pno[%d]\n",start_addr,buf_size,pno);
	//dump_hex(poff,buf_size,16);
   // FLASH_read_at(start_addr, buf, buf_size);
   
//	memcpy(buf,(unsigned char*)poff,buf_size);
	memcpy(buf, (unsigned char*)start_addr, buf_size);
	
    *poff += buf_size;
	
	(void )start_addr;
    return 0;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
        uint32_t size)
{
    uint32_t start_addr;
    hal_logic_partition_t *partition_info;
    hal_partition_t real_pno;
    real_pno = pno;
	int i =0;

    partition_info = hal_flash_get_info( real_pno );
    if ((size + off_set) > partition_info->partition_length) {
        return EIO;
    }
//	printf("hal_flash_erase addr[%x]len[%d]\n",(size + off_set),size);

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), DEVICE_PAGE_SIZE);
//	printf("hal_flash_erase start_addr:%X\n", start_addr);
//	printf("flash_info.flash_offset_mask:%X\n", flash_info.flash_offset_mask);
	for(i=start_addr; i<start_addr + size; i+=DEVICE_SECTOR_SIZE)
	{
		flash_sector_erase(i&flash_info.flash_offset_mask);
	} 

   // FLASH_unlock_erase(start_addr, size);
    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int flash_sw_bank()
{
	return 1;
}



