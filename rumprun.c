#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>

#include "rumprunfn.h"

void _netbsd_exit(int status);
extern char *_netbsd__progname;

static jmp_buf buf;
static int ret;

#define MAXARGS 20

static int cmd(int id, ...);

static int
cmd(int id, ...)
{
	va_list ap;
	int argc = 0;
	char *argv[MAXARGS];
	char *arg;
	int i;

	va_start(ap, id);
	for (i = 0; i < MAXARGS; i++) {
		arg = va_arg(ap, char *);
		if (arg == NULL)
			break;
		argv[argc++] = arg;
	}
	va_end(ap);
	_netbsd__progname = argv[0];
	if (! setjmp(buf)) {
		ret = rumprun_functions[id](argc, argv);
	}
	return ret;
}

int
main() {

	ret = cmd(CMD_IFCONFIG, "ifconfig", "lo0", NULL);
	/* cannot call another function after exit() unless re-init libc... */
	ret = cmd(CMD_PING, "ping", "-o", "127.0.0.1", NULL);
	if (! setjmp(buf)) {
		//_netbsd_exit(ret); /* TODO unfortunately we override this! need to get a reference */
	}
	return ret;
}

void
emul_exit(int status)
{

	ret = status;
	longjmp(buf, status);
}

void
emul__exit(int status)
{

	ret = status;
	longjmp(buf, status);
}

#define STUB_ABORT(name) void name(void); void name(void) { abort(); }

STUB_ABORT(emul__fork)
STUB_ABORT(emul__vfork14)
STUB_ABORT(emul_execve)
