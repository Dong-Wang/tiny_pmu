/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Demo to show how to use tiny_pmu.h.
 */
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/notifier.h>
#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <asm/cpufeature.h>

#include "tiny_pmu.h"


/* ~10 instructions */
static inline void monitor_target_nop(void)
{
        asm volatile("nop ; nop ; nop ; nop ; nop");
        asm volatile("nop ; nop ; nop ; nop ; nop");
}

static inline void monitor_target_add(void)
{
	int i, j;
	for (i=0; i<500; i++) {
		j+=i;
	}
}

static int pmu_demo_init(void)
{
	u64 ins_begin_counter, ins_end_counter;
	u64 cyc_begin_counter, cyc_end_counter;
	int ret_0, ret_1;

	printk("Load PMU demo...Hello.\n");

	ret_0 = set_pe_monitor(0, IA32_PERFEVT_INST_RETIRE);
	ret_0 = set_pe_monitor(1, IA32_PERFEVT_UNHALTCORECYCLE);
	ret_0 = read_pe_counter(0, &ins_begin_counter);
	ret_0 = read_pe_counter(1, &cyc_begin_counter);

	monitor_target_add();
	//monitor_target_nop();
	//printk("Run monitor target.\n");
	//asm volatile("cpuid");

	ret_1 = read_pe_counter(0, &ins_end_counter);
	ret_1 = read_pe_counter(1, &cyc_end_counter);
	ret_1 = unset_pe_monitor(0);
	ret_1 = unset_pe_monitor(1);

	printk(KERN_INFO"ret_0: %d; ret_1: %d; instruction retired: %llu; core cycles: %llu; CPI(multiply by %d): %llu.\n",
		ret_0,
		ret_1,
		counter_delta(ins_begin_counter, ins_end_counter),
		counter_delta(cyc_begin_counter, cyc_end_counter),
		TMAM_MATRIC_RESOLUTION,
		tmam_cpi(counter_delta(cyc_begin_counter, cyc_end_counter), counter_delta(ins_begin_counter, ins_end_counter)));

	return 0;
}

static void pmu_demo_exit(void)
{
	printk("Unload PMU demo...Bye.\n");
}

module_init(pmu_demo_init);
module_exit(pmu_demo_exit);
MODULE_LICENSE("GPL");
