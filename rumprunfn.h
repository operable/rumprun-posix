int main_ifconfig(int argc, char **argv);
int main_ping(int argc, char **argv);

enum commands {
	CMD_IFCONFIG,
	CMD_PING,
	CMD_MAX,
};

int (*rumprun_functions[CMD_MAX])(int, char **);

