/*---------------------------ADDRESSESS-----------------------------------*/
#define GDMA_CH0_BASE_ADDR 		0xFD500000
#define GDMA_CH_STATUS_ADDR  		(GDMA_CH0_BASE_ADDR + 0x0000011CU)
#define GDMA_CH_CTR0_ADDR 		(GDMA_CH0_BASE_ADDR + 0x00000110U)

#define GDMA_CH_SRC_DSCR_WORD0_ADDR 	(GDMA_CH0_BASE_ADDR + 0x00000128U)
#define GDMA_CH_SRC_DSCR_WORD1_ADDR 	(GDMA_CH0_BASE_ADDR + 0x0000012CU)
#define GDMA_CH_SRC_DSCR_WORD2_ADDR 	(GDMA_CH0_BASE_ADDR + 0x00000130U)

#define GDMA_CH_DST_DSCR_WORD0_ADDR 	(GDMA_CH0_BASE_ADDR + 0x00000138U)
#define GDMA_CH_DST_DSCR_WORD1_ADDR 	(GDMA_CH0_BASE_ADDR + 0x0000013CU)
#define GDMA_CH_DST_DSCR_WORD2_ADDR 	(GDMA_CH0_BASE_ADDR + 0x00000140U)

#define GDMA_CH_DATA_ATTR_ADDR		(GDMA_CH0_BASE_ADDR + 0x00000120U)
#define GDMA_CH_CTRL2_ADDR 		(GDMA_CH0_BASE_ADDR + 0x00000200U)
#define GDMA_CH_ISR_ADDR 		(GDMA_CH0_BASE_ADDR + 0x00000100U)

#define GDMA_CH_IEN_ADDR		(GDMA_CH0_BASE_ADDR + 0x00000108U)
	
#define CRF_APB_BASE_ADDR		0xFD1A0000
#define FPD_DMA_REF_CTRL_ADDR		( CRF_APB_BASE_ADDR + 0x000000B8U)

/*-----------------------------UTILS--------------------------------------*/
#define BUFFER_LENGTH 20
#define DONE_WITH_ERROR 0x00000003U
#define DONE_WITH_NO_ERROR 0x00000000U
#define PHYS_BASE_ADDR 0x50600000U
#define FPD_DMA_IRQ 	156

#define DSB_ISH() asm volatile("dsb ish" ::: "memory")
#define DSB_SY() asm volatile("dsb sy" ::: "memory")
#define ISB()	asm volatile("isb" ::: "memory");

#define DCACHE_CLEAN		0
#define DCACHE_INVALIDATE	1
#define DCACHE_CLEAN_INVALIDATE	2


#include <inmate.h>

//types u32 e u64 are defined in inmate.h
//static u32 src[BUFFER_LENGTH] __attribute__((aligned(64)));
//static u32 dst[BUFFER_LENGTH] __attribute__((aligned(64)));
static u32 *src = (u32*) 0x50A00000;
static u32 *dst = (u32*) 0x50A10000;
static u32 *dst_ptr = (u32*) 0x1050A10000;

u64 start_time;

static inline u64 virt_to_phys(u64 virt_addr,u64 phys_base){
	return virt_addr + phys_base;
}


