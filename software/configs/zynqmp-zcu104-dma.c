#include <jailhouse/types.h>
#include <jailhouse/cell-config.h>

struct {
	struct jailhouse_cell_desc cell;
	__u64 cpus[1];
	struct jailhouse_memory mem_regions[12];
	struct jailhouse_irqchip irqchips[1];
	struct jailhouse_pci_device pci_devices[1];
	//union jailhouse_stream_id stream_ids[1]; 
} __attribute__((packed)) config = {
	.cell = {
		.signature = JAILHOUSE_CELL_DESC_SIGNATURE,
		.revision = JAILHOUSE_CONFIG_REVISION,
		.architecture = JAILHOUSE_ARM64,
		.name = "dma_cell",
		.flags = JAILHOUSE_CELL_PASSIVE_COMMREG |
			JAILHOUSE_CELL_VIRTUAL_CONSOLE_ACTIVE | JAILHOUSE_CELL_VIRTUAL_CONSOLE_PERMITTED,

		.cpu_set_size = sizeof(config.cpus),
		.num_memory_regions = ARRAY_SIZE(config.mem_regions),
		.num_irqchips = ARRAY_SIZE(config.irqchips),
		.num_pci_devices = ARRAY_SIZE(config.pci_devices),
		//.num_stream_ids = ARRAY_SIZE(config.stream_ids), 
		.vpci_irq_base = 140-32,
		.console = {
			.address = 0xff000000,
			.type = JAILHOUSE_CON_TYPE_XUARTPS,
			.flags = JAILHOUSE_CON_ACCESS_MMIO |
				 JAILHOUSE_CON_REGDIST_4,
		},
	},

	.cpus = {
		0x8,
	},

	.mem_regions = {
		/* IVSHMEM */
		/* STATE */ {
			.phys_start = 0x25000000,
			.virt_start = 0x25000000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* RW */ {
			.phys_start = 0x25001000,
			.virt_start = 0x25001000,
			.size = 0x9000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_ROOTSHARED | JAILHOUSE_MEM_IO,
		},
		/* Output of VM1 - Input for VM2 */ {	
			.phys_start = 0x2500a000,
			.virt_start = 0x2500a000,
			.size = 0x0E552000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* Output of VM2 - Input for VM1 */ {	
			.phys_start = 0x3355C000,
			.virt_start = 0x3355C000,
			.size = 0x0E552000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_IO |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* Output of VM3 - Input for VM1 */ {	
			.phys_start = 0x41AAE000,
			.virt_start = 0x41AAE000,
			.size = 0x0E552000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_IO |
				JAILHOUSE_MEM_ROOTSHARED,
		},
		/* UART */ {
			.phys_start = 0xff000000,
			.virt_start = 0xff000000,
			.size = 0x1000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO | JAILHOUSE_MEM_ROOTSHARED,
		},
		/* RAM */ {
			.phys_start = 0x50600000,
			.virt_start = 0,
			.size = 0x00400000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_EXECUTE | JAILHOUSE_MEM_LOADABLE | JAILHOUSE_MEM_DMA,
		},

		
		/* Mem for DMA */{
			.phys_start = 0x50A00000,
			.virt_start = 0x50A00000,
			.size = 0x00200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE | JAILHOUSE_MEM_DMA,
		},
		
		
		/* Mem for FPGA */{
			.phys_start = 0x1050A00000,
			.virt_start = 0x1050A00000,
			.size = 0x00200000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE | JAILHOUSE_MEM_DMA,
		},
		

		/* FPD-DMA */
		{
			.phys_start = 0xFD500000,
			.virt_start = 0xFD500000,
			.size = 0x00010000, // 64 KB
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},

		
		/* CRF_APB */
		{
			.phys_start = 0xFD1A0000,
			.virt_start = 0xFD1A0000,
			.size = 0x00001000, // 4 KB
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_IO,
		},
		

		/* communication region */ {
			.virt_start = 0x80000000,
			.size = 0x00001000,
			.flags = JAILHOUSE_MEM_READ | JAILHOUSE_MEM_WRITE |
				JAILHOUSE_MEM_COMM_REGION,
		},
	},

	.irqchips = {
		/* GIC */ {
			.address = 0xf9010000,
			.pin_base = 32,
			.pin_bitmap = {
				(1<<(52 - 32)),
				0,
				0,
				(1 << (140 - 128)) | (1 << (141 - 128)) | (1 << (156 - 128)),
			},
		},
	},

	.pci_devices = {
		{ /* IVSHMEM (demo) */
			.type = JAILHOUSE_PCI_TYPE_IVSHMEM,
			.domain = 2,
			.bdf = 0 << 3,
			.bar_mask = JAILHOUSE_IVSHMEM_BAR_MASK_INTX,
			.shmem_regions_start = 0,
			.shmem_dev_id = 0,
			.shmem_peers = 3,
			.shmem_protocol = JAILHOUSE_SHMEM_PROTO_UNDEFINED,
		},
	},
	
	/*
	.stream_ids = {

		{
			.mmu500.id = 0x14e8,	// [14:10] TBU5 ID 00101 , [9:0] FPD Master Stream-ID 0011,10,1xxx CH = {0,..7}
			.mmu500.mask_out = 0x0, 
		},
	},
	*/
	
};
