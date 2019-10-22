/*=======================================*/   
// <code build version>
/*=======================================*/   
/****************************************************/   
/*程序编译版本号，每次编译自动加 1*/  
#define FW_BUILD 1814 
/****************************************************/   
/*程序svn version*/  
#define FW_VERS SVN_VER 
/****************************************************/   
/*code mode*/  
#ifdef CFG_B2B_SIMU  
#define FW_MODE "=SIMU=" 
#else //CFG_B2B_SIMU  
#ifdef IOT_PLATFORM  
#ifdef CFG_FAST_JOIN  
#define FW_MODE "=IOT_TEST=" 
#else//CFG_FAST_JOIN   
#define FW_MODE "=IOT=" 
#endif //#ifdef CFG_FAST_JOIN 
#else //IOT_PLATFORM  
#define FW_MODE "=MODEM=" 
#endif //#ifdef IOT_PLATFORM 
#endif   
#if (CFG_DPLL_CLOCK==26)  
#define DPLL_CLK "DPLL26M" 
#endif   
#if (CFG_DPLL_CLOCK==40)  
#define DPLL_CLK "DPLL40M" 
#endif   
#if RF_SUBTYPE_DEFINE==MAXIM_RF  
#define RF_TM "RF=Maxim." 
#elif RF_SUBTYPE_DEFINE==APOLLO_SIMU  
#define RF_TM "RF=SIMU." 
#elif RF_SUBTYPE_DEFINE==APOLLO_1505  
#define RF_TM "RF=AB1505." 
#elif RF_SUBTYPE_DEFINE==APOLLO_1601  
#define RF_TM "RF=AB1601." 
#elif RF_SUBTYPE_DEFINE==APOLLO_1606  
#define RF_TM "RF=AB1606." 
#elif RF_SUBTYPE_DEFINE==APOLLO_C  
#define RF_TM "RF=APOLLO_C "
#elif RF_SUBTYPE_DEFINE==APOLLO_D  
#define RF_TM "RF=APOLLO_D "
#elif RF_SUBTYPE_DEFINE==APOLLO_E  
#define RF_TM "RF=APOLLO_E "
#elif RF_SUBTYPE_DEFINE==ATHENA_B  
#define RF_TM "RF=ATHENA_B "
#elif RF_SUBTYPE_DEFINE==ATHENA_BX  
#define RF_TM "RF=ATHENA_BX "
#else   
#define RF_TM "RF=ERROR" 
#endif   
#define RATE_VER "RATE=OLD" 
#if (USE_HI_USB)  
#define HIF_VER "=USB=" 
#else   
#define HIF_VER "=SDIO=" 
#endif   
/****************************************************/   
/*code mode, code is*/
/****************************************************/   
#if ROM_PATCH  
#define FW_LABEL FW_MODE RF_TM
#else   
#define FW_LABEL FW_MODE RF_TM
#endif   
/****************************************************/   
