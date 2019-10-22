ENTRY(_start)
SECTIONS
{
	PROVIDE (__executable_start = 0x00010000);
	NDS_SAG_LMA = 0x00000000 ;
	. = 0x00010000;
	ITCM_RO_BEGIN = .;
	_ILM_BASE = 0x00010000;
	_DLM_BASE = 0x00800000;
	PATCH_BASE = 0x0001f000;
	_ILM_SIZE = 0x00020000;
	_DLM_SIZE = 0x0000a000;/*0x0000c000;*/
	SRAM_SIZE = 0x00020000;
	STACKSIZE = 0x00000800;
	ICCM_SIZE =  _ILM_SIZE -4;
	DCCM_SIZE = _DLM_SIZE - STACKSIZE;
	STACK_NZI_BEGIN = _DLM_BASE + DCCM_SIZE;
	STACK_NZI_END = STACK_NZI_BEGIN + STACKSIZE - 4;
	FLASH_CFG_BASE = 0x2f000;/*0x30000-4096 _DLM_SIZE + _ILM_SIZE*/	
	FLASH_CODE2_BASE = 0x00008000;/*32K*/
	FLASH_ROM_BASE = 0x30000;/*_DLM_SIZE + _ILM_SIZE*/
	PASRAM_NZI_BEGIN = 0x09000000;
	FLASH_CODE_BEGIN = 0x00c40000;
	FLASH_CODE2_BEGIN = 0x00c10000+FLASH_CODE2_BASE;
	FLASH_CNFIG_BEGIN = 0x00c3f000;/*FLASH_CODE_BEGIN-4096*/
	.vector	.	:	AT(NDS_SAG_LMA)	{ *(.vector) }
	.nds32_init		:	AT(LOADADDR (.vector) + SIZEOF (.vector))
		{ KEEP(*(.nds32_init .nds32_init.*)) . = ALIGN(4);}
	.EILM_REGION_CODE		:	AT(LOADADDR (.nds32_init) + SIZEOF (.nds32_init))
		{ *(.EILM_REGION_CODE) . = ALIGN(4);}
	.keep_text		:	AT(LOADADDR (.EILM_REGION_CODE) + SIZEOF (.EILM_REGION_CODE))
		{ KEEP(*(.keep_text)) . = ALIGN(4);}
	.interp		:	AT(ALIGN(LOADADDR (.keep_text) + SIZEOF (.keep_text), ALIGNOF(.interp)))
		{ *(.interp) }
	.hash		:	AT(ALIGN(LOADADDR (.interp) + SIZEOF (.interp), ALIGNOF(.hash)))
		{ *(.hash) }
	.dynsym		:	AT(ALIGN(LOADADDR (.hash) + SIZEOF (.hash), ALIGNOF(.dynsym)))
		{ *(.dynsym) }
	.dynstr		:	AT(ALIGN(LOADADDR (.dynsym) + SIZEOF (.dynsym), ALIGNOF(.dynstr)))
		{ *(.dynstr) }
	.gnu.version		:	AT(ALIGN(LOADADDR (.dynstr) + SIZEOF (.dynstr), ALIGNOF(.gnu.version)))
		{ *(.gnu.version) }
	.gnu.version_d		:	AT(ALIGN(LOADADDR (.gnu.version) + SIZEOF (.gnu.version), ALIGNOF(.gnu.version_d)))
		{ *(.gnu.version_d) }
	.gnu.version_r		:	AT(ALIGN(LOADADDR (.gnu.version_d) + SIZEOF (.gnu.version_d), ALIGNOF(.gnu.version_r)))
		{ *(.gnu.version_r) }
	.rel.init		:	AT(ALIGN(LOADADDR (.gnu.version_r) + SIZEOF (.gnu.version_r), ALIGNOF(.rel.init)))
		{ *(.rel.init) }
	.rela.init		:	AT(ALIGN(LOADADDR (.rel.init) + SIZEOF (.rel.init), ALIGNOF(.rela.init)))
		{ *(.rela.init) }
	.rel.text		:	AT(ALIGN(LOADADDR (.rela.init) + SIZEOF (.rela.init), ALIGNOF(.rel.text)))
		{ *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*) }
	.rela.text		:	AT(ALIGN(LOADADDR (.rel.text) + SIZEOF (.rel.text), ALIGNOF(.rela.text)))
		{ *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*) }
	.rel.fini		:	AT(ALIGN(LOADADDR (.rela.text) + SIZEOF (.rela.text), ALIGNOF(.rel.fini)))
		{ *(.rel.fini) }
	.rela.fini		:	AT(ALIGN(LOADADDR (.rel.fini) + SIZEOF (.rel.fini), ALIGNOF(.rela.fini)))
		{ *(.rela.fini) }
	.rel.rodata		:	AT(ALIGN(LOADADDR (.rela.fini) + SIZEOF (.rela.fini), ALIGNOF(.rel.rodata)))
		{ *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*) }
	.rela.rodata		:	AT(ALIGN(LOADADDR (.rel.rodata) + SIZEOF (.rel.rodata), ALIGNOF(.rela.rodata)))
		{ *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*) }
	.rel.data.rel.ro		:	AT(ALIGN(LOADADDR (.rela.rodata) + SIZEOF (.rela.rodata), ALIGNOF(.rel.data.rel.ro)))
		{ *(.rel.data.rel.ro*) }
	.rela.data.rel.ro		:	AT(ALIGN(LOADADDR (.rel.data.rel.ro) + SIZEOF (.rel.data.rel.ro), ALIGNOF(.rela.data.rel.ro)))
		{ *(.rel.data.rel.ro*) }
	.rel.data		:	AT(ALIGN(LOADADDR (.rela.data.rel.ro) + SIZEOF (.rela.data.rel.ro), ALIGNOF(.rel.data)))
		{ *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*) }
	.rela.data		:	AT(ALIGN(LOADADDR (.rel.data) + SIZEOF (.rel.data), ALIGNOF(.rela.data)))
		{ *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*) }
	.rel.tdata		:	AT(ALIGN(LOADADDR (.rela.data) + SIZEOF (.rela.data), ALIGNOF(.rel.tdata)))
		{ *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*) }
	.rela.tdata		:	AT(ALIGN(LOADADDR (.rel.tdata) + SIZEOF (.rel.tdata), ALIGNOF(.rela.tdata)))
		{ *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*) }
	.rel.tbss		:	AT(ALIGN(LOADADDR (.rela.tdata) + SIZEOF (.rela.tdata), ALIGNOF(.rel.tbss)))
		{ *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*) }
	.rela.tbss		:	AT(ALIGN(LOADADDR (.rel.tbss) + SIZEOF (.rel.tbss), ALIGNOF(.rela.tbss)))
		{ *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*) }
	.rel.ctors		:	AT(ALIGN(LOADADDR (.rela.tbss) + SIZEOF (.rela.tbss), ALIGNOF(.rel.ctors)))
		{ *(.rel.ctors) }
	.rela.ctors		:	AT(ALIGN(LOADADDR (.rel.ctors) + SIZEOF (.rel.ctors), ALIGNOF(.rela.ctors)))
		{ *(.rela.ctors) }
	.rel.dtors		:	AT(ALIGN(LOADADDR (.rela.ctors) + SIZEOF (.rela.ctors), ALIGNOF(.rel.dtors)))
		{ *(.rel.dtors) }
	.rela.dtors		:	AT(ALIGN(LOADADDR (.rel.dtors) + SIZEOF (.rel.dtors), ALIGNOF(.rela.dtors)))
		{ *(.rela.dtors) }
	.rela.dyn		:	AT(ALIGN(LOADADDR (.rela.dtors) + SIZEOF (.rela.dtors), ALIGNOF(.rela.dyn)))
		{ *(rela.dyn) *(.rela__libc_subfreeres) *(.rela__libc_atexit) *(.rela__libc_thread_subfreeres) *(.rela.init_array) *(.rela.fini_array) }
	.rel.got		:	AT(ALIGN(LOADADDR (.rela.dyn) + SIZEOF (.rela.dyn), ALIGNOF(.rel.got)))
		{ *(.rel.got) }
	.rela.got		:	AT(ALIGN(LOADADDR (.rel.got) + SIZEOF (.rel.got), ALIGNOF(.rela.got)))
		{ *(.rela.got) }
	.rel.sdata		:	AT(ALIGN(LOADADDR (.rela.got) + SIZEOF (.rela.got), ALIGNOF(.rel.sdata)))
		{ *(.rel.sdata .rel.sdata.* .rel.gnu.linkonce.s.*) }
	.rela.sdata		:	AT(ALIGN(LOADADDR (.rel.sdata) + SIZEOF (.rel.sdata), ALIGNOF(.rela.sdata)))
		{ *(.rela.sdata .rela.sdata.* .rela.gnu.linkonce.s.*) }
	.rel.sbss		:	AT(ALIGN(LOADADDR (.rela.sdata) + SIZEOF (.rela.sdata), ALIGNOF(.rel.sbss)))
		{ *(.rel.sbss .rel.sbss.* .rel.gnu.linkonce.sb.*) }
	.rela.sbss		:	AT(ALIGN(LOADADDR (.rel.sbss) + SIZEOF (.rel.sbss), ALIGNOF(.rela.sbss)))
		{ *(.rela.sbss .rela.sbss.* .rela.gnu.linkonce.sb.*) }
	.rel.sdata2		:	AT(ALIGN(LOADADDR (.rela.sbss) + SIZEOF (.rela.sbss), ALIGNOF(.rel.sdata2)))
		{ *(.rel.sdata2 .rel.sdata2.* .rel.gnu.linkonce.s2.*) }
	.rela.sdata2		:	AT(ALIGN(LOADADDR (.rel.sdata2) + SIZEOF (.rel.sdata2), ALIGNOF(.rela.sdata2)))
		{ *(.rela.sdata2 .rela.sdata2.* .rela.gnu.linkonce.s2.*) }
	.rel.sbss2		:	AT(ALIGN(LOADADDR (.rela.sdata2) + SIZEOF (.rela.sdata2), ALIGNOF(.rel.sbss2)))
		{ *(.rel.sbss2 .rel.sbss2.* .rel.gnu.linkonce.sb2.*) }
	.rela.sbss2		:	AT(ALIGN(LOADADDR (.rel.sbss2) + SIZEOF (.rel.sbss2), ALIGNOF(.rela.sbss2)))
		{ *(.rela.sbss2 .rela.sbss2.* .rela.gnu.linkonce.sb2.*) }
	.rel.bss		:	AT(ALIGN(LOADADDR (.rela.sbss2) + SIZEOF (.rela.sbss2), ALIGNOF(.rel.bss)))
		{ *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*) }
	.rela.bss		:	AT(ALIGN(LOADADDR (.rel.bss) + SIZEOF (.rel.bss), ALIGNOF(.rela.bss)))
		{ *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*) }
	.rel.plt		:	AT(ALIGN(LOADADDR (.rela.bss) + SIZEOF (.rela.bss), ALIGNOF(.rel.plt)))
		{ *(.rel.plt) }
	.rela.plt		:	AT(ALIGN(LOADADDR (.rel.plt) + SIZEOF (.rel.plt), ALIGNOF(.rela.plt)))
		{ *(.rela.plt) }
	.init		:	AT(ALIGN(LOADADDR (.rela.plt) + SIZEOF (.rela.plt), ALIGNOF(.init)))
		{ KEEP(*(.init)) }
	.plt		:	AT(ALIGN(LOADADDR (.init) + SIZEOF (.init), ALIGNOF(.plt)))
		{ *(.plt) }
	.fini		:	AT(ALIGN(LOADADDR (.plt) + SIZEOF (.plt), ALIGNOF(.fini)))
		{ KEEP(*(.fini)) }
	.ex9.itable		:	AT(ALIGN(LOADADDR (.fini) + SIZEOF (.fini), ALIGNOF(.ex9.itable)))
		{ *(.ex9.itable) }
	PROVIDE (__etext = .);
	PROVIDE (_etext = .);
	PROVIDE (etext = .);
	.rodata2		:	AT(ALIGN(LOADADDR (.ex9.itable) + SIZEOF (.ex9.itable), ALIGNOF(.rodata2)))
		{ *(  .gnu.linkonce.r.*) }
	.rodata1		:	AT(ALIGN(LOADADDR (.rodata2) + SIZEOF (.rodata2), ALIGNOF(.rodata1)))
		{ *(.rodata1) }
	.sdata2		:	AT(ALIGN(LOADADDR (.rodata1) + SIZEOF (.rodata1), ALIGNOF(.sdata2)))
		{ *(.sdata2 .sdata2.* .gnu.linkonce.s2.*) }
	.sbss2		:	AT(ALIGN(LOADADDR (.sdata2) + SIZEOF (.sdata2), ALIGNOF(.sbss2)))
		{ *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*) }
	.eh_frame_hdr		:	AT(ALIGN(LOADADDR (.sbss2) + SIZEOF (.sbss2), ALIGNOF(.eh_frame_hdr)))
		{ *(.eh_frame_hdr) }	
	ITCM_RO_SIZE = . - ITCM_RO_BEGIN;
	. = FLASH_CODE2_BEGIN;
	.FLASH_CODE2	:	AT(FLASH_CODE2_BASE)	{  *(.FLASH_CODE2) }	
	FLASH_CODE2_SIZE = . - FLASH_CODE2_BEGIN;
	
	NDS_SAG_LMA = _DLM_BASE ;
	. = _DLM_BASE;
	DTCM_CONST_BEGIN = .;
	__data_start = .;
	. = ALIGN(0x20);
	__data_lmastart =  LOADADDR (.eh_frame);
	.eh_frame		:	AT(_ILM_SIZE)
		{ KEEP(*(.eh_frame)) }
	.gcc_except_table		:	AT(ALIGN(LOADADDR (.eh_frame) + SIZEOF (.eh_frame), ALIGNOF(.gcc_except_table)))
		{ KEEP(*(.gcc_except_table)) *(.gcc_except_table.*) }
	.tdata		:	AT(ALIGN(LOADADDR (.gcc_except_table) + SIZEOF (.gcc_except_table), ALIGNOF(.tdata)))
		{ *(.tdata .tdata.* .gnu.linkonce.td.*) }
	. = ALIGN(32 / 8);
	PROVIDE (__preinit_array_start = .);
	.preinit_array		:	AT(ALIGN(LOADADDR (.tdata) + SIZEOF (.tdata), ALIGNOF(.preinit_array)))
		{ KEEP(*(.preinit_array)) }
	PROVIDE (__preinit_array_end = .);
	PROVIDE (__init_array_start = .);
	.init_array		:	AT(ALIGN(LOADADDR (.preinit_array) + SIZEOF (.preinit_array), ALIGNOF(.init_array)))
		{ KEEP(*(.init_array)) }
	PROVIDE (__init_array_end = .);
	PROVIDE (__fini_array_start = .);
	.fini_array		:	AT(ALIGN(LOADADDR (.init_array) + SIZEOF (.init_array), ALIGNOF(.fini_array)))
		{ KEEP(*(.fini_array)) }
	PROVIDE (__fini_array_end = .);
	.ctors		:	AT(ALIGN(LOADADDR (.fini_array) + SIZEOF (.fini_array), ALIGNOF(.ctors)))
		{ KEEP(*crtbegin*.o(.ctors)) KEEP(*(EXCLUDE_FILE (*crtend*.o ) .ctors)) KEEP(*(SORT(.ctors.*))) KEEP(*(.ctors)) }
	.dtors		:	AT(ALIGN(LOADADDR (.ctors) + SIZEOF (.ctors), ALIGNOF(.dtors)))
		{ KEEP(*crtbegin*.o(.dtors)) KEEP(*(EXCLUDE_FILE (*crtend*.o ) .dtors)) KEEP(*(SORT(.dtors.*))) KEEP(*(.dtors)) }
	.jcr		:	AT(ALIGN(LOADADDR (.dtors) + SIZEOF (.dtors), ALIGNOF(.jcr)))
		{ KEEP(*(.jcr)) }
	.data.rel.ro		:	AT(ALIGN(LOADADDR (.jcr) + SIZEOF (.jcr), ALIGNOF(.data.rel.ro)))
		{ *(.data.rel.ro.local) *(.data.rel.ro*) }
	.dynamic		:	AT(ALIGN(LOADADDR (.data.rel.ro) + SIZEOF (.data.rel.ro), ALIGNOF(.dynamic)))
		{ *(.dynamic) }
	.data		:	AT(ALIGN(LOADADDR (.dynamic) + SIZEOF (.dynamic), ALIGNOF(.data)))
		{ *(.data .data.* .gnu.linkonce.d.*) KEEP(*(.gnu.linkonce.d.*personality*)) SORT(CONSTRUCTORS) . = ALIGN(8); }
	.data1		:	AT(ALIGN(LOADADDR (.data) + SIZEOF (.data), ALIGNOF(.data1)))
		{ *(.data1) . = ALIGN(8); }
	. = ALIGN(4);
	.got		:	AT(ALIGN(LOADADDR (.data1) + SIZEOF (.data1), ALIGNOF(.got)))
		{ *(.got.plt) *(.got) }
	.sdata_d		:	AT(ALIGN(LOADADDR (.got) + SIZEOF (.got), ALIGNOF(.sdata_d)))
		{ *(.sdata_d .sdata_d.*) }
	.sdata_w		:	AT(ALIGN(LOADADDR (.sdata_d) + SIZEOF (.sdata_d), ALIGNOF(.sdata_w)))
		{ *(.sdata_w .sdata_w.*) }
	.sdata_h		:	AT(ALIGN(LOADADDR (.sdata_w) + SIZEOF (.sdata_w), ALIGNOF(.sdata_h)))
		{ *(.sdata_h .sdata_h.*) }
	.sdata_b		:	AT(ALIGN(LOADADDR (.sdata_h) + SIZEOF (.sdata_h), ALIGNOF(.sdata_b)))
		{ *(.sdata_b .sdata_b.*) }
	.sdata_f		:	AT(ALIGN(LOADADDR (.sdata_b) + SIZEOF (.sdata_b), ALIGNOF(.sdata_f)))
		{ *(.sdata_f .sdata_f.*) }
	. = ALIGN(4);
	_edata = .;
	PROVIDE (edata = .);
	.dtcm_const		:	AT(ALIGN(LOADADDR (.sdata_f) + SIZEOF (.sdata_f), ALIGNOF(.dtcm_const)))
		{ *(.dtcm_const) }
	.dtcm2_const		:	AT(ALIGN(LOADADDR (.dtcm_const) + SIZEOF (.dtcm_const), ALIGNOF(.dtcm2_const)))
		{ *(.dtcm2_const) }
	.dtcm_data		:	AT(ALIGN(LOADADDR (.dtcm2_const) + SIZEOF (.dtcm2_const), ALIGNOF(.dtcm_data)))
		{ *(.dtcm_data) }
	.dtcm2_data		:	AT(ALIGN(LOADADDR (.dtcm_data) + SIZEOF (.dtcm_data), ALIGNOF(.dtcm2_data)))
		{ *(.dtcm2_data) }
	__zi_start = .;
	.dtcm2_zi		:	AT(ALIGN(LOADADDR (.dtcm2_data) + SIZEOF (.dtcm2_data), ALIGNOF(.dtcm2_zi)))
		{ *(.dtcm2_zi) }
	.dtcm_zi		:	AT(ALIGN(LOADADDR (.dtcm2_zi) + SIZEOF (.dtcm2_zi), ALIGNOF(.dtcm_zi)))
		{ *(.dtcm_zi) }
	__bss_start = .;
	PROVIDE (__sbss_start = .);
	PROVIDE (___sbss_start = .);
	.tbss		:	AT(ALIGN(LOADADDR (.dtcm_zi) + SIZEOF (.dtcm_zi), ALIGNOF(.tbss)))
		{ *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) }
	.sbss_f		:	AT(ALIGN(LOADADDR (.tbss) + SIZEOF (.tbss), ALIGNOF(.sbss_f)))
		{ *(.sbss_f .sbss_f.*) *(.scommon_f .scommon_f.*) }
	.sbss_b		:	AT(ALIGN(LOADADDR (.sbss_f) + SIZEOF (.sbss_f), ALIGNOF(.sbss_b)))
		{ *(.sbss_b .sbss_b.*) *(.scommon_b .scommon_b.*) . = ALIGN(2); }
	.sbss_h		:	AT(ALIGN(LOADADDR (.sbss_b) + SIZEOF (.sbss_b), ALIGNOF(.sbss_h)))
		{ *(.sbss_h .sbss_h.*) *(.scommon_h .scommon_h.*) . = ALIGN(4); }
	.sbss_w		:	AT(ALIGN(LOADADDR (.sbss_h) + SIZEOF (.sbss_h), ALIGNOF(.sbss_w)))
		{ *(.sbss_w .sbss_w.*) *(.scommon_w .scommon_w.*) *(.dynsbss) *(.scommon) . = ALIGN(8); }
	.sbss_d		:	AT(ALIGN(LOADADDR (.sbss_w) + SIZEOF (.sbss_w), ALIGNOF(.sbss_d)))
		{ *(.sbss_d .sbss_d.*) *(.scommon_d .scommon_d.*) }
	.bss		:	AT(ALIGN(LOADADDR (.sbss_d) + SIZEOF (.sbss_d), ALIGNOF(.bss)))
		{ *(.dynbss) *(.bss .bss.* .gnu.linkonce.b.*) *(COMMON) . = ALIGN(32 / 8); }	
	.dtcm_alloc		:	AT(ALIGN(LOADADDR (.bss) + SIZEOF (.bss), ALIGNOF(.dtcm_alloc)))
		{ *(.dtcm_alloc) }
	PROVIDE (__sbss_end = .);
	PROVIDE (___sbss_end = .);
	. = ALIGN(32 / 8);
	_end = .;
	PROVIDE (end = .);
	DTCM_CONST_SIZE = . - DTCM_CONST_BEGIN;
	PROVIDE (_stack = STACK_NZI_END);	
	. = FLASH_CNFIG_BEGIN;
	.FLASH_CONFIG	:	AT(FLASH_CFG_BASE)	{  *(.FLASH_CONFIG)}				
	. = FLASH_CODE_BEGIN;
	.FLASH_CODE	:	AT(FLASH_ROM_BASE)	{  *(.text .stub .text.* .gnu.linkonce.t.*) *(ITCM_TEXT) *(.itcm_text)  *(.FLASH_CODE) *(.rodata .rodata.*) *(.flash_rodata) }	
	FLASH_CODE_SIZE = . - FLASH_CODE_BEGIN;
	NDS_SAG_LMA = PASRAM_NZI_BEGIN ;
	. = PASRAM_NZI_BEGIN;
	SRAM_BEGIN = .;
	__zi_sram_start = .;
	SM_REGION_PACKET	:	{  *(SM_REGION_PACKET) }
	SM_REGION_ZI	    :	{  *(SM_REGION_ZI) }
	__zi_sram_end = .;
	SM_REGION_CODE		:	AT(ALIGN(LOADADDR (SM_REGION_ZI) + SIZEOF (SM_REGION_ZI), ALIGNOF(SM_REGION_CODE))) {  *(.SM_REGION_CODE)  }
	SM_REGION_SHARE		:	AT(ALIGN(LOADADDR (SM_REGION_CODE) + SIZEOF (SM_REGION_CODE), ALIGNOF(SM_REGION_SHARE))) {  *(SM_REGION_SHARE) }
	NDS_SAG_LMA_SIZE = . - NDS_SAG_LMA;
	SRAM_SIZE = . - SRAM_BEGIN;
	.stab	0 : { *(.stab) }
	.stabstr	0 : { *(.stabstr) }
	.stab.excl	0 : { *(.stab.excl) }
	.stab.exclstr	0 : { *(.stab.exclstr) }
	.stab.index	0 : { *(.stab.index) }
	.stab.indexstr	0 : { *(.stab.indexstr) }
	.note.nds32	0 : { *(.note.nds32) *(.note.nds32.*) }
	.comment	0 : { *(.comment) }
	.debug	0 : { *(.debug) }
	.line	0 : { *(.line) }
	.debug_srcinfo	0 : { *(.debug_srcinfo) }
	.debug_sfnames	0 : { *(.debug_sfnames) }
	.debug_aranges	0 : { *(.debug_aranges) }
	.debug_pubnames	0 : { *(.debug_pubnames) }
	.debug_info	0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev	0 : { *(.debug_abbrev) }
	.debug_line	0 : { *(.debug_line) }
	.debug_frame	0 : { *(.debug_frame) }
	.debug_str	0 : { *(.debug_str) }
	.debug_loc	0 : { *(.debug_loc) }
	.debug_macinfo	0 : { *(.debug_macinfo) }
	.debug_weaknames	0 : { *(.debug_weaknames) }
	.debug_funcnames	0 : { *(.debug_funcnames) }
	.debug_typenames	0 : { *(.debug_typenames) }
	.debug_varnames	0 : { *(.debug_varnames) }
}
ASSERT( ITCM_RO_SIZE <= 0x8000, OVERFLOW1);
ASSERT( FLASH_CODE2_SIZE <= 0x18000, OVERFLOW5);
ASSERT( DTCM_CONST_SIZE <= DCCM_SIZE, OVERFLOW2);
ASSERT( SRAM_SIZE <= 0x20000, OVERFLOW3);
ASSERT( FLASH_CODE_SIZE <= 0xBF000, OVERFLOW4);
