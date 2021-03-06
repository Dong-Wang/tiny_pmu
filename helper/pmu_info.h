
#ifndef __PMU_INFO_H__
#define __PMU_INFO_H__

/*
 * Must include below headers before pmu_info.h in user space application.
 *   #include <stdint.h>
 *   #include <stdio.h>
 * Must include below headers before pmu_info.h in kernel module.
 *   #include <linux/kernel.h>
 */

/*
 * What information are needed for Tiny PMU?
 * - Which performance monitoring version this CPU support? Verion 1/2/3/4. CPUID.0AH:EAX[7:0].
 * - How many general-purpose counter? CPUID.0AH:EAX[15:8]
 * - Bitwidth of general-purpose counter: CPUID.0AH:EAX[23:16]
 * - How many fixed-function performance coutner? CPUID.0AH:EDX[4:0]
 * - Bitwidth of fixed-function performance counter: CPUID.0AH:EDX[12:5]
 */

#define CPUID_INIT_EAX_PERFMON_LEAF     0x0A

#define CPUID_EAX_PMU_VER_MASK          0x000000FF
#define CPUID_EAX_GPMC_NUM_MASK         0x0000FF00
#define CPUID_EAX_GPMC_WIDTH_MASK       0x00FF0000
#define CPUID_EDX_FPC_NUM_MASK          0x0000001F
#define CPUID_EDX_FPC_WIDTH_MASK        0x00001FE0

#define CPUID_EAX_PMU_VER_SHIFT         0
#define CPUID_EAX_GPMC_NUM_SHIFT        8
#define CPUID_EAX_GPMC_WIDTH_SHIFT      16
#define CPUID_EDX_FPC_NUM_SHIFT         0
#define CPUID_EDX_FPC_WIDTH_SHIFT       5

#define CPUID_TRANS_U32_TO_U8(val, mask, shift) \
	((uint8_t)(((val) & (mask)) >> (shift)))

#define pmu_cpuid(in_eax, out_eax, out_ebx, out_ecx, out_edx)                        \
	__asm__ ("cpuid\n\t"                                                          \
			: "=a" (out_eax), "=b" (out_ebx), "=c" (out_ecx), "=d" (out_edx)    \
			: "0" (in_eax))

#ifdef __KERNEL__
#define PMU_PRINT(fmt, args...) \
	printk(KERN_INFO fmt"\n", ##args)
#else
#define PMU_PRINT(fmt, args...) \
	printf(fmt"\n", ##args)
#endif

struct ia_pmu_info {
	uint8_t version;
	uint8_t gpmc_num;
	uint8_t gpmc_width;
	uint8_t fpc_num;
	uint8_t fpc_width;
};

static inline void get_pmu_info(struct ia_pmu_info *pmu_info)
{
	uint32_t eax, ebx, ecx, edx;

	pmu_cpuid(CPUID_INIT_EAX_PERFMON_LEAF, eax, ebx, ecx, edx);

	pmu_info->version    = CPUID_TRANS_U32_TO_U8(eax, CPUID_EAX_PMU_VER_MASK, CPUID_EAX_PMU_VER_SHIFT);
	pmu_info->gpmc_num   = CPUID_TRANS_U32_TO_U8(eax, CPUID_EAX_GPMC_NUM_MASK, CPUID_EAX_GPMC_NUM_SHIFT);
	pmu_info->gpmc_width = CPUID_TRANS_U32_TO_U8(eax, CPUID_EAX_GPMC_WIDTH_MASK, CPUID_EAX_GPMC_WIDTH_SHIFT);
	pmu_info->fpc_num    = CPUID_TRANS_U32_TO_U8(edx, CPUID_EDX_FPC_NUM_MASK, CPUID_EDX_FPC_NUM_SHIFT);
	pmu_info->fpc_width  = CPUID_TRANS_U32_TO_U8(edx, CPUID_EDX_FPC_WIDTH_MASK, CPUID_EDX_FPC_WIDTH_SHIFT);
}

static inline void print_pmu_info(struct ia_pmu_info *pmu_info)
{
	PMU_PRINT("PMU Version: %d", pmu_info->version);
	PMU_PRINT("Gerneral Purpose Performance Monitoring Coutner");
	PMU_PRINT("  number: %d", pmu_info->gpmc_num);
	PMU_PRINT("  counter width: %d", pmu_info->gpmc_width);
	PMU_PRINT("Fixed Function Perfromance Monitoring Counter");
	PMU_PRINT("  number: %d", pmu_info->fpc_num);
	PMU_PRINT("  counter width: %d", pmu_info->fpc_width);
}

#endif
