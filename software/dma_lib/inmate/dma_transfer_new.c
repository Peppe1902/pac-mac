
#include <inmate.h>
#include <dma_utils.h>

//con tale dimensione eccedo l'ultimo indirizzo valido quindi la logica FPGA deve bloccare l'accesso
//#define BUFFER_LENGTH 1030 //PRIMO CASO DI TEST
//#define BUFFER_LENGTH 65 //SECONDO CASO DI TEST
#define BUFFER_LENGTH 65 //TERZO CASO DI TEST
#define PHYS_BASE_ADDR 0x50600000U
#define GDMA_CH0_BASE_ADDR 0xFD500000

//i tipi u32 e u64 è definito in inmate.h
//static u32 src[BUFFER_LENGTH] __attribute__((aligned(64)));
//static u32 dst[BUFFER_LENGTH] __attribute__((aligned(64)));
static u32 *src = (u32*) 0x50A00000;
//static u32 *dst = (u32*) 0x50AFF000; //PRIMO CASO DI TEST
static u32 *dst = (u32*) 0x50AFFF00; //SECONDO CASO DI TEST
//static u32 *dst = (u32*) 0x50B00000;
//static u32 *dst_ptr = (u32*) 0x1050B00000;
//static u32 *dst_ptr = (u32*) 0x1050AFF000; //PRIMO CASO DI TEST
static u32 *dst_ptr = (u32*) 0x1050AFFF00; //SECONDO CASO DI TEST

static u32 *fpga1_ptr = (u32*) 0x1050A10000;
static u32 *mem_ptr = (u32*) 0x50A10000;
static u32 *fpga2_ptr = (u32*) 0x1050B00000;

static inline u64 virt_to_phys(u64 virt_addr,u64 phys_base){
	return virt_addr + phys_base;
}


/**Usiamo FPD-DMA (indicato con GDMA che ha 8 canali noi useremo il primo CH0) -> (FPD = Full Power Domain)**/
void inmate_main(void){
	
	DSB_ISH(); 
	ISB();

	map_range((void*)GDMA_CH0_BASE_ADDR,0x10000,MAP_UNCACHED);
	map_range((void*)CRF_APB_BASE_ADDR,0x1000,MAP_UNCACHED);
	map_range((void*)src,BUFFER_LENGTH*4,MAP_UNCACHED); //commentarlo se src è in RAM
	map_range((void*)dst,BUFFER_LENGTH*4,MAP_UNCACHED);
	map_range((void*)dst_ptr,BUFFER_LENGTH*4,MAP_UNCACHED);
	//map_range((void*)dst_ptr,0x200000,MAP_UNCACHED);

	DSB_ISH(); 
	ISB();

	power_on_fpd_dma();

	memset((void*)dst,0,BUFFER_LENGTH*4);
	
	for(int i=0;i<BUFFER_LENGTH;i++){
		src[i] = i+1;
		printk("written src[%d] = %d\n",i,src[i]);
	}	

	
	DSB_ISH(); 
	ISB();

	//dcache_flush_range((void*)src,BUFFER_LENGTH*4,DCACHE_CLEAN);
	
	//gdma_config_simple_mode(GDMA_CH0_BASE_ADDR,(u64)src,virt_to_phys((u64)dst,PHYS_BASE_ADDR),BUFFER_LENGTH);
	//gdma_config_simple_mode(GDMA_CH0_BASE_ADDR,virt_to_phys((u64)src,PHYS_BASE_ADDR),(u64)dst_ptr,BUFFER_LENGTH);//transfer da RAM alla pagina per l'FPGA
	gdma_config_simple_mode(GDMA_CH0_BASE_ADDR,(u64)src,(u64)dst_ptr,BUFFER_LENGTH);
	//gdma_config_simple_mode(GDMA_CH0_BASE_ADDR,(u64)src,(u64)dst,BUFFER_LENGTH);
	
	//gdma_stress_test(GDMA_CH0_BASE_ADDR,(u64)src,(u64)dst_ptr,BUFFER_LENGTH,10000);
	//gdma_stress_test(GDMA_CH0_BASE_ADDR,(u64)src,(u64)dst,BUFFER_LENGTH,10000);

	
	u64 start_time = timer_get_ticks();

	gdma_start_transfer(GDMA_CH0_BASE_ADDR);
	
	if(gdma_poll_for_conversion(GDMA_CH0_BASE_ADDR) != -1){	
	
		//dcache_flush_range((void*)dst,BUFFER_LENGTH*4,DCACHE_INVALIDATE);

		u64 end_time = timer_get_ticks();

		u64 diff = timer_ticks_to_ns(end_time - start_time); //converto da tick in nanosecondi
		
		printk("Time required = %llu ns\n ",diff);

		printk("------------DONE WITH NO ERROR-------------\n");
		
		
		for(int i=0;i<BUFFER_LENGTH;i++){
			printk("non-root cell : dst[%d] = %d\n",i,dst[i]);
		}
		

	}else{
		printk("------------DONE WITH ERROR-------------\n");
	}

	
	printk("---------SCRITTURA AD INDIRIZZO VALIDO CON PROCESSORE---------\n");
	fpga1_ptr[4] = 1000;

	printk("dst[4] = %d\n",mem_ptr[4]);


	printk("---------LETTURA AD INDIRIZZO NON VALIDO CON PROCESSORE---------\n");
	printk("dst[4] = %d\n",fpga2_ptr[4]);
	
	halt();
}


/*
DSB = Data Synchronization Barrier
ish = Inner Shareable domain (cioè la barriera si applica all’interno del dominio di condivisione delle CPU, 
tipicamente tutte le CPU e periferiche con accesso alla stessa memoria condivisa)
*/




