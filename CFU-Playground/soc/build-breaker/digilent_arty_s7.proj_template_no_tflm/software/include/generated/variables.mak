PACKAGES=libc libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata bios
PACKAGE_DIRS=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libc /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libcompiler_rt /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libbase /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libfatfs /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitespi /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitedram /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libliteeth /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitesdcard /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitesata /home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/bios
LIBS=libc libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata
TRIPLE=riscv64-unknown-elf
CPU=vexriscv
CPUFAMILY=riscv
CPUFLAGS=-march=rv32im -mabi=ilp32 -mno-div -D__vexriscv__
CPUENDIANNESS=little
CLANG=0
CPU_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/cores/cpu/vexriscv
SOC_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc
PICOLIBC_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/pythondata-software-picolibc/pythondata_software_picolibc/data
COMPILER_RT_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/pythondata_software_compiler_rt/pythondata_software_compiler_rt/data
export BUILDINC_DIRECTORY
BUILDINC_DIRECTORY=/home/polykarpos/CFU-Playground/soc/build/digilent_arty_s7.proj_template_no_tflm/software/include
LIBC_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libc
LIBCOMPILER_RT_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libcompiler_rt
LIBBASE_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libbase
LIBFATFS_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libfatfs
LIBLITESPI_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitespi
LIBLITEDRAM_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitedram
LIBLITEETH_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/libliteeth
LIBLITESDCARD_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitesdcard
LIBLITESATA_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/liblitesata
BIOS_DIRECTORY=/home/polykarpos/CFU-Playground/third_party/python/litex/litex/soc/software/bios
LTO=0