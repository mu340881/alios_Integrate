EXTRA_POST_BUILD_TARGETS += gen_crc_bin
OTA_BIN_OUTPUT_FILE := $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINTYPE_LOWER)$(BIN_OUTPUT_SUFFIX)
OTA_BIN_OUTPUT2_FILE := $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINTYPE_LOWER)_2.bin
OTA_ONE_BIN_OUTPUT_FILE := fw_ota_v100.bin

ifeq ($(HOST_OS),Win32)
	SETENV = set
else
	SETENV = export
endif

gen_crc_bin:
	echo $(OBJCOPY) -O binary -R .note -R .comment   -R .FLASH_CODE  -R SM_REGION_SHARE -R SM_REGION_ZI   -R SM_REGION_PACKET -R SM_REGION_CODE  -S  $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) $(OUTPUT_DIR)/binary/iot_iccm.bin  
	$(OBJCOPY) -O binary -R .note -R .comment   -R .FLASH_CODE  -R SM_REGION_SHARE -R SM_REGION_ZI   -R SM_REGION_PACKET -R SM_REGION_CODE  -S  $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) $(OUTPUT_DIR)/binary/iot_iccm.bin  
	echo $(OBJCOPY) -O binary -j  .FLASH_CODE  -j SM_REGION_CODE  -S  $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) $(OUTPUT_DIR)/binary/iot_flash.bin 
	$(OBJCOPY) -O binary -j  .FLASH_CODE  -j SM_REGION_CODE  -S  $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) $(OUTPUT_DIR)/binary/iot_flash.bin 
	echo $(OUTPUT_DIR)/binary/iot_iccm.bin $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT_FILE)  
	#$(CP)  $(OUTPUT_DIR)/binary/iot_iccm.bin $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT_FILE) 
	#$(CP)  $(OUTPUT_DIR)/binary/iot_flash.bin $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT2_FILE)
	echo python platform/mcu/atbm6431/tools/combin_iot.py $(OUTPUT_DIR) /binary/iot_iccm.bin $(OUTPUT_DIR)/binary/iot_flash.bin -o $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT_FILE)  $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT2_FILE) -p 12967 7730 0
	python platform/mcu/atbm6431/tools/combin_iot.py $(OUTPUT_DIR)/binary/iot_iccm.bin $(OUTPUT_DIR)/binary/iot_flash.bin -o $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT_FILE)  $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT2_FILE) -p 12967 7730 0
	python platform/mcu/atbm6431/tools/pack_ota_bin.py $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT_FILE) $(OUTPUT_DIR)/binary/$(OTA_BIN_OUTPUT2_FILE) -o $(OUTPUT_DIR)/binary/$(OTA_ONE_BIN_OUTPUT_FILE)
	$(OBJDUMP) -d -S  $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) >  $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER).S
	$(RM) $(OUTPUT_DIR)/binary/iot_flash.bin $(OUTPUT_DIR)/binary/iot_iccm.bin 
