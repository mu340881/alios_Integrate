#include "atbm_type.h"
#include "reloc_defs.h"


#if (CHIP_TYPE==CHIP_ATHENAB)
#define FLASH_BASE				0x12000000
#else //#if (CHIP_TYPE==CHIP_ATHENAB)
#define FLASH_BASE		 		0x400000
#endif  //#if (CHIP_TYPE==CHIP_ATHENAB)


#define DEVICE_SIZE				0x1000000u
#define DEVICE_BLOCK_SIZE		0x008000u
#define DEVICE_SECTOR_SIZE		0x001000u
#define DEVICE_PAGE_SIZE		0x000100u