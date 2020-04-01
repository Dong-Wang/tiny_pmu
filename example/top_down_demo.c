/*
 * Copyright (C) 2020 Intel Corporation
 *
 * Demo of using Top-Down in kernel.
 * Only support Skylake.
 */
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/notifier.h>
#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <asm/cpufeature.h>
#include <asm/msr-index.h>
#include <asm/processor.h>
#include <asm/msr.h>
#include <linux/vmalloc.h>

#include "tiny_pmu.h"

/*
 * program logic example
 * add vmalloc to generate memory bound
 * add some if statement to generate bad speculation
 */
static void program_logic(void){
	int i;
	int k = 0;
	char * memory_ptr = vmalloc(1024);
	for(i=0;i<1024;i++){
		k++;
		if(k%15==3){
			k+=4;
		}else{
			k+=1;
		}
		memory_ptr[i] = 'a';
	}
	vfree(memory_ptr);
}

static int top_down_demo_init(void)
{
	//directly get from PMU register
	u64 clock;
	u64 clock_begin;
	u64 clock_end;

	u64 slots_issued;
	u64 slots_issued_begin;
	u64 slots_issued_end;

	u64 slots_retired;
	u64 slots_retired_begin;
	u64 slots_retired_end;

	u64 fetch_bubbles;
	u64 fetch_bubbles_begin;
	u64 fetch_bubbles_end;

	u64 recovery_bubbles;
	u64 recovery_bubbles_begin;
	u64 recovery_bubbles_end;

	//base event, which not directly get from pmu
	u64 total_slots;

	//level 1 bound
	u64 frontend_bound;
	u64 bad_speculation;
	u64 retiring;
	u64 bankend_bound;

	printk("Load Top Down demo.\n");
	printk("Begin using top-down event to analyze demo program.\n");

	unset_pe_monitor(0);
	unset_pe_monitor(1);
	unset_pe_monitor(2);

	//get total_slots, slots_issued, slots_retired
	set_pe_monitor(0, IA32_PERFEVT_CPU_CLK_UNHALTED_THREAD);
	set_pe_monitor(1, IA32_PERFEVT_UOPS_ISSUED_ANY);
	set_pe_monitor(2, IA32_PERFEVT_UOPS_RETIRED_RETIRED_SLOTS);
	read_pe_counter(0, &clock_begin);
	read_pe_counter(1, &slots_issued_begin);
	read_pe_counter(2, &slots_retired_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(1);
	unset_pe_monitor(2);
	read_pe_counter(0, &clock_end);
	read_pe_counter(1, &slots_issued_end);
	read_pe_counter(2, &slots_retired_end);
	clock = counter_delta(clock_begin, clock_end);
	slots_issued = counter_delta(slots_issued_begin, slots_issued_end);
	slots_retired = counter_delta(slots_retired_begin, slots_retired_end);

	//get fetch_bubbles,recovery_bubbles,br_mispred_retired
	set_pe_monitor(0, IA32_PERFEVT_IDQ_UOPS_LE3_NOT_DELIVERED_CORE);
	read_pe_counter(0, &fetch_bubbles_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &fetch_bubbles_end);
	fetch_bubbles = counter_delta(fetch_bubbles_begin, fetch_bubbles_end);

	//get fetch_bubbles_MIW
	set_pe_monitor(2, IA32_PERFEVT_INT_MISC_RECOVERY_CYCLES);
	read_pe_counter(2, &recovery_bubbles_begin);
	program_logic();
	unset_pe_monitor(2);
	read_pe_counter(2, &recovery_bubbles_end);
	recovery_bubbles = counter_delta(recovery_bubbles_begin,recovery_bubbles_end);
	total_slots = clock * 4;

	//calculate category level 1
	frontend_bound=	TMAM_MATRIC_RESOLUTION * TMAM_MATRIC_THREADS * fetch_bubbles / total_slots;
	bad_speculation = TMAM_MATRIC_RESOLUTION * (slots_issued-slots_retired + (4 * recovery_bubbles)/TMAM_MATRIC_THREADS) / (total_slots / TMAM_MATRIC_THREADS);
	retiring = TMAM_MATRIC_RESOLUTION * slots_retired / (total_slots / TMAM_MATRIC_THREADS);
	bankend_bound = TMAM_MATRIC_RESOLUTION-(frontend_bound + bad_speculation + retiring);

	//display result in dmesg
	printk("frontend_bound=%lld,bad_speculation=%lld,retiring=%lld,bankend_bound=%lld\n",frontend_bound,bad_speculation,retiring,bankend_bound);
	return 0;
}

static void top_down_demo_exit(void)
{
	printk("Unload Top Down demo...Bye.\n");
}

module_init(top_down_demo_init);
module_exit(top_down_demo_exit);
MODULE_LICENSE("GPL");