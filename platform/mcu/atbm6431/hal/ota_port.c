#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include "ota_log.h"
#include "ota_hal_plat.h"
#include "atbm_hal.h"
#include "hal/soc/soc.h"
#include "app_flash_param.h"

#define FLASH_OTA_BLOCK 256 
#define OTA_BUF_LEN (1024) //need big than OTA_BUFFER_MAX_SIZE+FLASH_OTA_BLOCK
static int file_offset=0;
static int remain_len=0;
static int head_offset=0;
static struct ota_fw_header *pota_header=NULL;
static u8 *pota_buf=NULL;

void atbm6431_OTA_Start(void)
{
   printf("atbm6431_OTA_Start \n");
   if(pota_header==NULL)
   		pota_header = malloc(OTA_FW_HEAD_LEN);
   
   if(pota_header==NULL){
   		printf("\n<ERR>Firmware_ota alloc fail\n");
		return;
   }

   if(pota_buf==NULL)
   		pota_buf = malloc(OTA_BUF_LEN);//need big than OTA_BUFFER_MAX_SIZE*2
   
   if(pota_buf==NULL){
   		printf("\n<ERR>Firmware_ota alloc2 fail\n");
		return;
   }
     
   flash_updatafw1_block_erase();
   flash_updatafw2_block_erase();
   pota_header->fw1_length=0;
   file_offset= 0;
   head_offset=0; 
   remain_len=0;
   return;
}


int atbm6431_OTA_Write(u8 *buffer, uint32_t length_wr)
{
	int len_last_wr;
	uint32_t length = length_wr;


	if(pota_header==NULL){
			printf("\n<ERR>%s Write fail\n",__func__);
		return -1;
	}

	if((remain_len+length_wr)>OTA_BUF_LEN){		
		printf("<WARNING> please set OTA_BUF_LEN log\n",length_wr);
		return -1;		
	}
	//bulid OTA head
	if(head_offset < OTA_FW_HEAD_LEN){

		if(length+head_offset >= OTA_FW_HEAD_LEN){			
			printf("OTA_Write2 head_offset %d,len%d\n",head_offset,OTA_FW_HEAD_LEN-head_offset);
			memcpy((char *)pota_header+head_offset,buffer,OTA_FW_HEAD_LEN-head_offset);
			remain_len = length-(OTA_FW_HEAD_LEN-head_offset);
		}
		else {
			printf("OTA_Write head_offset %d,len%d\n",head_offset,length);
			memcpy((char *) pota_header+head_offset,buffer,length);			
		}
			
		head_offset += length;
		if(head_offset >= OTA_FW_HEAD_LEN){
			printf("\npota_header->fw1_length %x %x\n",pota_header->fw1_length,pota_header->fw2_length);
		}
		if(head_offset > OTA_FW_HEAD_LEN){				
			printf("OTA_Writecopy offset %d,len%d remain_len %d\n",head_offset,head_offset-OTA_FW_HEAD_LEN,remain_len);
			memcpy(pota_buf,buffer+(length-remain_len),remain_len);
		}
		file_offset=0;
		return 0;
	}

		
	memcpy(pota_buf+remain_len,buffer,length_wr);
	remain_len += length_wr;
    
	//bulid fw1
    if(file_offset < pota_header->fw1_length){
		//write fw1+fw2,just need wr fw1 len
	    if(file_offset+remain_len > pota_header->fw1_length){
			length= pota_header->fw1_length-file_offset;	
	    }
		else {
			length = remain_len;
		}
		if(length >= FLASH_OTA_BLOCK){
			len_last_wr= (length)&(~(FLASH_OTA_BLOCK-1));
			printf("fw1 %x %x \n",file_offset,len_last_wr);
			flash_updatafw1_write(file_offset,pota_buf,len_last_wr);				
			file_offset += len_last_wr;
			memmove(pota_buf,pota_buf+len_last_wr,remain_len-len_last_wr);
			remain_len-=len_last_wr;
		}
    }
	
	//bulid fw2
	if(file_offset >= pota_header->fw1_length) {
		printf("f2 %x\n",file_offset);
		length = remain_len;
		if(length >= FLASH_OTA_BLOCK){
			len_last_wr= (length)&(~(FLASH_OTA_BLOCK-1));
			//printf("fw2 %x %x \n",file_offset,len_last_wr);
			flash_updatafw2_write((file_offset-pota_header->fw1_length),pota_buf,len_last_wr);			
			file_offset += len_last_wr;
			memmove(pota_buf,pota_buf+len_last_wr,remain_len-len_last_wr);
			remain_len-=len_last_wr;
		}
		
		if(file_offset+remain_len >= pota_header->fw1_length+pota_header->fw2_length ){
			printf("fw2_write end len_last_wr %d  %x %x\n",remain_len,file_offset,pota_header->fw1_length);
			flash_updatafw2_write((file_offset-pota_header->fw1_length) ,pota_buf,remain_len);
			remain_len= 0;
		}
	}
	
	//printf("OTA_Write--\n");
	//file_offset+= length;
    return 0;
}

int atbm6431_OTA_Stop(void)
{  
	//printf("atbm6431_OTA_Stop \n");
   if(pota_header==NULL){
   		printf("\n<ERR>Firmware_ota Write fail\n");
		return -1;
   }

    /* check file md5, and burning it to flash ... finally reboot system */
    flash_updatafwFlag_write();
    if(pota_header){
		free(pota_header);
		pota_header=NULL;
   	}
    return 0;
}


static int ota_init(void *something)
{
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    uint32_t offset = param->off_bp;
    printf("ota_init off:0x%x \n",offset);
    if(offset==0) {
        /* prepare to os update  */
	    printf("ota module init\n");
	   	atbm6431_OTA_Start();
        
    }
    return 0;
}

static int ota_write(int* off, char* buf ,int buf_len)
{
	hal_TaskSuspendAll();
    atbm6431_OTA_Write((u8 *)buf ,buf_len);
	hal_TaskResumeAll();
    return 0;
}

static int ota_boot(void *something)
{
	extern void Console_SetPolling(int Polling);
	Console_SetPolling(1);

    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param == NULL) {
        return -1;
    }
    if (param->res_type == OTA_FINISH) {
	    printf("ota module OTA_FINISH\n");
		atbm6431_OTA_Stop();    
    }
    else if (param->res_type==OTA_BREAKPOINT) {
	    printf("ota module OTA_BREAKPOINT\n");
    }
    else {
        return -1;
    }

    return 0;
}


static int ota_read(int* off, char* out_buf, int out_buf_len)
{
	printf("ota_read off %x\n",*off);
    return hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t*)off, out_buf, out_buf_len);
}

static int ota_rollback(void *something)
{
    return 0;
}

const char   *aos_get_app_version(void);
static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return SYSINFO_APP_VERSION;
    }
}

ota_hal_module_t ota_hal_module = {
    .init     = ota_init,
    .write    = ota_write,
    .read     = ota_read,
    .boot     = ota_boot,
    .rollback = ota_rollback,
    .version  = ota_get_version,
};

