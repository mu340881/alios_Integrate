

#ifndef _IOT_FLASH_DEF_H_
#define _IOT_FLASH_DEF_H_


#ifdef __cplusplus
extern "C" {
#endif

/**
 * flash program(write) data mode
 */
enum iot_flash_write_mode {
    IOT_WRITE_MODE_PAGE_256B = 1 << 0,                           	   /**< write 1 to 256 bytes per page */
    IOT_WRITE_MODE_BYTE 	= 1 << 1,                                  /**< byte write */
    IOT_WRITE_MODE_AAI 		= 1 << 2,                                  /**< auto address increment */
    IOT_WRITE_MODE_DUAL_BUFFER = 1 << 3,                         	   /**< dual-buffer write, like AT45DB series */
};

/* manufacturer information */
typedef struct {
    char *name;
    uint8_t id;
} iot_flash_mf;

#define IOT_FLASH_SIZE_UTIL (512*1024)
#define IOT_FLASH_ERASE_UTIL (1*1024)
#define IOT_SUPPORT_FLASH_NUM 16
/* flash chip information */
struct iot_flash_flash_chip{
    char *name;                                  /**< flash chip name */
    uint32_t size;                           	 /**< flash size (512Kbytes) */
	
    uint8_t mf_id;                               /**< manufacturer ID */
    uint8_t type_id;                             /**< memory type ID */
    uint8_t capacity_id;                         /**< capacity ID */
    uint8_t write_mode;                          /**< write mode  @see iot_flash_write_mode */

    uint8_t erase_gran;                          /**< erase granularity  (Kbytes)  */		
    uint8_t erase_gran_cmd;                      /**< erase granularity size block command */
	uint8_t read_cmd;							 /**< read cmd */
	
	uint8_t status_cmd;							 /**< status_cmd */
	
};

/* IOT support manufacturer JEDEC ID */
#define IOT_MANUFACTURER_ID_CYPRESS                             0x01
#define IOT_MANUFACTURER_ID_FUJITSU                             0x04
#define IOT_MANUFACTURER_ID_XTX                             	0x0B
#define IOT_MANUFACTURER_ID_EON                                 0x1C
#define IOT_MANUFACTURER_ID_ATMEL                               0x1F
#define IOT_MANUFACTURER_ID_MICRON                              0x20
#define IOT_MANUFACTURER_ID_AMIC                                0x37
#define IOT_MANUFACTURER_ID_SANYO                               0x62
#define IOT_MANUFACTURER_ID_INTEL                               0x89
#define IOT_MANUFACTURER_ID_ESMT                                0x8C
#define IOT_MANUFACTURER_ID_FUDAN                               0xA1
#define IOT_MANUFACTURER_ID_HYUNDAI                             0xAD
#define IOT_MANUFACTURER_ID_SST                                 0xBF
#define IOT_MANUFACTURER_ID_MICRONIX                            0xC2
#define IOT_MANUFACTURER_ID_GIGADEVICE                          0xC8
#define IOT_MANUFACTURER_ID_GIGADEVICE_GD                       0x51
#define IOT_MANUFACTURER_ID_ISSI                                0xD5
#define IOT_MANUFACTURER_ID_WINBOND                             0xEF

/* IOT supported manufacturer information table */
#define IOT_MANUFACTURER_TABLE                                     \
{                                                         \
    {"Cypress",    IOT_MANUFACTURER_ID_CYPRESS},                   \
    {"Fujitsu",    IOT_MANUFACTURER_ID_FUJITSU},                   \
    {"EON",        IOT_MANUFACTURER_ID_EON},                       \
    {"Atmel",      IOT_MANUFACTURER_ID_ATMEL},                     \
    {"Micron",     IOT_MANUFACTURER_ID_MICRON},                    \
    {"AMIC",       IOT_MANUFACTURER_ID_AMIC},                      \
    {"Sanyo",      IOT_MANUFACTURER_ID_SANYO},                     \
    {"Intel",      IOT_MANUFACTURER_ID_INTEL},                     \
    {"ESMT",       IOT_MANUFACTURER_ID_ESMT},                      \
    {"Fudan",      IOT_MANUFACTURER_ID_FUDAN},                     \
    {"Hyundai",    IOT_MANUFACTURER_ID_HYUNDAI},                   \
    {"SST",        IOT_MANUFACTURER_ID_SST},                       \
    {"GigaDevice", IOT_MANUFACTURER_ID_GIGADEVICE},                \
    {"ISSI",       IOT_MANUFACTURER_ID_ISSI},                      \
    {"Winbond",    IOT_MANUFACTURER_ID_WINBOND},                   \
    {"Micronix",   IOT_MANUFACTURER_ID_MICRONIX},                  \
}


#ifdef __cplusplus
}
#endif

#endif /* _IOT_FLASH_DEF_H_ */