/* Function to perform CLEAN, INVALIDATE and CLEAN+INVALIDATE operations on the data cache up to the point of coherency */
static inline void dcache_flush_range(void *addr, u64 size, int choice){

    u64 ctr;
    u64 dsize,dline,end;

    //read the CTR_EL0 control register to obtain the data cache line length
 	asm volatile("mrs %[ctr], ctr_el0\n\t" : [ctr] "=r" (ctr));
	
	//data cache line size in bytes (computed from the DminLine[19:16] field of the control register)
	dsize = 4 << ((ctr >> 16) & 0xf);
	
	dline = (u64)addr & ~(dsize-1); //round down: make the address divisible by dsize (clear the low bits)

	end = (dline + size + dsize-1) & ~(dsize-1); //round up: useful when end is not divisible by dsize
	
	DSB_ISH();//dsb(ish)

	if(choice == DCACHE_CLEAN){

		for (; dline < end; dline += dsize) {
			asm volatile("dc cvac, %0" :: "r"(dline) : "memory");
		}

	}else if(choice == DCACHE_INVALIDATE){

		for (; dline < end; dline += dsize) {
			asm volatile("dc ivac, %0" :: "r"(dline) : "memory");
		}

	}else{

		for (; dline < end; dline += dsize) {
			asm volatile("dc civac, %0" :: "r"(dline) : "memory");
		}

	}
	
	DSB_ISH(); //dsb(ish) : blocks execution until pending memory operations are completed
	ISB();//isb : empy processor pipeline
}



/* Function to configure a simple-mode transfer with FPD-DMA (called GDMA) on ZCU104 */
static void gdma_config_simple_mode(u64 src_addr,u64  dst_addr,u32 length){ 

	volatile u32* ptr = (volatile u32* ) GDMA_CH_STATUS_ADDR ;

	printk("GDMA_CH_STATUS (BEFORE WHILE) = %u\n",*ptr);

	//wait until ZDMA_CH_STATUS[1:0] becomes 00 (done, no error) or 11 (done with error)
	while((*ptr & DONE_WITH_ERROR )!=DONE_WITH_ERROR  && (*ptr & DONE_WITH_ERROR )!=DONE_WITH_NO_ERROR);

	printk("GDMA_CH_STATUS (AFTER WHILE) = %u\n",*ptr);

	ptr = (volatile u32*) GDMA_CH_CTR0_ADDR;

	*ptr &= ~(1 << 6); //SIMPLE MODE DMA (POINT_TYPE = 0)

	ptr = (volatile u32*) GDMA_CH_SRC_DSCR_WORD0_ADDR;

	*ptr = (u32) (src_addr & 0xFFFFFFFF); //lower 32-bit of src address

	printk("prima parte indirizzo src = %u\n",*ptr);

	ptr = (volatile u32*) GDMA_CH_SRC_DSCR_WORD1_ADDR;

	*ptr = (u32) ((src_addr >> 32) & 0xFFF); //upper 12-bits of src address (N.B GDMA_CH_SRC_DSCR_WORD1_ADDR[31:17] are reserved for future use)

	printk("seconda parte indirizzo src = %u\n",*ptr);

	ptr = (volatile u32*) GDMA_CH_DST_DSCR_WORD0_ADDR;

	*ptr = (u32) (dst_addr & 0xFFFFFFFF); //lower 32-bit of dst address
	
	printk("prima parte indirizzo dst = %u\n",*ptr);

	ptr = (volatile u32*) GDMA_CH_DST_DSCR_WORD1_ADDR;

	*ptr = (u32) ((dst_addr >> 32) & 0xFFF); //upper 12 bits of dst address (N.B GDMA_CH_DST_DSCR_WORD1_ADDR[31:17] are reserved for future use)
	
	printk("seconda parte indirizzo dst = %u\n",*ptr);

	ptr = (volatile u32*) GDMA_CH_SRC_DSCR_WORD2_ADDR ;

	*ptr = 4 * length; //src transfer size 

	ptr = (volatile u32*) GDMA_CH_DST_DSCR_WORD2_ADDR ;

	*ptr = 4 * length; //dst transfer size

	ptr = (volatile u32*) GDMA_CH_DATA_ATTR_ADDR; // AXI transaction features for data read and write during DMA transfer.
	
	printk("GDMA_CH_DATA_ATTR = %u\n",*ptr); //*ptr must be equal to 0x0483D20F (default value, in decimal : 75.747.855)

	DSB_ISH(); //dsb(ish) : blocks execution until pending memory operations are completed
	ISB();//isb : empy processor pipeline
}


