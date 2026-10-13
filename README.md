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


## Jailhouse setup

- Copy the `software/dma_lib` folder into `/jailhouse/inmates/lib`.
- Add `dma_utils.o` from `dma_lib` to the Makefile in `/jailhouse/inmates/lib/arm64`.
- Add the command `-I/$(src)/lib/dma_lib/include` to the Makefile in `/jailhouse/inmate/`.

Once this is done, you can develop your own application using this library and load it into a Jailhouse cell.
Before starting Jailhouse compilation, execute the following steps in order to produce the `.bin` files of the developed source:
- Develop your own inmate (e.g., `dma_transfer.c`) in `/jailhouse/demos/arm`
- Add the name of your `.bin` (e.g., `dma_transfer.bin`) to the `INMATES` line of Makefile located in `/jailhouse/demos/arm64` 
- In the Makefile located in `/jailhouse/demos/arm64`, add another line of the form `dma_transfer.y := ../arm/dma_transfer.o`

Once these steps are completed, launch the compilation and extract the image that must be loaded to the microSD.

> [!NOTE]
> You may need to force copy the updated Jailhouse folder to your microSD card with the following command:

```bash
sudo cp -r path/../jailhouse /media/user_name/RootFS
```

Then, on the board, enable Jailhouse by loading the kernel module `jailhouse.ko` located in /jailhouse/driver,
and subsequently create:

1) the root cell:

```bash
sudo jailhouse enable zynqmp-zcu104.cell
```

2) the non-root cell with DMA support:

```bash
sudo jailhouse enable zynqmp-zcu104-dma.cell
```

> [!NOTE]
> The `.cell` files will be generated after Jailhouse compilation.

Load the desired `.bin` (e.g., `dma_transfer.bin`) into the non-root Jailhouse cell:

```bash
sudo jailhouse cell load 1 dma_transfer.bin -s "con-type=JAILHOUSE" -a 0x1000
```

Then start the cell (e.g., `dma_cell`):

```bash
sudo jailhouse cell start dma_cell
```


## Block Design

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/258adab3-4caf-4131-a2a5-e1263031d3a3" />

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/e12ef1cc-8951-4a06-9988-d58d7d376d30" />


