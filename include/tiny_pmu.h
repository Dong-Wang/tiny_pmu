/*
 * Tiny APIs to access PMU.
 */
#ifndef __TINY_PMU_H_
#define __TINY_PMU_H_

#include <asm/msr.h>

/* 
 * Different CPU support different number of general-purpose performance monitoring counter (GPMC)
 * Please check how many counters your CPU support by CPUID.0AH:EAX[15:08] for non-hyperthreading
 * and CPUID.0AH:EAX[23:16] for hyperthreading
 */

/*
 * Performance Event Selection Register index
 */
#define IA32_PERFEVTSEL0 0x186
#define IA32_PERFEVTSEL1 0x187
#define IA32_PERFEVTSEL2 0x188
#define IA32_PERFEVTSEL3 0x189
#define IA32_PERFEVTSEL4 0x18a
#define IA32_PERFEVTSEL5 0x18b
#define IA32_PERFEVTSEL6 0x18c
#define IA32_PERFEVTSEL7 0x18d

/*
 * Performance Counter Register index
 */
#define IA32_PERFCTR0 0xc1
#define IA32_PERFCTR1 0xc2
#define IA32_PERFCTR2 0xc3
#define IA32_PERFCTR3 0xc4
#define IA32_PERFCTR4 0xc5
#define IA32_PERFCTR5 0xc6
#define IA32_PERFCTR6 0xc7
#define IA32_PERFCTR7 0xc8

/*
 * Performance Counter width
 */
#define IA32_PERFCTR_WIDTH          48    /* General Purpose Counter,Skylake Server */
#define IA32_PERFCTR_MAX_VALUE      ((1ULL<<IA32_PERFCTR_WIDTH)-1)

/*
 * Detailed description in <Intel 64 and IA-32 Architectures Software Developer's Manual>, Document ID 325462
 * 18.2.1.2 Pre-defined Architectural Performance Events
 */
#define IA32_PERFEVT_UNHALTCORECYCLE    0x0053003c  /* UnHalted Core Cycles */
#define IA32_PERFEVT_INST_RETIRE        0x005300c0  /* Instructions Retired*/
#define IA32_PERFEVT_UNHALTREFCYCLE     0x0053013c  /* UnHalted Reference Cycles */
#define IA32_PERFEVT_LLC_REF            0x00534f2e  /* LLC Reference */
#define IA32_PERFEVT_LLC_MISS           0x0053412e  /* LLC Misses */
#define IA32_PERFEVT_BRANCH_HITS        0x005300c4  /* Branch Instruction Retired */
#define IA32_PERFEVT_BRANCH_MISS        0x005300c5  /* Branch Misses Retired */
#define IA32_PERFEVT_TOPDOWN_SLOTS      0x005301a4  /* Topdown Slots */

/*
 * Skylake Server Performance Event list
 */
#define IA32_PERFEVT_CPU_CLK_UNHALTED_THREAD 			    0x0053003c  /* clock */
#define IA32_PERFEVT_INT_MISC_RECOVERY_CYCLES               0x0053010d  /* recovery cycles */
#define IA32_PERFEVT_UOPS_ISSUED_ANY                        0x0053010e  /* slots issue */
#define IA32_PERFEVT_UOPS_RETIRED_RETIRED_SLOTS             0x005302c2  /* slots retired */
#define IA32_PERFEVT_IDQ_UOPS_0_NOT_DELIVERED_CORE          0x0453019c  /* fetch bubbles */
#define IA32_PERFEVT_IDQ_UOPS_LE3_NOT_DELIVERED_CORE        0x0353019c  /* fetch bubbles */
#define IA32_PERFEVT_ISSUE_SLOTS_NOT_CONSUMED_RECOVERY      0x005302ca  /* recovery bubbles */
#define IA32_PERFEVT_BR_MISP_RETIRED_ALL_BRANCHES           0x005301c5  /* branch miss prediction retired */
#define IA32_PERFEVT_MACHINE_CLEARS_COUNT                   0x005301c3  /* machine clear count */
#define IA32_PERFEVT_IDQ_MS_UOPS                            0x00533079  /* total microcode sequencer deliver uops */
#define IA32_PERFEVT_UOPS_ISSUED_ANY                        0x0053010e  /* uops from RAT to RS */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_TOTAL            0x005304a3  /* total execution stalls */
#define IA32_PERFEVT_RS_EVENTS_EMPTY_CYCLES                 0x0053015e  /* cycles when RS is empty */
#define IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_1_UOP_EXEC     0x015301b1  /* cycles at least 1 uops execute */
#define IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_2_UOP_EXEC     0x025301b1  /* cycles at least 2 uops execute */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_MEM_ANY          0x065306a3  /* stall due to memory load */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L1D_MISS         0x0c530ca3  /* stall due to memory load when L1 miss */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L2_MISS          0x055305a3  /* stall due to memory load when L2 miss */
#define IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_HIT          0x005304d1  /* retired uops for LLC hit */
#define IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_MISS         0x005320d1  /* retired uops for LLC miss */
#define IA32_PERFEVT_RESOURCE_STALLS_SB                     0x005308a2  /* stall due to store buffer full */
#define IA32_PERFEVT_UNC_ARB_TRK_OCCUPANCY_ALL              0x00530180  /* cycles waiting for memory controller */

/*
 * Setup performance event to GPMC
 * index, GPMC index, range 0~7, depends on your CPU.
 * event_id: u64, IA32_PERFEVT_x
 * return value: 0 for success, other for failed.
 */
#define set_pe_monitor(index, event_id) \
	wrmsr_safe((IA32_PERFEVTSEL##index), (u32)((u64)(event_id)), (u32)((u64)(event_id) >> 32))

/*
 * Remove performance event from specify GPMC
 * index: GPMC index, range 0~7, depends on your CPU.
 * return value: 0 for success, other for failed.
 */
#define unset_pe_monitor(index) \
	wrmsr_safe((IA32_PERFEVTSEL##index), 0, 0)

/*
 * Read event counter
 * index: GPMC index, range 0~7, depends on your CPU.
 * value: (u64 *)
 * return value: 0 for success, other for failed.
 */
#define read_pe_counter(index, value) \
	rdmsrl_safe((IA32_PERFCTR##index), (value))

#define distance(x,y) ((y)>=(x)?(y - x):(0xffffffffffff - x + y))

/*
 * Calculat event number
 */
#define counter_delta(begin, end) \
	((end)>=(begin)? ((end)-(begin)) : (IA32_PERFCTR_MAX_VALUE+(end)-(begin)))

#endif
