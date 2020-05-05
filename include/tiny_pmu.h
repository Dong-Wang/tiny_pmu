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
 * Performance General Counter width
 * Skylake Server
 */
#define IA32_PERFCTR_WIDTH          48    
#define IA32_PERFCTR_MAX_VALUE      ((1ULL<<IA32_PERFCTR_WIDTH)-1)

/*
 * Detailed description in <Intel 64 and IA-32 Architectures Software Developer's Manual>, Document ID 325462
 * 18.2.1.2 Pre-defined Architectural Performance Events
 */
#define IA32_PERFEVT_UNHALTCORECYCLE    0x0043003c  /* UnHalted Core Cycles */
#define IA32_PERFEVT_INST_RETIRE        0x004300c0  /* Instructions Retired*/
#define IA32_PERFEVT_UNHALTREFCYCLE     0x0043013c  /* UnHalted Reference Cycles */
#define IA32_PERFEVT_LLC_REF            0x00434f2e  /* LLC Reference */
#define IA32_PERFEVT_LLC_MISS           0x0043412e  /* LLC Misses */
#define IA32_PERFEVT_BRANCH_HITS        0x004300c4  /* Branch Instruction Retired */
#define IA32_PERFEVT_BRANCH_MISS        0x004300c5  /* Branch Misses Retired */
#define IA32_PERFEVT_TOPDOWN_SLOTS      0x004301a4  /* Topdown Slots */

/*
 * Skylake Server Performance Event list
 */
#define IA32_PERFEVT_CPU_CLK_UNHALTED_THREAD 			    0x0043003c  /* clock */
#define IA32_PERFEVT_INT_MISC_RECOVERY_CYCLES               0x0043010d  /* recovery cycles */
#define IA32_PERFEVT_UOPS_ISSUED_ANY                        0x0043010e  /* slots issue */
#define IA32_PERFEVT_UOPS_RETIRED_RETIRED_SLOTS             0x004302c2  /* slots retired */
#define IA32_PERFEVT_IDQ_UOPS_0_NOT_DELIVERED_CORE          0x0443019c  /* fetch bubbles */
#define IA32_PERFEVT_IDQ_UOPS_LE3_NOT_DELIVERED_CORE        0x0343019c  /* fetch bubbles */
#define IA32_PERFEVT_ISSUE_SLOTS_NOT_CONSUMED_RECOVERY      0x004302ca  /* recovery bubbles */
#define IA32_PERFEVT_BR_MISP_RETIRED_ALL_BRANCHES           0x004301c5  /* branch miss prediction retired */
#define IA32_PERFEVT_MACHINE_CLEARS_COUNT                   0x004301c3  /* machine clear count */
#define IA32_PERFEVT_IDQ_MS_UOPS                            0x00433079  /* total microcode sequencer deliver uops */
#define IA32_PERFEVT_UOPS_ISSUED_ANY                        0x0043010e  /* uops from RAT to RS */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_TOTAL            0x004304a3  /* total execution stalls */
#define IA32_PERFEVT_RS_EVENTS_EMPTY_CYCLES                 0x0043015e  /* cycles when RS is empty */
#define IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_1_UOP_EXEC     0x014301b1  /* cycles at least 1 uops execute */
#define IA32_PERFEVT_UOPS_EXECUTED_CYCLES_GE_2_UOP_EXEC     0x024301b1  /* cycles at least 2 uops execute */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_MEM_ANY          0x064314a3  /* stall due to memory load */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L1D_MISS         0x0c430ca3  /* stall due to memory load when L1 miss */
#define IA32_PERFEVT_CYCLE_ACTIVITY_STALLS_L2_MISS          0x054305a3  /* stall due to memory load when L2 miss */
#define IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_HIT          0x004304d1  /* retired uops for LLC hit */
#define IA32_PERFEVT_MEM_LOAD_UOPS_RETIRED_LLC_MISS         0x004320d1  /* retired uops for LLC miss */
#define IA32_PERFEVT_RESOURCE_STALLS_SB                     0x004308a2  /* stall due to store buffer full */
#define IA32_PERFEVT_UNC_ARB_TRK_OCCUPANCY_ALL              0x00430180  /* cycles waiting for memory controller */
#define IA32_PERFEVT_INST_RETIRED_ANY_P                     0x004300C0  /* Number of instructions retired. General Counter - architectural event */
#define IA32_PERFEVT_EXE_ACTIVITY_BOUND_ON_STORES           0x004340A6  /* Cycles where the Store Buffer was full and no outstanding load */
#define IA32_PERFEVT_EXE_ACTIVITY_1_PORTS_UTIL              0x004302A6  /* Cycles total of 1 uop is executed on all ports and Reservation Station was not empty */
#define IA32_PERFEVT_EXE_ACTIVITY_2_PORTS_UTIL              0x004304A6  /* Cycles total of 2 uops are executed on all ports and Reservation Station was not empty */
#define IA32_PERFEVT_EXE_ACTIVITY_EXE_BOUND_0_PORTS         0x004301A6  /* Cycles where no uops were executed, the Reservation Station was not empty, the Store Buffer was full and there was no outstanding load */

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
 * Calculat event number
 */
#define counter_delta(begin, end) \
	((end)>=(begin)? ((end)-(begin)) : (IA32_PERFCTR_MAX_VALUE + (end) - (begin) + 1))

/*
 * TMAM (Top-down Microarchitecture Anlysis Method) Metrics Formula List
 * NOTICE!!!!  To avoid float computing in kernel space, numerator mulitply by TMAM_MATRIC_RESOLUTION.
 */
