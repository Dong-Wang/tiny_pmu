
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "pmu_info.h"

static int pmu_info_init(void)
{
	struct ia_pmu_info pmu_info;

	printk("Load pmu_info....\n");
	get_pmu_info(&pmu_info);
	print_pmu_info(&pmu_info);

	return 0;
}

static void pmu_info_exit(void)
{
	printk("Unload pmu_info....\n");
}


module_init(pmu_info_init);
module_exit(pmu_info_exit);
MODULE_LICENSE("GPL");

