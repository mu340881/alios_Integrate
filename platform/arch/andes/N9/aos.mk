NAME := arch_andes_N9

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := arch for andes_N9

$(NAME)_SOURCES := port.c         \
									 portISR.c         \
									 port_aos.c         \
                   panic.c        \
                   backtrace.c    \
                   vectors.S \
                   os_cpu_a.S

GLOBAL_INCLUDES += .            \
                   ./include    