#define TMAM_MATRIC_RESOLUTION 100

/*
 * Threads per core, usually 1 or 2
 */
#define TMAM_MATRIC_THREADS 1

/*
 * limit the bound value to [0,1]
 */
#define limit(value) ((value>=0&&value<=TMAM_MATRIC_RESOLUTION)?value:(value<0)?0:TMAM_MATRIC_RESOLUTION)

/* CPI */
#define tmam_cpi(CPU_CLK_UNHALTED_THREAD, INST_RETIRED_ANY) \
	limit(((CPU_CLK_UNHALTED_THREAD)*TMAM_MATRIC_RESOLUTION / (INST_RETIRED_ANY)))

/*
 * formula for frontend_bound
 * a:IA32_PERFEVT_IDQ_UOPS_LE3_NOT_DELIVERED_CORE
 * b:IA32_PERFEVT_CPU_CLK_UNHALTED_THREAD
 */
#define tmam_frontend_bound(a,b) \
	limit((TMAM_MATRIC_RESOLUTION * TMAM_MATRIC_THREADS * (a) / (b) / 4))

/*
 * formula for bad_speculation
 * a:UOPS_ISSUED.ANY
 * b:UOPS_RETIRED.RETIRE_SLOTS
 * c:INT_MISC.RECOVERY_CYCLES_ANY
 * d:CPU_CLK_UNHALTED.THREAD
 */
#define tmam_bad_speculation(a,b,c,d) \
	limit((TMAM_MATRIC_RESOLUTION * ((a)-(b) + (4 * (c))/TMAM_MATRIC_THREADS) / ((d) * 4/ TMAM_MATRIC_THREADS)))

/*
 * formula for retiring
 * a:UOPS_RETIRED.RETIRE_SLOTS
 * b:CPU_CLK_UNHALTED.THREAD
 */
#define tmam_retiring(a,b) \
	limit((TMAM_MATRIC_RESOLUTION * (a) / ((b) * 4 / TMAM_MATRIC_THREADS)))

/*
 * formula for backend_bound
 * a:frondend_bound
 * b:bad_speculation
 * c:retiring
 */
#define tmam_backend_bound(a,b,c) \
	limit((TMAM_MATRIC_RESOLUTION-((a) + (b) + (c))))

/*
 * formula for retiring
 * a:UOPS_RETIRED.RETIRE_SLOTS
 * b:IDQ_UOPS_NOT_DELIVERED.CORE
 */
#define tmam_fetch_latency_bound(a,b) \
	limit((TMAM_MATRIC_RESOLUTION * (a) / (b) / 4))

/*
 * formula for fetch_bandwidth_bound
 * a:frondend_bound
 * b:tmam_fetch_latency_bound
 */
#define tmam_fetch_bandwidth_bound(a,b) \
	limit(((a) - (b)))

/*
 * formula for micro_sequencer
 * a:IDQ.MS_CYCLES
 * b:CPU_CLK_UNHALTED.THREAD
 */
#define tmam_micro_sequencer(a,b) \
	limit((TMAM_MATRIC_RESOLUTION * (a) / (b) / 4))

/*
 * formula for micro_sequencer
 * a:retiring
 * b:micro_sequencer
 */
#define tmam_base(a,b) \
	limit(((a) - (b)))
	
/*
 * formula for memory bound
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
#define tmam_memory_bound(a,b,c,d,e,f,g,j,k,m,p,q) \
	limit((TMAM_MATRIC_RESOLUTION - TMAM_MATRIC_RESOLUTION*(((b)-(e)+4*((c)/TMAM_MATRIC_THREADS)+(a)+(e))/(4*(d)/TMAM_MATRIC_THREADS)))*((f)+(g))/((m)+(j)+(((TMAM_MATRIC_RESOLUTION*(q)/(p))>18*TMAM_MATRIC_RESOLUTION/10)?(k):0)+(f)+(g)))

/*
 * formula for core bound
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
#define tmam_core_bound(a,b,c,d,e,f,g,j,k,m,p,q) \
	limit((TMAM_MATRIC_RESOLUTION-(TMAM_MATRIC_RESOLUTION*((b)-(e)+4*((c)/TMAM_MATRIC_THREADS)+(a)+(e))/(4*(d)/TMAM_MATRIC_THREADS)))*(TMAM_MATRIC_RESOLUTION-(TMAM_MATRIC_RESOLUTION*((f)+(g))/((m)+(j)+(((TMAM_MATRIC_RESOLUTION*(q)/(p))>18*TMAM_MATRIC_RESOLUTION/10)?(k):0)+(f)+(g))))/TMAM_MATRIC_RESOLUTION)

/*
 * formula for l1_bound
 * a:CYCLE_ACTIVITY.STALLS_MEM_ANY
 * b:CYCLE_ACTIVITY.STALLS_L1D_MISS
 * c:CPU_CLK_UNHALTED.THREAD
 */
#define tmam_l1_bound(a,b,c) \
	limit(TMAM_MATRIC_RESOLUTION * ((a) - (b)) / (c))

/*
 * formula for l2_bound
 * a:CYCLE_ACTIVITY.STALLS_L1D_MISS
 * b:CYCLE_ACTIVITY.STALLS_L2_MISS
 * c:CPU_CLK_UNHALTED.THREAD
 */
#define tmam_l2_bound(a,b,c) \
	limit(TMAM_MATRIC_RESOLUTION * ((a) - (b)) / (c))

#endif
