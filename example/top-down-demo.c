/*
 * Copyright (C) 2019 Intel Corporation
 * Author: Zhang Yang (yang6.zhang@intel.com)
 *
 * Demo of using Top-Down in kernel.
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

#include "tiny_pmu.h"

static void program_logic(void){
	int i;
	int k = 0;
	for(i=0;i<500000;i++){
		k++;
		if(k%15==3){
			k+=4;
		}
	}
	
	printk("k=%d\n",k);
}

static int top_down_demo_init(void)
{
	printk("top_down_demo_init \n");
	//directly get from register
	u64 clock;
	u64 clock_begin;
	u64 clock_end;
	
	u64 cpu_clk_unhalted_thread;
	u64 cpu_clk_unhalted_thread_begin;
	u64 cpu_clk_unhalted_thread_end;
	
	u64 total_slots;
	u64 total_slots_begin;
	u64 total_slots_end;
	
	u64 slots_issued;
	u64 slots_issued_begin;
	u64 slots_issued_end;
	
	u64 slots_retired;
	u64 slots_retired_begin;
	u64 slots_retired_end;
	
	u64 fetch_bubbles;
	u64 fetch_bubbles_begin;
	u64 fetch_bubbles_end;

	u64 fetch_bubbles_MIW;
	u64 fetch_bubbles_MIW_begin;
	u64 fetch_bubbles_MIW_end;
		
	u64 recovery_bubbles;
	u64 recovery_bubbles_begin;
	u64 recovery_bubbles_end;
	
	u64 br_mispred_retired;
	u64 br_mispred_retired_begin;
	u64 br_mispred_retired_end;
	
	u64 machine_clears_count;
	u64 machine_clears_count_begin;
	u64 machine_clears_count_end;
	
	u64 ms_uops;
	u64 ms_uops_begin;
	u64 ms_uops_end;
	
	u64 uops_issues_any;
	u64 uops_issues_any_begin;
	u64 uops_issues_any_end;
	
	u64 cycle_activity_stalls_total;
	u64 cycle_activity_stalls_total_begin;
	u64 cycle_activity_stalls_total_end;
	
	u64 rs_empty_cycles;
	u64 rs_empty_cycles_begin;
	u64 rs_empty_cycles_end;
	
	u64 cycles_ge_1_uop_exec;
	u64 cycles_ge_1_uop_exec_begin;
	u64 cycles_ge_1_uop_exec_end;

	u64 cycles_ge_2_uop_exec;
	u64 cycles_ge_2_uop_exec_begin;
	u64 cycles_ge_2_uop_exec_end;
	
	u64 memstall_anyload;
	u64 memstall_anyload_begin;
	u64 memstall_anyload_end;
	
	u64 memstall_l1miss;
	u64 memstall_l1miss_begin;
	u64 memstall_l1miss_end;

	u64 memstall_l2miss;
	u64 memstall_l2miss_begin;
	u64 memstall_l2miss_end;
	
	u64 uops_retired_llc_hit;
	u64 uops_retired_llc_hit_begin;
	u64 uops_retired_llc_hit_end;
	
	u64 uops_retired_llc_miss;
	u64 uops_retired_llc_miss_begin;
	u64 uops_retired_llc_miss_end;

	u64 memstall_stores;
	u64 memstall_stores_begin;
	u64 memstall_stores_end;
	
	u64 ext_mem_outstanding;
	u64 ext_mem_outstanding_begin;
	u64 ext_mem_outstanding_end;
	
	//count table1 event
	u64 retire_uop_fraction;
	u64 ms_slots_retired;
	u64 ops_executed;
	u64 l3_hit_fraction;
	u64 mem_stalls_l3miss;
	
	//level 1
	u64 frontend_bound;
	u64 bad_speculation;
	u64 retiring;
	u64 bankend_bound;
	
	//level2
	u64 fetch_latency_bound;
	u64 fetch_bandwidth_bound;
	u64 br_mispred_fraction;
	u64 branch_mispredicts;
	u64 machine_clear;
	u64 micro_sequencer;
	u64 base;
	u64 core_bound;
	u64 memory_bound;
	
	//level 3
	u64 l1_bound;
	u64 l2_bound;
	u64 l3_bound;
	u64 ext_memory_bound;
	u64 memstall_l3miss;
	
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
	clock = distance(clock_begin, clock_end);
	slots_issued = distance(slots_issued_begin, slots_issued_end);
	slots_retired = distance(slots_retired_begin, slots_retired_end);
	
	//get fetch_bubbles,recovery_bubbles,br_mispred_retired
	set_pe_monitor(0, IA32_PERFEVT_IDQ_UOPS_LE3_NOT_DELIVERED_CORE);
	set_pe_monitor(2, IA32_PERFEVT_BR_MISP_RETIRED_ALL_BRANCHES);
	read_pe_counter(0, &fetch_bubbles_begin);
	read_pe_counter(2, &br_mispred_retired_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(2);
	read_pe_counter(0, &fetch_bubbles_end);
	read_pe_counter(2, &br_mispred_retired_end);
	fetch_bubbles = distance(fetch_bubbles_begin, fetch_bubbles_end);
	br_mispred_retired = distance(br_mispred_retired_begin, br_mispred_retired_end);
	
	//get fetch_bubbles_MIW
	set_pe_monitor(0, IA32_PERFEVT_IDQ_UOPS_0_NOT_DELIVERED_CORE);
	set_pe_monitor(2, IA32_PERFEVT_INT_MISC_RECOVERY_CYCLES);
	read_pe_counter(0, &fetch_bubbles_MIW_begin);
	read_pe_counter(2, &recovery_bubbles_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(2);
	read_pe_counter(0, &fetch_bubbles_MIW_end);
	read_pe_counter(2, &recovery_bubbles_end);
	fetch_bubbles_MIW = distance(fetch_bubbles_MIW_begin, fetch_bubbles_MIW_end);
	recovery_bubbles = distance(recovery_bubbles_begin,recovery_bubbles_end);
	total_slots = clock * 4;
	
	//get machine_clears_count,ms_uops,uops_issues_any
	set_pe_monitor(0, IA32_PERFEVT_MACHINE_CLEARS_COUNT);
	set_pe_monitor(1, IA32_PERFEVT_IDQ_MS_UOPS);
	set_pe_monitor(2, IA32_PERFEVT_UOPS_ISSUED_ANY);
	read_pe_counter(0, &machine_clears_count_begin);
	read_pe_counter(1, &ms_uops_begin);
	read_pe_counter(2, &uops_issues_any_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(1);
	unset_pe_monitor(2);
	read_pe_counter(0, &machine_clears_count_end);
	read_pe_counter(1, &ms_uops_end);
	read_pe_counter(2, &uops_issues_any_end);
	machine_clears_count = distance(machine_clears_count_begin, machine_clears_count_end);
	ms_uops = distance(ms_uops_begin, ms_uops_end);
	uops_issues_any = distance(uops_issues_any_begin, uops_issues_any_end);
	
	//get cycle_activity_stalls_total,rs_empty_cycles,cycles_ge_1_uop_exec
	set_pe_monitor(0, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_TOTAL);
	set_pe_monitor(1, IA32_PERFEVT_RS_EVENTS_EMPTY_CYCLES);
	set_pe_monitor(2, IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_1_UOP_EXEC);
	read_pe_counter(0, &cycle_activity_stalls_total_begin);
	read_pe_counter(1, &rs_empty_cycles_begin);
	read_pe_counter(2, &cycles_ge_1_uop_exec_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(1);
	unset_pe_monitor(2);
	read_pe_counter(0, &cycle_activity_stalls_total_end);
	read_pe_counter(1, &rs_empty_cycles_end);
	read_pe_counter(2, &cycles_ge_1_uop_exec_end);
	cycle_activity_stalls_total = distance(cycle_activity_stalls_total_begin, cycle_activity_stalls_total_end);
	rs_empty_cycles = distance(rs_empty_cycles_begin, rs_empty_cycles_end);
	cycles_ge_1_uop_exec = distance(cycles_ge_1_uop_exec_begin, cycles_ge_1_uop_exec_end);
	
	//get cycles_ge_2_uop_exec,memstall_anyload,memstall_l1miss
	set_pe_monitor(0, IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_2_UOP_EXEC);
	set_pe_monitor(1, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_MEM_ANY);
	set_pe_monitor(2, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L1D_MISS);
	read_pe_counter(0, &cycles_ge_2_uop_exec_begin);
	read_pe_counter(1, &memstall_anyload_begin);
	read_pe_counter(2, &memstall_l1miss_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(1);
	unset_pe_monitor(2);
	read_pe_counter(0, &cycles_ge_2_uop_exec_end);
	read_pe_counter(1, &memstall_anyload_end);
	read_pe_counter(2, &memstall_l1miss_end);
	cycles_ge_2_uop_exec = distance(cycles_ge_2_uop_exec_begin, cycles_ge_2_uop_exec_end);
	memstall_anyload = distance(memstall_anyload_begin, memstall_anyload_end);
	memstall_l1miss = distance(memstall_l1miss_begin, memstall_l1miss_end);	
	
	//get memstall_l2miss,uops_retired_llc_hit,uops_retired_llc_miss
	set_pe_monitor(0, IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L2_MISS);
	set_pe_monitor(1, IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_HIT);
	set_pe_monitor(2, IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_MISS);
	read_pe_counter(0, &memstall_l2miss_begin);
	read_pe_counter(1, &uops_retired_llc_hit_begin);
	read_pe_counter(2, &uops_retired_llc_miss_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(1);
	unset_pe_monitor(2);
	read_pe_counter(0, &memstall_l2miss_end);
	read_pe_counter(1, &uops_retired_llc_hit_end);
	read_pe_counter(2, &uops_retired_llc_miss_end);
	memstall_l2miss = distance(memstall_l2miss_begin, memstall_l2miss_end);
	uops_retired_llc_hit = distance(uops_retired_llc_hit_begin, uops_retired_llc_hit_end);
	uops_retired_llc_miss = distance(uops_retired_llc_miss_begin, uops_retired_llc_miss_end);
	
	//get memstall_stores,ext_mem_outstanding
	set_pe_monitor(0, IA32_PERFEVT_RESOURCE_STALLS_SB);
	set_pe_monitor(1, IA32_PERFEVT_UNC_ARB_TRK_OCCUPANCY_ALL);
	read_pe_counter(0, &memstall_stores_begin);
	read_pe_counter(1, &ext_mem_outstanding_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(1);
	read_pe_counter(0, &memstall_stores_end);
	read_pe_counter(1, &ext_mem_outstanding_end);
	memstall_stores = distance(memstall_stores_begin, memstall_stores_end);
	ext_mem_outstanding = distance(ext_mem_outstanding_begin, ext_mem_outstanding_end);
	
	//calculte base event
	retire_uop_fraction = 1000 * slots_retired / uops_issues_any;
	ms_slots_retired = retire_uop_fraction * ms_uops / 1000;
	ops_executed = cycle_activity_stalls_total - rs_empty_cycles + cycles_ge_1_uop_exec - cycles_ge_2_uop_exec;
	l3_hit_fraction = 1000 * uops_retired_llc_hit / (uops_retired_llc_hit + uops_retired_llc_miss);
	mem_stalls_l3miss = (1000 - l3_hit_fraction) * memstall_l2miss;
	
	//calculate category level 1
	frontend_bound=	1000 * fetch_bubbles / total_slots;
	bad_speculation = 1000 * (slots_issued-slots_retired + recovery_bubbles) / total_slots;
	retiring = 1000 * slots_retired / total_slots;
	bankend_bound = 1000-(frontend_bound + bad_speculation + retiring);
	
	//calculate category level 2
	fetch_latency_bound = 1000 * fetch_bubbles_MIW / total_slots;
	fetch_bandwidth_bound = frontend_bound - fetch_latency_bound;
	br_mispred_fraction = 1000 * br_mispred_retired / (br_mispred_retired + machine_clears_count);
	branch_mispredicts = bad_speculation * br_mispred_fraction / 1000;
	machine_clear = bad_speculation - branch_mispredicts;
	micro_sequencer = 1000 * ms_uops / total_slots;
	base = retiring - micro_sequencer;
	memory_bound = 1000 * (memstall_anyload + memstall_stores) / clock;
	core_bound = 1000 * ops_executed / clock - memory_bound;
	
	//calculate category level 3
	l1_bound = 1000 * (memstall_anyload - memstall_l1miss) / clock;
	l2_bound = 1000 * (memstall_l1miss - memstall_l2miss) / clock;
	l3_bound = 1000 * (memstall_l2miss - mem_stalls_l3miss) / clock;
	ext_memory_bound = 1000 * mem_stalls_l3miss / clock;
	
	printk("memstall_anyload=%llx\n",memstall_anyload);
	printk("memstall_l1miss=%llx\n",memstall_l1miss);
	printk("memstall_l2miss=%llx\n",memstall_l2miss);
	printk("mem_stalls_l3miss=%llx\n",mem_stalls_l3miss);
	
	
	printk("fetch_bubbles=%llx, total_slots=%llx, slots_issued=%llx, slots_retired=%llx, recovery_bubbles=%llx\n", fetch_bubbles,total_slots,slots_issued,slots_retired, recovery_bubbles);
	printk("frontend_bound=%d,bad_speculation=%d,retiring=%d,bankend_bound=%d\n",frontend_bound,bad_speculation,retiring,bankend_bound);
	printk("fetch_latency_bound=%llx,fetch_bandwidth_bound=%llx\n",fetch_latency_bound,fetch_bandwidth_bound);
	printk("br_mispred_fraction=%llx,branch_mispredicts=%llx\n",br_mispred_fraction,branch_mispredicts);	
	printk("machine_clear=%llx,micro_sequencer=%llx\n",machine_clear,micro_sequencer);	
	printk("base=%llx,memory_bound=%llx,core_bound=%llx\n",base,memory_bound,core_bound);
	printk("l1_bound=%llx\n",l1_bound);
	printk("l2_bound=%llx\n",l2_bound);
	printk("l3_bound=%llx\n",l3_bound);
	printk("ext_memory_bound=%llx\n",ext_memory_bound);
	return 0;
}

static void top_down_demo_exit(void)
{
	printk("Unload Top Down demo...Bye.\n");
}

module_init(top_down_demo_init);
module_exit(top_down_demo_exit);
MODULE_LICENSE("GPL");
