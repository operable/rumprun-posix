#include "rumprunfn.h"

int (*rumprun_functions[CMD_MAX])(int, char **) = {
	main_ifconfig,
	main_ping,
};

