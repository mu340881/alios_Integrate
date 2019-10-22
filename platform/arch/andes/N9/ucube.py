src = Split('''
        port.c
        portISR.c
        port_aos.c
		startup.c
        panic.c
        backtrace.c
        vectors.S
        os_cpu_a.S
''')


component = aos_component('andes', src)
component.add_global_includes('./include')

