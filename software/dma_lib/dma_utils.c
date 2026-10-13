
#include <inmate.h>
#include <dma_utils.h>

/* Function to perform CLEAN, INVALIDATE and CLEAN+INVALIDATE operations on the data cache up to the point of coherency */
void dcache_flush_range(void *addr, u64 size, int choice){

        u64 ctr;
        u64 dsize,dline,end;

    //read the CTR_EL0 control register to obtain the data cache line length
 	asm volatile("mrs %[ctr], ctr_el0\n\t" : [ctr] "=r" (ctr));
	
	//data cache line size in bytes (computed from the DminLine[19:16] field of the control register)
	dsize = 4 << ((ctr >> 16) & 0xf);
	
	dline = (u64)addr & ~(dsize-1);//arrotondamento verso il basso, rendo l'indirizzo divisibile per dsize (mettendo a 0, dsize-1 bits)

	end = (dline + size + dsize-1) & ~(dsize-1); //arrotondamento verso l'alto, soprattutto se end non è divisibile per dsize
	
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
	
	DSB_ISH();//dsb(ish)
	ISB();//isb
}



/**Funzione per configurare un trasferimento di tipo simple mode con l'FPD-DMA (o GDMA) della ZCU104**/
void gdma_config_simple_mode(u64 GDMA_CH_BASE_ADDR,u64 src_addr,u64 dst_addr,u32 length){ 

	volatile u32* ptr = (volatile u32* ) (GDMA_CH_BASE_ADDR + GDMA_CH_STATUS_OFFS) ;

	printk("GDMA_CH_STATUS (BEFORE WHILE) = %u\n",*ptr);

	//attendo fino a che non si verifica che ZDMA_CH_STATUS[1:0] diventi 00 (done with no error) o diventi 11 (done with error)
	while((*ptr & DONE_WITH_ERROR )!=DONE_WITH_ERROR  && (*ptr & DONE_WITH_ERROR )!=DONE_WITH_NO_ERROR);

	printk("GDMA_CH_STATUS (AFTER WHILE) = %u\n",*ptr);

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_CTR0_OFFS);

	*ptr &= ~(1 << 6); //SIMPLE MODE DMA (POINT_TYPE = 0)

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_SRC_DSCR_WORD0_OFFS);

	*ptr = (u32) (src_addr & 0xFFFFFFFF); //parte bassa indirizzo src (lower 32-bit of address)

	printk("prima parte indirizzo src = %u\n",*ptr);

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_SRC_DSCR_WORD1_OFFS);

	*ptr = (u32) ((src_addr >> 32) & 0xFFF); //parte alta indirizzo src (upper 12-bits of src address), GDMA_CH_SRC_DSCR_WORD1_ADDR[31:17] are reserved for future use

	printk("seconda parte indirizzo src = %u\n",*ptr);

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_DST_DSCR_WORD0_OFFS);

	*ptr = (u32) (dst_addr & 0xFFFFFFFF); //parte bassa indirizzo dst (lower 32-bit of address)
	
	printk("prima parte indirizzo dst = %u\n",*ptr);

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_DST_DSCR_WORD1_OFFS);

	*ptr = (u32) ((dst_addr >> 32) & 0xFFF); //parte alta indirizzo dst (upper 12 bits of dst address), GDMA_CH_DST_DSCR_WORD1_ADDR[31:17] are reserved for future use
	
	printk("seconda parte indirizzo dst = %u\n",*ptr);

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_SRC_DSCR_WORD2_OFFS);

	*ptr = 4 * length; //src transfer size (in bytes nel nostro caso è 32)

	ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_DST_DSCR_WORD2_OFFS);

	*ptr = 4 * length; //dst transfer size (in bytes nel nostro caso è 32)

	//ptr = (volatile u32*) GDMA_CH_DATA_ATTR_ADDR; //caratteristiche delle transazioni AXI per letture e scritture di dati durante il trasferimento DMA.
	
	//printk("GDMA_CH_DATA_ATTR = %u\n",*ptr); //ci deve essere 0x0483D20F come valore di default in decimale 75.747.855

	DSB_ISH(); //dsb(ish) : Blocca l'esecuzione fino a che tutte le operazioni di memoria precedenti siano completate.
	ISB();//isb : Svuota la pipeline del processore 
}


