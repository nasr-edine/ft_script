#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

static const char *program_name;

static void error_exit(const char *message)
{
	fprintf(stderr, "%s: %s\n", program_name, message);
	exit(EXIT_FAILURE);
}

static void errno_exit(const char *message)
{
	fprintf(stderr, "%s: ", program_name);
	perror(message);
	exit(EXIT_FAILURE);
}

static int prepare_master_pseudo_terminal(void)
{
	int fd = posix_openpt(O_RDWR | O_NOCTTY);
	if (fd < 0)
		errno_exit("cannot open master pseudo-terminal");
	if (fd <= STDERR_FILENO)
		error_exit("stdin/stdout/stderr are not open");

	if (grantpt(fd) < 0)
		errno_exit("pseudo-terminal permission not granted");
	if (unlockpt(fd) < 0)
		errno_exit("pseudo-terminal permission not unlocked");

	return fd;
}

static const char *slave_pseudo_terminal_name(int master_fd)
{
	errno = 0; /* ptsname may not assign to errno, even if on error */
	const char *name = ptsname(master_fd);
	if (name == NULL)
		errno_exit("cannot name slave pseudo-terminal");
	return name;
}

static int open_noctty(const char *pathname)
{
	int fd = open(pathname, O_RDWR | O_NOCTTY);
	if (fd < 0)
		errno_exit("cannot open slave pseudo-terminal");
	return fd;
}

static void set_terminal_size(int fd)
{
	// should_set_terminal_size = false;
	int ret;
#if defined(TIOCGWINSZ) && defined(TIOCSWINSZ)
	struct winsize size;
	// on recupere la taille du terminal sortie
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) >= 0)
		// on definit la taille du pseudo terminal maitre a partir du terminal de sortie
		ret = ioctl(fd, TIOCSWINSZ, &size);
	printf("fd = %d ioctl = %d Screen width: %i  Screen height: %i\n", fd, ret, size.ws_col, size.ws_row);
#endif /* defined(TIOCGWINSZ) && defined(TIOCSWINSZ) */
}

// static struct termios original_termios;

int main(int argc, char const *argv[])
{
	// ouvre le pseudo terminal maitre
	int master_fd = prepare_master_pseudo_terminal();
	printf("master_fd: %d\n", master_fd);

	// recupere le nom du pseudo terminal esclave
	const char *slave_name = slave_pseudo_terminal_name(master_fd);
	printf("slave_name: %s\n", slave_name);

	// ouvre le pseudo terminal esclave
	int slave_fd = open_noctty(slave_name);
	printf("slave_fd: %d\n", slave_fd);

	// definit la taille du pseudo terminal maitre
	set_terminal_size(master_fd);

	// disable_canonical_io();

	struct termios original_termios;
	tcgetattr(STDIN_FILENO, &original_termios);
	printf("label: %lu\n", original_termios.c_cflag);
	printf("label: %lu\n", original_termios.c_iflag);
	printf("label: %lu\n", original_termios.c_ispeed);
	printf("label: %lu\n", original_termios.c_lflag);
	printf("label: %lu\n", original_termios.c_oflag);
	printf("label: %lu\n", original_termios.c_ospeed);
	// printf("label: %lu\n", original_termios.c_cflag);

	return 0;
}