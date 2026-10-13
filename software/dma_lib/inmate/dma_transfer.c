#include <inmate.h>
#include <dma_utils.h>

/*-----------------------------UTILS--------------------------------------*/
#define GDMA_CH0_BASE_ADDR 0xFD500000
#define BUFFER_LENGTH 1000
#define DONE_WITH_ERROR 0x00000003U
#define DONE_WITH_NO_ERROR 0x00000000U
#define PHYS_BASE_ADDR 0x50600000U


static u32 *src = (u32*) 0x50A00000;
static u32 *dst = (u32*) 0x50A20000;
static u32 *dst_ptr = (u32*) 0x1050A20000; 


#include <inmate.h>
#include <dma_utils.h>


/**FPD-DMA (FPD = Full Power Domain), which has 8 channels, is used (called GDMA). 
Channel 0 is used**/
void inmate_main(void){
	
	DSB_ISH(); 
	ISB();

	map_range((void*)GDMA_CH0_BASE_ADDR,0x10000,MAP_UNCACHED);
	map_range((void*)CRF_APB_BASE_ADDR,0x1000,MAP_UNCACHED);
	map_range((void*)src,BUFFER_LENGTH*4,MAP_UNCACHED); //comment this line is src is located in RAM region
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

	gdma_stress_test(GDMA_CH0_BASE_ADDR,(u64)src,(u64)dst_ptr,BUFFER_LENGTH,20000);

	halt();
}




