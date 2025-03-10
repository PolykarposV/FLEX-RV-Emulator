FROM ubuntu:24.04

WORKDIR /workspace

RUN apt update && apt install -y git wget bzip2 python3 unzip gcc make cmake openocd expect ccache verilator libevent-dev libjson-c-dev ninja-build libusb-1.0-0-dev libftdi1-dev nano locales libtinfo-dev python3-requests usbutils
RUN locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8
RUN git clone https://github.com/google/CFU-Playground.git && cd CFU-Playground && ./scripts/setup
COPY ./CFU-Playground /workspace/FLEX-RV-Emulator
RUN cp -rf FLEX-RV-Emulator/* CFU-Playground/
RUN cd /tmp && wget https://static.dev.sifive.com/dev-tools/freedom-tools/v2020.08/riscv64-unknown-elf-gcc-10.1.0-2020.08.2-x86_64-linux-ubuntu14.tar.gz
RUN tar zxvf /tmp/riscv64-unknown-elf-gcc-10.1.0-2020.08.2-x86_64-linux-ubuntu14.tar.gz -C /workspace/CFU-Playground
RUN echo 'export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libudev.so.1' >> ~/.bashrc
RUN echo 'source /workspace/tools/Xilinx/Vivado/2020.1/settings64.sh' >> ~/.bashrc
RUN echo 'export HOME=/workspace' >> ~/.bashrc
RUN echo 'export PATH=$PATH:$HOME/CFU-Playground/riscv64-unknown-elf-gcc-10.1.0-2020.08.2-x86_64-linux-ubuntu14/bin' >> ~/.bashrc
RUN echo 'export PATH=$PATH:/workspace/tools/Xilinx/DocNav' >> ~/.bashrc
RUN echo 'export PATH=$PATH:/workspace/tools/Xilinx/Vivavo/2020.1/bin' >>~/.bashrc
RUN echo 'export RISCV=$HOME/CFU-Playground/riscv64-unknown-elf-gcc-10.1.0-2020.08.2-x86_64-linux-ubuntu14' >> ~/.bashrc
RUN echo 'export PATH=$RISCV/bin:$PATH' >> ~/.bashrc
RUN cp ~/.bashrc /workspace/.bashrc
RUN ln -s /lib/x86_64-linux-gnu/libtinfo.so.6 /lib/x86_64-linux-gnu/libtinfo.so.5
RUN rm -rf /workspace/FLEX-RV-Emulator
