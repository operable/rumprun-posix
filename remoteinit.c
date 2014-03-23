#include <sys/types.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <rump/rumpclient.h>
#include <rump/rump_syscalls.h>

/* we are not supposed to use values below 100 but NetBSD libc does */
void rumprun_init (void) __attribute__((constructor (1)));

static void
die(const char *fmt, ...)
{
        va_list va;

        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
        fputs("\n", stderr);
        exit(1);
}

void
rumprun_init()
{
        int ret, fd;

        ret = rumpclient_init();
        if (ret != 0)
                die("rumpclient init failed");
        /* this has to be the greatest hack ever */
        while ((fd = rump_sys_kqueue()) < 3)
                continue;
        rump_sys_close(fd);
}

/* add the fork emulations here as only applicable when using rumpclient */
int
emul__fork(void)
{
	return rumpclient_fork();
}

int
emul__vfork14(void)
{
	return rumpclient_fork();
}

extern char **environ;

int
emul_execve(const char *filename, char *const argv[], char *const envp[])
{
	return rumpclient_exec(filename, argv, environ);
}

