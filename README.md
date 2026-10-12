# PAC-MAC : Fine Grained Memory Access Control for Inter-VM Communication

The purpose of this project is to implement a fine-grained hardware control of memory accesses in inter-VM VirtIO-based communications, to improve the isolation
between virtualization domains. The project will include a first prototype on ZCU104 (Zynq UltraScale + MPSOC).
The steps followed during the design are:
- Building the **Jailhouse** static partitioning hypervisor on the PS of the ZCU104.
- Creation of bare metal non root cell, which will write or read on FPGA page via FPD-DMA.
- FPGA Design in order to intercept and analyze the write/read requests.
- Forwarding of the memory request to the DRAM if the request is valid, blocking it otherwise.

## System Architecture
<img width="1000" alt="image" src="https://github.com/user-attachments/assets/b7dc9571-5ca9-4723-a5ad-616f36c0c108" />

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/258adab3-4caf-4131-a2a5-e1263031d3a3" />

## Block Design
<img width="1000" alt="image" src="https://github.com/user-attachments/assets/e12ef1cc-8951-4a06-9988-d58d7d376d30" />

## Jailhouse setup

- Copy the `dma_lib` folder into `/jailhouse/inmates/lib`
- Add `dma_utils.o` from `dma_lib` to the Makefile in `/jailhouse/inmates/lib/arm64`
- Add the command `-I/$(src)/lib/dma_lib/include` to the Makefile in `/jailhouse/inmate/`

Once this is done, it will be possible to develop your own
application that uses this library and that can then be loaded into
a Jailhouse cell. 
Before launching the Jailhouse compilation,
execute the following steps so that the .bin files of the
developed source codes are produced:
• Develop your own inmate (e.g., dma transfer.c) in /jailhouse/demos/arm
• Add in the Makefile located in /jailhouse/demos/arm64
on the INMATES line the name of your file in .bin format
(e.g., dma transfer.bin)
• Add in the Makefile located in /jailhouse/demos/arm64 another line of the type:
dma_transfer.y := ../arm/dma_transfer.o
