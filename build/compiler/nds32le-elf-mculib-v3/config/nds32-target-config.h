/* CPU Family Names  */
#define NDS32_CPU n9
#define NDS32_LIST_CPU "N1337-S N1337-SPU N968-S N1068-S N968A-S N1068A-S N1337-FPU N1068-FPU N1068A-FPU N1068-SPU N1068A-SPU D1088 D1088-SPU D1088-FPU"
#define NDS32_CORE "n9"
#define NDS32_ARCH v3

/* Toolchain Target Names  */
#define NDS32_TARGET nds32le-elf
#define NDS32_ENDIAN little

/* Toolchain Target: nds32{be|le}-elf  */
#define NDS32_LIB_C_DEFAULT mculib_static
#define NDS32_LIB_CPP_DEFAULT mculib_static

/* Toolchain Option: -Os1, -Os2, -Os3  */
#define NDS32_SUPPORT_mOs 3

/* Toolchain Option: -Og  */
#define NDS32_SUPPORT_mOg 1

/* Toolchain Support CPU Type */
#define NDS32_SUPPORT_mcpu_TYPE 1

/* Toolchain Option: -mcmodel  */
#define NDS32_SUPPORT_CMODEL 1

/* Toolchain Support: LTO  */
#define NDS32_SUPPORT_LTO 1

/* LTO Option Support: Need to duplicate compiler options to Linker when LTO option was enabled. */
#define NDS32_SUPPORT_LTO_COMPILER_OPTIONS_TO_LINKER ["-g.*", "-m.*", "-O.*"]
