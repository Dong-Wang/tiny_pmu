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

	u64 inst_retired_any;
	u64 inst_retired_any_begin;
	u64 inst_retired_any_end;

	u64 exe_activity_bound_on_stores;
	u64 exe_activity_bound_on_stores_begin;
	u64 exe_activity_bound_on_stores_end;

	u64 exe_activity_1_ports_util;
	u64 exe_activity_1_ports_util_begin;
	u64 exe_activity_1_ports_util_end;

	u64 exe_activity_2_ports_util;
	u64 exe_activity_2_ports_util_begin;
	u64 exe_activity_2_ports_util_end;

	u64 exe_activity_exe_bound_0_ports;
	u64 exe_activity_exe_bound_0_ports_begin;
	u64 exe_activity_exe_bound_0_ports_end;

	//base event, which not directly get from pmu
	u64 total_slots;
	u64 retire_uop_fraction;
	u64 ms_slots_retired;
	u64 ops_executed;
	u64 l3_hit_fraction;
	u64 mem_stalls_l3miss;

	//level 1 event
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
	set_pe_monitor(2, IA32_PERFEVT_BR_MISP_RETIRED_ALL_BRANCHES);
	read_pe_counter(0, &fetch_bubbles_begin);
	read_pe_counter(2, &br_mispred_retired_begin);
	program_logic();
	unset_pe_monitor(0);
	unset_pe_monitor(2);
	read_pe_counter(0, &fetch_bubbles_end);
	read_pe_counter(2, &br_mispred_retired_end);
	fetch_bubbles = counter_delta(fetch_bubbles_begin, fetch_bubbles_end);
	br_mispred_retired = counter_delta(br_mispred_retired_begin, br_mispred_retired_end);

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
	fetch_bubbles_MIW = counter_delta(fetch_bubbles_MIW_begin, fetch_bubbles_MIW_end);
	recovery_bubbles = counter_delta(recovery_bubbles_begin,recovery_bubbles_end);
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
	machine_clears_count = counter_delta(machine_clears_count_begin, machine_clears_count_end);
	ms_uops = counter_delta(ms_uops_begin, ms_uops_end);
	uops_issues_any = counter_delta(uops_issues_any_begin, uops_issues_any_end);

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
	cycle_activity_stalls_total = counter_delta(cycle_activity_stalls_total_begin, cycle_activity_stalls_total_end);
	rs_empty_cycles = counter_delta(rs_empty_cycles_begin, rs_empty_cycles_end);
	cycles_ge_1_uop_exec = counter_delta(cycles_ge_1_uop_exec_begin, cycles_ge_1_uop_exec_end);

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
	cycles_ge_2_uop_exec = counter_delta(cycles_ge_2_uop_exec_begin, cycles_ge_2_uop_exec_end);
	memstall_anyload = counter_delta(memstall_anyload_begin, memstall_anyload_end);
	memstall_l1miss = counter_delta(memstall_l1miss_begin, memstall_l1miss_end);

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
	memstall_l2miss = counter_delta(memstall_l2miss_begin, memstall_l2miss_end);
	uops_retired_llc_hit = counter_delta(uops_retired_llc_hit_begin, uops_retired_llc_hit_end);
	uops_retired_llc_miss = counter_delta(uops_retired_llc_miss_begin, uops_retired_llc_miss_end);

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
	memstall_stores = counter_delta(memstall_stores_begin, memstall_stores_end);
	ext_mem_outstanding = counter_delta(ext_mem_outstanding_begin, ext_mem_outstanding_end);

	//get exe_activity_bound_on_stores
	set_pe_monitor(0, IA32_PERFEVT_EXE_ACTIVITY_BOUND_ON_STORES);
	read_pe_counter(0, &exe_activity_bound_on_stores_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &exe_activity_bound_on_stores_end);
	exe_activity_bound_on_stores = counter_delta(exe_activity_bound_on_stores_begin, exe_activity_bound_on_stores_end);

	//get exe_activity_1_ports_util
	set_pe_monitor(0, IA32_PERFEVT_EXE_ACTIVITY_1_PORTS_UTIL);
	read_pe_counter(0, &exe_activity_1_ports_util_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &exe_activity_1_ports_util_end);
	exe_activity_1_ports_util = counter_delta(exe_activity_1_ports_util_begin, exe_activity_1_ports_util_end);

	//get exe_activity_2_ports_util
	set_pe_monitor(0, IA32_PERFEVT_EXE_ACTIVITY_2_PORTS_UTIL);
	read_pe_counter(0, &exe_activity_2_ports_util_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &exe_activity_2_ports_util_end);
	exe_activity_2_ports_util = counter_delta(exe_activity_2_ports_util_begin, exe_activity_2_ports_util_end);

	//get exe_activity_exe_bound_0_ports
	set_pe_monitor(0, IA32_PERFEVT_EXE_ACTIVITY_EXE_BOUND_0_PORTS);
	read_pe_counter(0, &exe_activity_exe_bound_0_ports_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &exe_activity_exe_bound_0_ports_end);
	exe_activity_exe_bound_0_ports = counter_delta(exe_activity_exe_bound_0_ports_begin, exe_activity_exe_bound_0_ports_end);

	//get inst_retired_any
	set_pe_monitor(0, IA32_PERFEVT_INST_RETIRED_ANY_P);
	read_pe_counter(0, &inst_retired_any_begin);
	program_logic();
	unset_pe_monitor(0);
	read_pe_counter(0, &inst_retired_any_end);
	inst_retired_any = counter_delta(inst_retired_any_begin, inst_retired_any_end);

	//calculte base event
	retire_uop_fraction = TMAM_MATRIC_RESOLUTION * slots_retired / uops_issues_any;
	ms_slots_retired = retire_uop_fraction * ms_uops / TMAM_MATRIC_RESOLUTION;
	ops_executed = cycle_activity_stalls_total - rs_empty_cycles + cycles_ge_1_uop_exec - cycles_ge_2_uop_exec;
	l3_hit_fraction = TMAM_MATRIC_RESOLUTION * uops_retired_llc_hit / (uops_retired_llc_hit + uops_retired_llc_miss);
	mem_stalls_l3miss = (TMAM_MATRIC_RESOLUTION - l3_hit_fraction) * memstall_l2miss;

	//calculate category level 1
	frontend_bound=	TMAM_MATRIC_RESOLUTION * TMAM_MATRIC_THREADS * fetch_bubbles / total_slots;
	bad_speculation = TMAM_MATRIC_RESOLUTION * (slots_issued-slots_retired + (4 * recovery_bubbles)/TMAM_MATRIC_THREADS) / (total_slots / TMAM_MATRIC_THREADS);
	retiring = TMAM_MATRIC_RESOLUTION * slots_retired / (total_slots / TMAM_MATRIC_THREADS);
	bankend_bound = TMAM_MATRIC_RESOLUTION-(frontend_bound + bad_speculation + retiring);

	//calculate category level 2
	fetch_latency_bound = TMAM_MATRIC_RESOLUTION * fetch_bubbles_MIW / ( clock / TMAM_MATRIC_THREADS );
	fetch_bandwidth_bound = TMAM_MATRIC_RESOLUTION * (fetch_bubbles - 4 * fetch_bubbles_MIW)/ ( total_slots / TMAM_MATRIC_THREADS );
	br_mispred_fraction = TMAM_MATRIC_RESOLUTION * br_mispred_retired / (br_mispred_retired + machine_clears_count);
	branch_mispredicts = bad_speculation * br_mispred_fraction / TMAM_MATRIC_RESOLUTION;
	machine_clear = bad_speculation - branch_mispredicts;
	micro_sequencer = TMAM_MATRIC_RESOLUTION * ms_uops / total_slots;
	base = retiring - micro_sequencer;

	memory_bound = tmam_memory_bound(fetch_bubbles,slots_issued,recovery_bubbles,
		clock,slots_retired,memstall_anyload,exe_activity_bound_on_stores,
		exe_activity_1_ports_util,exe_activity_2_ports_util,exe_activity_exe_bound_0_ports,
		clock,slots_issued);
	core_bound = tmam_core_bound(fetch_bubbles,slots_issued,recovery_bubbles,
		clock,slots_retired,memstall_anyload,exe_activity_bound_on_stores,
		exe_activity_1_ports_util,exe_activity_2_ports_util,
		exe_activity_exe_bound_0_ports,clock,slots_issued);

	//display result in dmesg
	printk("frontend_bound=%lld,bad_speculation=%lld,retiring=%lld,bankend_bound=%lld\n",frontend_bound,bad_speculation,retiring,bankend_bound);
	printk("####################################################");
	printk("frontend_bound include fetch_latency_bound and fetch_bandwidth_bound:");
	printk("fetch_latency_bound=%lld,fetch_bandwidth_bound=%lld\n",fetch_latency_bound,fetch_bandwidth_bound);
	printk("####################################################");
	printk("bad_speculation include branch_mispredicts and machine_clear:");
	printk("branch_mispredicts=%lld,machine_clear=%lld\n",branch_mispredicts,machine_clear);	
	printk("####################################################");
	printk("retiring include micro_sequencer and base:");
	printk("micro_sequencer=%lld,base=%lld\n",micro_sequencer,base);
	printk("####################################################");
	printk("bankend_bound include memory_bound and core_bound:");
	printk("memory_bound=%lld,core_bound=%lld\n",memory_bound,core_bound);
	return 0;
}

static void top_down_demo_exit(void)
{
	printk("Unload Top Down demo...Bye.\n");
}

module_init(top_down_demo_init);
module_exit(top_down_demo_exit);
MODULE_LICENSE("GPL");