/**funzione per startare il transferimento con l'fpd-dma**/
void gdma_start_transfer(u64 GDMA_CH_BASE_ADDR){

	volatile u32 *ctrl2_ptr = (volatile u32*) (GDMA_CH_BASE_ADDR + GDMA_CH_CTRL2_OFFS);
	
	*ctrl2_ptr |= (1U << 0); //abilitazione del trasferimento.
	
	//printk("GDMA_CH_CTRL2 = %u\n",*ptr);
}


/**funzione per attendere (polling) che il trasferimento termini***/
int gdma_poll_for_conversion(u64 GDMA_CH_BASE_ADDR){
	
	volatile u32 *status_ptr = (volatile u32* ) (GDMA_CH_BASE_ADDR + GDMA_CH_STATUS_OFFS);
	
	//printk("GDMA_CH_STATUS (BEFORE WHILE) = %u\n",*ptr);

	while((*status_ptr & DONE_WITH_ERROR )!=DONE_WITH_ERROR  && (*status_ptr & DONE_WITH_ERROR )!=DONE_WITH_NO_ERROR);
	
	//printk("GDMA_CH_STATUS (AFTER WHILE) = %u\n",*ptr);

	if((*status_ptr & DONE_WITH_ERROR) == DONE_WITH_NO_ERROR){
		//printk("------------DONE WITH NO ERROR-------------\n");
		//ptr = (volatile u32*) GDMA_CH_CTRL2_ADDR;
		//printk("GDMA_CH_CTRL2 = %u\n",*ptr);
		return 0;
	}else{
		//printk("------------DONE WITH ERROR----------\n");
		return -1;
	}
}

/*Funzione per avviare un certo numero di trasferimenti con il DMA verso l'FPGA***/
void gdma_stress_test(u64 GDMA_CH_BASE_ADDR,u64 src_addr,u64 dst_addr,u32 length,u32 ripetitions){

	gdma_config_simple_mode(GDMA_CH_BASE_ADDR,src_addr,dst_addr,length);

	int ret_value=0;

	u64 start_time = timer_get_ticks();
	
	int j=0;
	while(j<ripetitions && ret_value != -1){
		gdma_start_transfer(GDMA_CH_BASE_ADDR);
		ret_value = gdma_poll_for_conversion(GDMA_CH_BASE_ADDR);
		j++;
	}

	if(ret_value != -1){

		u64 end_time = timer_get_ticks();

		u64 diff = timer_ticks_to_ns(end_time - start_time); //converto da tick in nanosecondi
	
		printk("Time required = %llu ns\n ",diff);

		printk("------------DONE WITH NO ERROR-------------\n");

	}else{
		printk("------------DONE WITH ERROR-------------\n");

	}

}

/**Funzione per fare length scritture a partire dall'indirizzo dst_addr***/
void cpu_writes(u32 *dst,u32 length){

	u64 start_time = timer_get_ticks();

  	int i=0;
	for(i=0;i<length;i++){
		dst[i] = i+1;
	}
	
	u64 end_time = timer_get_ticks();

	u64 diff = end_time - start_time;
	
	printk("Time required = %llu ns\n",timer_ticks_to_ns(diff));
}


/**Funzione per accendere il clock per l'FPD-DMA****/
void power_on_fpd_dma(void){

 	volatile u32* ptr = (volatile u32*)FPD_DMA_REF_CTRL_ADDR;	

	*ptr |= (1U << 24); //CLKACT  = 1 (clock active for FPD-DMA)

	printk("FPD_DMA_REF_CTRL : CLKACT = %u\n",*ptr);
}


