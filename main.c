/**********************************************************
* Signal handler example
*
* We'll install our own signal handler this time.
* We'll still work with the Ctrl-C, it's simple to test
*
*********************************************************/
#include <signal.h>
#include <stdio.h> // perror, printf
#include <sys/types.h> // pid_t
#include <unistd.h> // sleep
// use the extended signal handler instead of void (*X)(int)
#include <stdlib.h>
void myHandler(int signum, siginfo_t* sinfo, void* vp) {
    printf("SIG: signal number %d\n", sinfo->si_signo);
    printf("SIG: signal errno %d\n", sinfo->si_errno);
    printf("SIG: signal code %d\n", sinfo->si_code);
    printf("SIG: sending PID %d\n", sinfo->si_pid);
    printf("SIG: sending UID %d\n", sinfo->si_uid);
    // the other sinfo elements are not useable with SIGINT
    printf("SIG: reason for signal:\n");
    switch(sinfo->si_code) {
        case SI_USER:
        printf(" SIG: kill, segsend or raise\n"); break;
        case SI_KERNEL:
        printf(" SIG: the kernel\n"); break;
        case SI_QUEUE:
        printf(" SIG: sigqueue\n"); break;
        case SI_TIMER:
        printf(" SIG: timer expired\n"); break;
        case SI_MESGQ:
        printf(" SIG: mesq state changed\n"); break;
        case SI_ASYNCIO:
        printf(" SIG: Async IO completed\n"); break;
        case SI_SIGIO:
        printf(" SIG: queued SIGIO (one of multiple\n"); break;
        default:
        printf(" SIG: UNKNOWN?\n");
        }
    }

int main(int argc, char** argv) {
struct sigaction sa;
pid_t child;

printf("pid = %d\n", getpid());
// prepare the parameter for sigaction
// the pointer to the handler routine
sa.sa_sigaction = &myHandler;
// don't block additional signals while handling this one
sigemptyset(&sa.sa_mask);
// set single action and extended signal handler
sa.sa_flags = SA_ONESHOT | SA_SIGINFO;


printf("main: pid %d, ppid %d\n", getpid(), getppid());
child = fork();
if (child == 0)
 {
    printf("child: created with pid %d, ppid %d\n", getpid(), getppid());
    printf("Installing the handler\n");
    if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Problem installing SIGINT-handler");
    exit(EXIT_FAILURE);}
    exit(EXIT_SUCCESS);
}
else if (child == -1)
    {
    perror("problem creating a child");
    return -1;
    }
else
    {
    printf("main: created child %d\n", child);
    }
printf("Sleeping for 10 seconds\n");
sleep(10);
wait();
printf("Done sleeping, ending operation\n");
return 0;
}

/*
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <syslog.h>
#define LOG_DAEMON_NAME "borg"
#define LOG_FACILITY LOG_DAEMON
#define LOG_PRIORITY LOG_ERR

void daemonize(void) {
pid_t pid;
pid = fork();
if (pid < 0) {
    perror("on fork while daemonizing");
    exit(EXIT_FAILURE);
    }

if (pid > 0) {
// if we're the parent, we will terminate now
// so that child will be moved under init
exit(EXIT_SUCCESS);
}
// create a new session and put us into it
if (setsid() < 0) {
perror("on session creation while daemonizing");
exit(EXIT_FAILURE);
}
if (chdir("/") < 0) {
perror("on root dir change while daemonizing");
exit(EXIT_FAILURE);
}
// reset umask
umask(0);
// we don't need the normal fds anymore
// we use the symbolic constants here so that it's clear
// what w're doing
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
}
int main(int argc, char** argv) {
pid_t pid, ppid;
int i = 0;
pid = getpid();
ppid = getppid();
// open logging facility
// we want the system logger (syslogd) to add the
// PID to each message, hence the 'LOG_PID' flag
openlog(LOG_DAEMON_NAME, LOG_PID, LOG_FACILITY);
syslog(LOG_PRIORITY,
"main[pid=%d,ppid=%d]: going daemon\n", pid, ppid);
daemonize();
// we cannot use printf anymore. stdout is closed
// nothing will be printed here (infact, printf
// returns error here, but won't check that)
printf("hello world\n");
// refresh the p[p]ids
pid = getpid();
ppid = getppid();
syslog(LOG_PRIORITY,
"main[pid=%d,ppid=%d]: am daemon, starting loop\n",
pid, ppid);
while(i < 10) {
syslog(LOG_PRIORITY,
"iteration %.2d, waiting for 1s\n", i++);
sleep(1);
}
syslog(LOG_PRIORITY, "stopping\n");
return EXIT_SUCCESS;
}*/
