/* vi: set sw=8 ts=8: sts=8 */
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <setjmp.h>
#include <unistd.h>
#include <ucontext.h>


static ucontext_t *child;
static void (*child_func) (void);
static int child_argc;
static void *child_arg;
static jmp_buf parent;

int __getcontext(ucontext_t *__ucp)
{
	sigset_t sigempty;
	sigemptyset(&sigempty);
	memset(__ucp, 0, sizeof(ucontext_t));
	sigprocmask(SIG_BLOCK, &sigempty, &__ucp->uc_sigmask);
	return 0;
}

static void __initialize_context(void)
{
	ucontext_t *this = child;
	void (*func) (void) = child_func;
	void (*func2) (void *) = (void (*)(void *))child_func;
	int argc = child_argc;
	void *arg = child_arg;
	child = NULL;

	if (sigsetjmp(this->uc_mcontext, 0) == 0) {
		longjmp(parent, 1);
	}

	sigprocmask(SIG_SETMASK, &this->uc_sigmask, NULL);

	if (argc) {
		func2(arg);
	} else {
		func();
	}

	if (this->uc_link) {
		setcontext(this->uc_link);
	}

	exit(0);
}

int setcontext(const ucontext_t *ucp)
{
	siglongjmp(*(sigjmp_buf *)&ucp->uc_mcontext, 1);
	/* we should never get here */
	assert(0);
}

int swapcontext(ucontext_t *oucp, ucontext_t *ucp)
{
	if (sigsetjmp(oucp->uc_mcontext, 1) == 0) {
		siglongjmp(ucp->uc_mcontext, 1);
	}
	return 0;
}

void makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
	va_list ap;
	struct sigaction action, oldaction;
	sigset_t sigs, oldsigs;
	stack_t oldstack;

	assert((ucp->uc_stack.ss_sp) != NULL);

	child = ucp;
	child_func = func;
	child_argc = argc;

	if (argc) {
		assert(argc <= 1);
		va_start(ap, argc);
		child_arg = va_arg(ap, void *);
		va_end(ap);
	}

	action.sa_handler = (void(*)(int))__initialize_context;
	action.sa_flags = SA_ONSTACK;
	sigemptyset(&action.sa_mask);

	sigemptyset(&sigs);
	sigaddset(&sigs, SIGUSR1);

	sigprocmask(SIG_BLOCK, &sigs, &oldsigs);
	sigaltstack(&ucp->uc_stack, &oldstack);
	sigaction(SIGUSR1, &action, &oldaction);

	kill(getpid(), SIGUSR1);

	sigfillset(&sigs);
	sigdelset(&sigs, SIGUSR1);
	if (!setjmp(parent)) {
		while (child) {
			sigsuspend(&sigs);
		}
	}

	sigaltstack(&oldstack, NULL);
	sigaction(SIGUSR1, &oldaction, NULL);
	sigprocmask(SIG_SETMASK, &oldsigs, NULL);
}
