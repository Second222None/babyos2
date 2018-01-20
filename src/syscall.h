/*
 * guzhoudiaoke@126.com
 * 2017-10-30
 */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "types.h"


enum {
    SYS_PRINT = 0,
    SYS_FORK,
    SYS_EXEC,
	SYS_MMAP,
	SYS_EXIT,
	SYS_WAIT,
    SYS_SLEEP,
    SYS_SIGNAL,
    SYS_SIGRET,
    SYS_KILL,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_READ,
    SYS_WRITE,
    SYS_LINK,
    SYS_UNLINK,
    SYS_MKDIR,
    SYS_MKNOD,
    SYS_DUP,
    SYS_STAT,
    SYS_CHDIR,
    SYS_PIPE,
    MAX_SYSCALL,
};


class syscall_t {
public:
    static void  do_syscall(trap_frame_t* frame);
    static int32 sys_print(trap_frame_t* frame);
    static int32 sys_fork(trap_frame_t* frame);
    static int32 sys_exec(trap_frame_t* frame);
    static int32 sys_mmap(trap_frame_t* frame);
    static int32 sys_exit(trap_frame_t* frame);
    static int32 sys_wait(trap_frame_t* frame);
    static int32 sys_sleep(trap_frame_t* frame);
    static int32 sys_signal(trap_frame_t* frame);
    static int32 sys_sigret(trap_frame_t* frame);
    static int32 sys_kill(trap_frame_t* frame);
    static int32 sys_open(trap_frame_t* frame);
    static int32 sys_close(trap_frame_t* frame);
    static int32 sys_read(trap_frame_t* frame);
    static int32 sys_write(trap_frame_t* frame);
    static int32 sys_link(trap_frame_t* frame);
    static int32 sys_unlink(trap_frame_t* frame);
    static int32 sys_mkdir(trap_frame_t* frame);
    static int32 sys_mknod(trap_frame_t* frame);
    static int32 sys_dup(trap_frame_t* frame);
    static int32 sys_stat(trap_frame_t* frame);
    static int32 sys_chdir(trap_frame_t* frame);
    static int32 sys_pipe(trap_frame_t* frame);

private:
    static char s_print_buffer[1024];
    static int32 (*s_system_call_table[])(trap_frame_t* frame);
};

#endif
