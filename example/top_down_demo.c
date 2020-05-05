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
	long clock;
	u64 clock_begin;
	u64 clock_end;

	long slots_issued;
	u64 slots_issued_begin;
	u64 slots_issued_end;

	long slots_retired;
	u64 slots_retired_begin;
	u64 slots_retired_end;

	long fetch_bubbles;
	u64 fetch_bubbles_begin;
	u64 fetch_bubbles_end;

	long recovery_bubbles;
	u64 recovery_bubbles_begin;
	u64 recovery_bubbles_end;

	long fetch_bubbles_MIW;
	u64 fetch_bubbles_MIW_begin;
	u64 fetch_bubbles_MIW_end;

	long ms_uops;
	u64 ms_uops_begin;
	u64 ms_uops_end;

	u64 memstall_anyload;
	u64 memstall_anyload_begin;
	u64 memstall_anyload_end;

	u64 memstall_l1miss;
	u64 memstall_l1miss_begin;
	u64 memstall_l1miss_end;

	u64 memstall_l2miss;
	u64 memstall_l2miss_begin;
	u64 memstall_l2miss_end;

	//level 1 bound, all the bound can be calculate within 4 register
	long frontend_bound;
	long bad_speculation;
	long retiring;
	long bankend_bound;

	//level 2 bound, all the bound can be calculate within 4 register
	long fetch_latency_bound;
	long fetch_bandwidth_bound;
	long micro_sequencer;
	long base;

	//level 3 bound, all the bound can be calculate within 4 register
	long l1_bound;
	long l2_bound;

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

	//get recovery_bubbles
	set_pe_monitor(1, IA32_PERFEVT_INT_MISC_RECOVERY_CYCLES);
	read_pe_counter(1, &recovery_bubbles_begin);
	program_logic();
	unset_pe_monitor(1);
	read_pe_counter(1, &recovery_bubbles_end);
	recovery_bubbles = counter_delta(recovery_bubbles_begin,recovery_bubbles_end);

	//get fetch_bubbles_MIW
	set_pe_monitor(2, IA32_PERFEVT_IDQ_UOPS_0_NOT_DELIVERED_CORE);
	read_pe_counter(2, &fetch_bubbles_MIW_begin);
	program_logic();
	unset_pe_monitor(2);
	read_pe_counter(2, &fetch_bubbles_MIW_end);
	fetch_bubbles_MIW = counter_delta(fetch_bubbles_MIW_begin, fetch_bubbles_MIW_end);

	//get ms_uops
	set_pe_monitor(3, IA32_PERFEVT_IDQ_MS_UOPS);
	read_pe_counter(3, &ms_uops_begin);
	program_logic();
	unset_pe_monitor(3);
	read_pe_counter(3, &ms_uops_end);
	ms_uops = counter_delta(ms_uops_begin, ms_uops_end);

	//memstall_anyload
	set_pe_monitor(0, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_MEM_ANY);
	read_pe_counter(0, &memstall_anyload_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &memstall_anyload_end);
	memstall_anyload = counter_delta(memstall_anyload_begin, memstall_anyload_end);

	//memstall_l1miss
	set_pe_monitor(0, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L1D_MISS);
	read_pe_counter(0, &memstall_l1miss_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &memstall_l1miss_end);
	memstall_l1miss = counter_delta(memstall_l1miss_begin, memstall_l1miss_end);

	//get memstall_l2miss
	set_pe_monitor(0, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L2_MISS);
	read_pe_counter(0, &memstall_l2miss_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &memstall_l2miss_end);
	memstall_l2miss = counter_delta(memstall_l2miss_begin, memstall_l2miss_end);

	//calculate category level 1
	frontend_bound = tmam_frontend_bound(fetch_bubbles,clock);
	bad_speculation = tmam_bad_speculation(slots_issued,slots_retired,recovery_bubbles,clock);
	retiring = tmam_retiring(slots_retired,clock);
	bankend_bound = tmam_backend_bound(frontend_bound,bad_speculation,retiring);

	//calculate category level 2
	fetch_latency_bound = tmam_fetch_latency_bound(fetch_bubbles_MIW,clock);
	fetch_bandwidth_bound = tmam_fetch_bandwidth_bound(frontend_bound,fetch_latency_bound);
	micro_sequencer = tmam_micro_sequencer(ms_uops,clock);
	base = tmam_base(retiring,micro_sequencer);

	//calculate category level 3
	l1_bound = tmam_l1_bound(memstall_anyload,memstall_l1miss,clock);
	l2_bound = tmam_l2_bound(memstall_l1miss,memstall_l2miss,clock);

	printk("top-down result:\n");

	//display level 1 result in kernel log
	printk("level 1 bound: frontend_bound=%ld,bad_speculation=%ld,retiring=%ld,bankend_bound=%ld\n",frontend_bound,bad_speculation,retiring,bankend_bound);

	//display level 2 result in kernel log
	printk("level 2 bound: fetch_latency_bound=%ld,fetch_bandwidth_bound=%ld,micro_sequencer=%ld,base=%ld\n",fetch_latency_bound,fetch_bandwidth_bound,micro_sequencer,base);

	//display level 3 result in kernel log
	printk("level 3 bound: l1_bound=%ld,l2_bound=%ld\n",l1_bound,l2_bound);

	return 0;
}

static void top_down_demo_exit(void)
{
	printk("Unload Top Down demo...Bye.\n");
}

module_init(top_down_demo_init);
module_exit(top_down_demo_exit);
MODULE_LICENSE("GPL");