/*Function to start FPD-DMA transfer*/
static void gdma_start_transfer(void){

	volatile u32 *ctrl2_ptr = (volatile u32*) GDMA_CH_CTRL2_ADDR;
	
	*ctrl2_ptr |= (1U << 0); //transfer enabled
	
	//printk("GDMA_CH_CTRL2 = %u\n",*ptr);
}



/* Interrupt handler for the interrupt on line 156, generated by the FPD_DMA */
static void handle_IRQ(unsigned int irqn){
	
	volatile u32 *isr_ptr = (volatile u32*) GDMA_CH_ISR_ADDR ;

	*isr_ptr |= (1U << 10); //clear the interrupt by writing 1 to bit 10 of the ISR register (DMA_DONE)

	if(irqn != FPD_DMA_IRQ){
		return;
	}
	
	volatile u32 *status_ptr = (volatile u32*)GDMA_CH_STATUS_ADDR;

	if((*status_ptr & DONE_WITH_ERROR) == DONE_WITH_NO_ERROR){

		u64 end_time = timer_get_ticks();

		u64 diff = timer_ticks_to_ns(end_time - start_time);

		printk("Time required = %llu ns\n ",diff);
	}
}


/* Function to let the GDMA generate an interrupt */
static void gdma_interrupt_enable(unsigned int irqn){

	volatile u32 *ien_ptr = (volatile u32*) GDMA_CH_IEN_ADDR;

	*ien_ptr |= (1U << 10); // interrupt generation enabled when the DMA is done (with or without error)

	irq_init(handle_IRQ); //handler association
	irq_enable(irqn);

	DSB_ISH();
	ISB();
}



/**Function to enable FPD-DMA****/
static void power_on_fpd_dma(void){

 	volatile u32* ptr = (volatile u32*)FPD_DMA_REF_CTRL_ADDR;	

	*ptr |= (1U << 24); //CLKACT  = 1 (clock enabled for FPD-DMA)

	printk("FPD_DMA_REF_CTRL : CLKACT = %u\n",*ptr);
}


/**FPD-DMA is used (called GDMA). It has 8 channels and the first CH0 is used in this program (FPD = Full Power Domain)**/
void inmate_main(void){
	
	DSB_ISH(); 
	ISB();

	map_range((void*)GDMA_CH0_BASE_ADDR,0x10000,MAP_UNCACHED);
	map_range((void*)CRF_APB_BASE_ADDR,0x1000,MAP_UNCACHED);
	map_range((void*)src,BUFFER_LENGTH*4,MAP_UNCACHED); //comment this line if src is located in RAM
	map_range((void*)dst,BUFFER_LENGTH*4,MAP_UNCACHED);
	map_range((void*)dst_ptr,BUFFER_LENGTH*4,MAP_UNCACHED);

	DSB_ISH(); 
	ISB();

	power_on_fpd_dma();

	memset((void*)dst,0,BUFFER_LENGTH*4);
	
	for(int i=0;i<BUFFER_LENGTH;i++){
		src[i] = i+1;
		//printk("written src[%d] = %d\n",i,src[i]);
	}	
	
	DSB_ISH(); 
	ISB();

	//dcache_flush_range((void*)src,BUFFER_LENGTH*4,DCACHE_CLEAN); //if src is located in RAM uncomment this line !!!!
	
	//gdma_config_simple_mode((u64)src,virt_to_phys((u64)dst,PHYS_BASE_ADDR),BUFFER_LENGTH);
	//gdma_config_simple_mode(virt_to_phys((u64)src,PHYS_BASE_ADDR),(u64)dst_ptr,BUFFER_LENGTH);//transfer from RAM to FPGA page
	gdma_config_simple_mode((u64)src,(u64)dst_ptr,BUFFER_LENGTH);

	gdma_interrupt_enable(FPD_DMA_IRQ);

	//gdma_config_simple_mode((u64)src,(u64)dst,BUFFER_LENGTH);

	start_time = timer_get_ticks();

	gdma_start_transfer();

	halt();
}


