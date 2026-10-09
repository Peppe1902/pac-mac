# PAC-MAC
PAC-MAC : Fine Grained Memory Access Control for Inter-VM Communication

The purpose of this project is to implement a fine-grained hardware control of memory accesses in inter-VM VirtIO-based communications, to improve the isolation
between virtualization domains. The project will include a first prototype on ZCU104 (Zynq UltraScale + MPSOC).
The steps followed during the design are:
- Building the **Jailhouse** static partitioning hypervisor on the PS of the ZCU104.
- Creation of bare metal non root cell, which will write or read on FPGA page via FPD-DMA.
- FPGA Design in order to intercept and analyze the write/read requests.
- If the request is considered valid, it is forwarded to the DRAM; otherwise it is blocked and an error message is sent to the PS.

## System Architecture
<img width="1000" alt="image" src="https://github.com/user-attachments/assets/b7dc9571-5ca9-4723-a5ad-616f36c0c108" />

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/258adab3-4caf-4131-a2a5-e1263031d3a3" />

## Block Design
<img width="1000" alt="image" src="https://github.com/user-attachments/assets/e12ef1cc-8951-4a06-9988-d58d7d376d30" />


