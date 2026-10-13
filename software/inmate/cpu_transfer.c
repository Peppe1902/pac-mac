#include <inmate.h>
#include <dma_utils.h>

/*-----------------------------UTILS--------------------------------------*/
#define BUFFER_LENGTH 2500
#define DONE_WITH_ERROR 0x00000003U
#define DONE_WITH_NO_ERROR 0x00000000U
#define PHYS_BASE_ADDR 0x50600000U


static u32 *src = (u32*) 0x50A00000;
static u32 *dst = (u32*) 0x50A20000;
static u32 *dst_ptr = (u32*) 0x1050A20000; 


/**Usiamo FPD-DMA (indicato con GDMA che ha 8 canali noi useremo il primo CH0) -> (FPD = Full Power Domain)**/
void inmate_main(void){
	
	DSB_ISH(); 
	ISB();

	map_range((void*)src,BUFFER_LENGTH*4,MAP_UNCACHED); //commentarlo se src è in RAM
	map_range((void*)dst,BUFFER_LENGTH*4,MAP_UNCACHED);
	map_range((void*)dst_ptr,BUFFER_LENGTH*4,MAP_UNCACHED);

	DSB_ISH(); 
	ISB();

	memset((void*)dst,0,BUFFER_LENGTH*4);

	cpu_writes(dst,BUFFER_LENGTH);
	
	//for(int i=0;i<BUFFER_LENGTH;i++){
		//src[i] = i+1;
		//printk("written src[%d] = %d\n",i,src[i]);
	//}	
	
	for(int i=0;i<10;i++){
		printk("dst[%d] = %d\n",i,dst[i]);
	}	

	DSB_ISH(); 
	ISB();


	halt();
}
