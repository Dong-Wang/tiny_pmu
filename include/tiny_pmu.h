/*
 * Tiny APIs to access PMU.
 */
#ifndef __TINY_PMU_H_
#define __TINY_PMU_H_

#include <asm/msr.h>

/* 
 * use to deal with float value in kernel
 */
#define IA32_SCALE_FACTOR 	10000

/* 
 * thread per core, can be 1 or 2 
 */
#define IA32_THREAD_PER_CORE 1	  

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

/*
 * the max bit of general counter of Skylake is 48bit
 */
#define IA32_MAX_NUM_BIT_GENERAL_COUNTER 0xffffffffffff

/*
 * calculate the delta between counter value x and y
 */
#define counter_delta(x,y) ((y)>=(x)?(y - x):(IA32_MAX_NUM_BIT_GENERAL_COUNTER - x + y))

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
 * Detailed description in <Intel 64 and IA-32 Architectures Software Developer's Manual>, Document ID 325462
 * 18.2.1.2 Pre-defined Architectural Performance Events
 */
#define IA32_PERFEVT_UNHALTCORECYCLE	0x0043003c  /* UnHalted Core Cycles */
#define IA32_PERFEVT_INST_RETIRE 		0x004300c0  /* Instructions Retired*/
#define IA32_PERFEVT_UNHALTREFCYCLE		0x0043013c  /* UnHalted Reference Cycles */
#define IA32_PERFEVT_LLC_REF     		0x00434f2e  /* LLC Reference */
#define IA32_PERFEVT_LLC_MISS    		0x0043412e  /* LLC Misses */
#define IA32_PERFEVT_BRANCH_HITS 		0x004300c4  /* Branch Instruction Retired */
#define IA32_PERFEVT_BRANCH_MISS 		0x004300c5  /* Branch Misses Retired */
#define IA32_PERFEVT_TOPDOWN_SLOTS 		0x004301a4  /* Topdown Slots */

/*
 * Detailed description in <Intel 64 and IA-32 Architectures Software Developer's Manual>, Document ID 325462
 * Chapter 19 Performance Monitoring Events
 * This event is for top-down framework
 */
#define	IA32_PERFEVT_CPU_CLK_UNHALTED_THREAD 			0x0043003c  /* clock */
#define IA32_PERFEVT_INT_MISC_RECOVERY_CYCLES 			0x0043010d  /* recovery cycles */
#define IA32_PERFEVT_UOPS_ISSUED_ANY 					0x0043010e  /* slots issue */
#define IA32_PERFEVT_UOPS_RETIRED_RETIRED_SLOTS 		0x004302c2  /* slots retired */
#define IA32_PERFEVT_IDQ_UOPS_0_NOT_DELIVERED_CORE 		0x0443019c /* fetch bubbles */
#define IA32_PERFEVT_IDQ_UOPS_LE3_NOT_DELIVERED_CORE 	0x0043019c  /* fetch bubbles */
#define IA32_PERFEVT_ISSUE_SLOTS_NOT_CONSUMED_RECOVERY 	0x004302ca  /* recovery bubbles */
#define IA32_PERFEVT_BR_MISP_RETIRED_ALL_BRANCHES 		0x004300c5  /* branch miss prediction retired */
#define IA32_PERFEVT_MACHINE_CLEARS_COUNT 				0x014701c3  /* machine clear count */
#define IA32_PERFEVT_IDQ_MS_UOPS						0x01433079  /* total microcode sequencer deliver uops */
#define IA32_PERFEVT_UOPS_ISSUED_ANY					0x0043010e  /* uops from RAT to RS */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_TOTAL 		0x004304a3	/* total execution stalls */
#define IA32_PERFEVT_RS_EVENTS_EMPTY_CYCLES 			0x0043015e	/* cycles when RS is empty */
#define IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_1_UOP_EXEC 0x014301b1	/* cycles at least 1 uops execute */
#define IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_2_UOP_EXEC 0x024301b1	/* cycles at least 2 uops execute */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_MEM_ANY 		0x004314a3	/* stall due to memory load */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L1D_MISS 	0x00430ca3	/* stall due to memory load when L1 miss */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L2_MISS 		0x004305a3	/* stall due to memory load when L2 miss */
#define IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_HIT 		0x004304d1	/* retired uops for LLC hit */
#define IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_MISS 	0x004320d1	/* retired uops for LLC miss */
#define IA32_PERFEVT_RESOURCE_STALLS_SB 				0x004308a2	/* stall due to store buffer full */
#define IA32_PERFEVT_UNC_ARB_TRK_OCCUPANCY_ALL 			0x00430180	/* cycles waiting for memory controller */
#define IA32_PERFEVT_INST_RETIRED_ANY_P					0x004300C0	/* Number of instructions retired. General Counter - architectural event */
#define IA32_PERFEVT_EXE_ACTIVITY_BOUND_ON_STORES		0x004340A6	/* Cycles where the Store Buffer was full and no outstanding load */
#define IA32_PERFEVT_EXE_ACTIVITY_1_PORTS_UTIL			0x004302A6	/* Cycles total of 1 uop is executed on all ports and Reservation Station was not empty */
#define IA32_PERFEVT_EXE_ACTIVITY_2_PORTS_UTIL			0x004304A6  /* Cycles total of 2 uops are executed on all ports and Reservation Station was not empty */
#define IA32_PERFEVT_EXE_ACTIVITY_EXE_BOUND_0_PORTS		0x004301A6  /* Cycles where no uops were executed, the Reservation Station was not empty, the Store Buffer was full and there was no outstanding load */

/*
 * formula for memory bound and core bound
 * s:scale factor
 * t:thread per core
 * a:IDQ_UOPS_NOT_DELIVERED.CORE
 * b:UOPS_ISSUED.ANY
 * c:INT_MISC.RECOVERY_CYCLES_ANY
 * d:CPU_CLK_UNHALTED.THREAD_ANY
 * e:UOPS_RETIRED.RETIRE_SLOTS
 * f:CYCLE_ACTIVITY.STALLS_MEM_ANY
 * g:EXE_ACTIVITY.BOUND_ON_STORES
 * j:EXE_ACTIVITY.1_PORTS_UTIL
 * k:EXE_ACTIVITY.2_PORTS_UTIL
 * m:EXE_ACTIVITY.EXE_BOUND_0_PORTS
 * p:CPU_CLK_UNHALTED.THREAD
 * q:INST_RETIRED.ANY
 */
#define memory_bound_formula(s,t,a,b,c,d,e,f,g,j,k,m,p,q) \
	(s - s*((b-e+4*(c/t)+a+e)/(4*d/t)))*(f+g)/(m+j+(((s*q/p)>18*s/10)?k:0)+f+g)
#define core_bound_formula(s,t,a,b,c,d,e,f,g,j,k,m,p,q) \
	(s-(s*(b-e+4*(c/t)+a+e)/(4*d/t)))*(s-(s*(f+g)/(m+j+(((s*q/p)>18*s/10)?k:0)+f+g)))/s

#endif