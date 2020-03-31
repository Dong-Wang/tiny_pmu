
#include <stdint.h>
#include <stdio.h>

#include "pmu_info.h"

int main(int argc, char *argv[])
{
	struct ia_pmu_info pmu_info;

	get_pmu_info(&pmu_info);
	print_pmu_info(&pmu_info);
	return 0;
}

