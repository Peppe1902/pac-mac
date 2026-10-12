

#include <inmate.h>

/*---------------------------FPD-DMA REGISTERS-----------------------------------*/
#define GDMA_CH_STATUS_OFFS		(0x0000011CU)
#define GDMA_CH_CTR0_OFFS		(0x00000110U)

#define GDMA_CH_SRC_DSCR_WORD0_OFFS	(0x00000128U)
#define GDMA_CH_SRC_DSCR_WORD1_OFFS 	(0x0000012CU)
#define GDMA_CH_SRC_DSCR_WORD2_OFFS 	(0x00000130U)

#define GDMA_CH_DST_DSCR_WORD0_OFFS 	(0x00000138U)
#define GDMA_CH_DST_DSCR_WORD1_OFFS 	(0x0000013CU)
#define GDMA_CH_DST_DSCR_WORD2_OFFS 	(0x00000140U)

#define GDMA_CH_DATA_ATTR_OFFS		(0x00000120U)
#define GDMA_CH_CTRL2_OFFS 		(0x00000200U)
#define GDMA_CH_ISR_OFFS		(0x00000100U)

/*-----------------------------ADDRESSES OF CRF_APB REGISTERS----------------------------------*/
#define CRF_APB_BASE_ADDR		0xFD1A0000
#define FPD_DMA_REF_CTRL_ADDR		(CRF_APB_BASE_ADDR + 0x000000B8U)



/*-----------------------------UTILS--------------------------------------*/
#define DONE_WITH_ERROR 0x00000003U
#define DONE_WITH_NO_ERROR 0x00000000U


#define DSB_ISH() asm volatile("dsb ish" ::: "memory")
#define DSB_SY() asm volatile("dsb sy" ::: "memory")
#define ISB()	asm volatile("isb" ::: "memory");

#define DCACHE_CLEAN		0
#define DCACHE_INVALIDATE	1
#define DCACHE_CLEAN_INVALIDATE	2

/*---------------------------FUNCTIONS PROTOTYPES--------------------------*/
void gdma_config_simple_mode(u64 GDMA_CH_BASE_ADDR,u64 src_addr,u64  dst_addr,u32 length);
void gdma_start_transfer(u64 GDMA_CH_BASE_ADDR);
void gdma_start_transfer(u64 GDMA_CH_BASE_ADDR);
int gdma_poll_for_conversion(u64 GDMA_CH_BASE_ADDR);
void gdma_stress_test(u64 GDMA_CH_BASE_ADDR,u64 src_addr,u64 dst_addr,u32 length,u32 ripetitions);
void cpu_writes(u32 *dst,u32 ripetitions);
void dcache_flush_range(void *addr, u64 size, int choice);
void power_on_fpd_dma(void);








