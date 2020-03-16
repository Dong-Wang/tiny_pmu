/*
 * Copyright (C) 2019 Intel Corporation
 * Author: Wang Dong (dong1.wang@intel.com)
 *
 * Tiny APIs to access PMU.
 */
#ifndef __TINY_PMU_H_
#define __TINY_PMU_H_

#include <asm/msr.h>

/* 
 * Different CPU support different number of general-purpose permance monitoring counter
 * Please check how many counters your CPU support by CPUID.0Ah[15:08] for non-hyperthreading
 * and [23:16] for hyperthreading
 * At least, you can use 2 counters without checking CPUID.
 */
#define IA32_PERFEVTSEL0 0x186
#define IA32_PERFEVTSEL1 0x187
#define IA32_PERFEVTSEL2 0x188
#define IA32_PERFEVTSEL3 0x189
#define IA32_PERFEVTSEL4 0x18a
#define IA32_PERFEVTSEL5 0x18b
#define IA32_PERFEVTSEL6 0x18c
#define IA32_PERFEVTSEL7 0x18d
#define IA32_PERFCTR0 0xc1
#define IA32_PERFCTR1 0xc2
#define IA32_PERFCTR2 0xc3
#define IA32_PERFCTR3 0xc4
#define IA32_PERFCTR4 0xc5
#define IA32_PERFCTR5 0xc6
#define IA32_PERFCTR6 0xc7
#define IA32_PERFCTR7 0xc8

/*
 * Detailed description in <Intel 64 and IA-32 Architectures Software Developer's Manual>, Document ID 325462
 * 18.2.1.2 Pre-defined Architectural Performance Events
 */
#define IA32_PERFEVT_UNHALTCORECYCLE	0x0053003c  /* UnHalted Core Cycles */
#define IA32_PERFEVT_INST_RETIRE 	0x005300c0  /* Instructions Retired*/
#define IA32_PERFEVT_UNHALTREFCYCLE	0x0053013c  /* UnHalted Reference Cycles */
#define IA32_PERFEVT_LLC_REF     	0x00534f2e  /* LLC Reference */
#define IA32_PERFEVT_LLC_MISS    	0x0053412e  /* LLC Misses */
#define IA32_PERFEVT_BRANCH_HITS 	0x005300c4  /* Branch Instruction Retired */
#define IA32_PERFEVT_BRANCH_MISS 	0x005300c5  /* Branch Misses Retired */
#define IA32_PERFEVT_TOPDOWN_SLOTS 	0x005301a4  /* Topdown Slots */

/*
 * Intel CPU have lots of methods to monitor performance counter
 * In this demo, we use IA32_PERFEVTSELx MSR register, 2 register in total.
 * So we can monitor 2 events at one time.
 */

/*
 * Setup performance event to monitor
 * event_id: u64, IA32_PERFEVT_x
 * return value: 0 for success, other for failed.
 */
#define set_pe_monitor_0(event_id)  \
	wrmsr_safe((IA32_PERFEVTSEL0), (u32)((u64)(event_id)), (u32)((u64)(event_id) >> 32))

/*
 * Remove performance event when you don't monitor it
 * return value: 0 for success, other for failed.
 */
#define unset_pe_monitor_0()     \
	wrmsr_safe((IA32_PERFEVTSEL0), 0, 0)

/*
 * Read event counter
 * counter: (u64 *)
 * return value: 0 for success, other for failed.
 */
#define read_pe_counter_0(counter)	\
	rdmsrl_safe((IA32_PERFCTR0), (counter))

/*
 * Refer to set_pe_monitor_0()
 */
#define set_pe_monitor_1(event_id)  \
	wrmsr_safe((IA32_PERFEVTSEL1), (u32)((u64)(event_id)), (u32)((u64)(event_id) >> 32))

/*
 * Refer to unset_pe_monitor_0()
 */
#define unset_pe_monitor_1()     \
	wrmsr_safe((IA32_PERFEVTSEL1), 0, 0)

/*
 * Refer to read_pe_counter_0()
 */
#define read_pe_counter_1(counter)	\
	rdmsrl_safe((IA32_PERFCTR1), (counter))

#endif
