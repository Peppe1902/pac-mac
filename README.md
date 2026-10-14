# PAC-MAC : Fine Grained Memory Access Control for Inter-VM Communication

The purpose of this project is to implement a fine-grained hardware control of memory accesses in inter-VM VirtIO-based communications, to improve the isolation
between virtualization domains. The project will include a first prototype on ZCU104 (Zynq UltraScale + MPSOC).
The steps followed during the design are:
- Building the **Jailhouse** static partitioning hypervisor on the **Processing System** of the ZCU104.
- Creation of bare metal non root cell, which will write or read on FPGA page via FPD-DMA.
- FPGA Design in order to intercept and analyze the write/read requests.
- Forwarding of the memory request to the DRAM if the request is valid, blocking it otherwise.

## Setup

- Ubuntu 18.04 virtual machine under VMware
- **Free disk space:** 150 GB (200 GB preferable)
- **RAM:** 8 GB
- **Cores:** 6

## Prerequisites

This guide assumes the following are already set up:

- PetaLinux 2024.2
- Jailhouse installed on the ZCU104, following the [Minervasys documentation](https://github.com/Minervasys/jailhouse-documentation/blob/main/boards/zcu104/jailhouse.md)


## Tools & Technologies

- **HDL:** VHDL
- **Synthesis & P&R:** Vivado 2024.2
- **Embedded Linux build system:** PetaLinux
- **Hypervisor:** Jailhouse


## System Architecture

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/b7dc9571-5ca9-4723-a5ad-616f36c0c108" />

Thanks to Jailhouse, the **FPD-DMA** is allocated to **DMA_CELL**, a bare-metal non-root cell:
- FPD-DMA (full-power domain DMA) is one of the two DMA masters that can be found into PS-side of ZCU104.
- The FPD-DMA is used to transfer data from the Processing System to the FPGA.
- It can operate in different modes, in this project **Simple Mode** is used, which allows to transfer a contiguos block of elements.

The **CCI** is responsible for cache coherence management, but the FPD-DMA does not pass through it.

<img width="1000" alt="image" src="https://github.com/user-attachments/assets/dde75d36-0b0d-4e32-a7e2-6f13ba895ced" />

**ATL** translates FPGA addresses to their corresponding memory addresses: 
- ATL is mapped to addresses in range `[0x10_0000_0000,0x10_7FFF_FFFF]` and it simply ignores the two most significant digits of `AWADDR` and `ARADDR` signals
- For the remaining signals it makes passthrough.

**PAC_MAC** is the core of the design:
Internally it is composed of two components :
- **FILTER** — if the request is considered valid, this module addresses the DRAM; otherwise it addresses the SINK module (through the AXI SmartConnect).
- **SINK** — addressed by the FILTER when the transaction is considered invalid. It responds with an AXI error (`SLVERR`).

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

Then, on the board, enable Jailhouse by loading the kernel module `jailhouse.ko` located in `/jailhouse/driver`, and subsequently create:

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

Then start the cell. The cell name is the one specified in the configuration file (e.g `dma_cell`):

```bash
sudo jailhouse cell start dma_cell
```


## Block Design

Create a VIVADO project for ZCU104 MPSOC board:

<img width="800" src="https://github.com/user-attachments/assets/ded9f50b-206f-4089-a459-536811139fd3" />

Go to **Settings** and, in the IP section, add the repositories containing the custom IPs `PAC_MAC` and `ATL`.

<img width="800" src="https://github.com/user-attachments/assets/258adab3-4caf-4131-a2a5-e1263031d3a3" />

Add **Zynq UltraScale + MPSoc** IP and click **Run Connection Automation**. The PS block must be configured as reported below:

<img width="800" alt="image" src="https://github.com/user-attachments/assets/98ecbb3e-d273-4193-8060-065c2d2dd5a7" />

Add the **Processor System Reset** IP and click **Run Connection Automation** with the following options selected:

<img width="800" alt="image" src="https://github.com/user-attachments/assets/d6e74c9c-5780-4dd0-803a-c801481ee03c" />

<img width="800" alt="image" src="https://github.com/user-attachments/assets/171ad06f-ed54-441e-9fc4-7123d8072079" />

Add the **ATL** IP to the block design and use **Run Connection Automation** selecting only the first option:

<img width="800" alt="image" src="https://github.com/user-attachments/assets/82c54269-15dc-420a-bd5a-6613f8ebb027" />

Add the **PAC_MAC** IP to the block design and use **Run Connection Automation**:

<img width="800" alt="image" src="https://github.com/user-attachments/assets/1bf36176-5ec5-47de-87db-7a9caa9033a2" />

Add another **AXI SmartConnect** to the block design and configure it with one slave port and two master ports:

<img width="800" alt="image" src="https://github.com/user-attachments/assets/0b698e9b-b66d-4e23-9e42-a0315828699f" />

- Connect the PAC_MAC `M_AXI` port to the `S00_AXI` port of the AXI SmartConnect.
- Connect the SmartConnect `M00_AXI` port to the `S_AXI_HP0_FPD` port of the PS block.
- Connect the SmartConnect `M01_AXI` port to the `S01_AXI` port of the PAC_MAC IP.

At the end the complete design should be the following:

<img width="1000" src="https://github.com/user-attachments/assets/e12ef1cc-8951-4a06-9988-d58d7d376d30" />

In the **Address Editor**, assign the range `[0x11_0000_0000, 0x11_7FFF_FFFF]` to the `S01_AXI` slave interface of PAC_MAC, and the range `[0x0000_0000, 0x7FFF_FFFF]` to the `S00_AXI` slave interface of PAC_MAC.

> [!NOTE]
> Click **Assign All**, before doing the address assignment explained previously, if you see an interface labeled as **Unassign**.

<img width="800" alt="image" src="https://github.com/user-attachments/assets/4a7daf18-2e8f-4148-ad3f-514bd25977df" />

Validate the design and generate the bitstream and extract the hardware design as a `.xsa` file. 

Update the design in **Petalinux** with the following command:

```bash
petalinux-config --get-hw-description=<path_to_xsa_file>
```

> [!IMPORTANT]
> In some cases it may be necessary to clean the project first with `petalinux -x mrproper` before running the
previous command. This restores the project to the state prior to the build, so the steps required
to build **Jailhouse** (e.g. patching the kernel) will have to be repeated.

Relaunch the Petalinux build with the command:

```bash
petalinux-build
```

Extract the SD image and then connect the microSD to the board.
