//
// Created by Evgeny Terekhin on 04.07.26.
//

#ifndef NOLIBC_ARCH_WASM_H
#define NOLIBC_ARCH_WASM_H

#ifndef _NOLIBC_ARCH_WASM_H
#define _NOLIBC_ARCH_WASM_H

#include "linux/unistd.h"
#include "errno.h"
#include "compiler.h"
#include "crt.h"
#include "wasm_imports.h"

#define __ARCH_WANT_SYS_OLD_SELECT

#define DEBUGGING_SYSCALLS

int main(int argc, char **argv, char **envp);

static __inline__ long NOT_IMPLEMENTED_SYSCALL() {
    return -ENOSYS; // return syscall is missing
}

static __inline__ long my_syscall0_handler(long num) {
    switch (num) {
        case __NR_setsid:
        case __NR_sched_yield:
        case __NR_getuid: // get user id
        case __NR_geteuid: // effective user id
#if defined(__NR_geteuid32)
        case __NR_getuid32:  // effective 32bit user id
#endif
        case __NR_gettid: // Impossible to get tid as a number, only info if it is a worker thread or main thread
        case __NR_getppid:
        case __NR_getpid:
#if defined(WASM_STRICT_SYSCALL_IMPLEMENTATION)
            __builtin_trap(); // "Currenly not supported for wasm"
#else
            // TODO: it might be better to fake the values separately
            return 0;
#endif
#if defined(__NR_vfork)
        case __NR_vfork:
            __builtin_trap(); // "Currenly not supported for wasm"
            break;
#endif
#if defined(__NR_fork)
        case __NR_fork:
            __builtin_trap(); // "Currenly not supported for wasm"
            break;
#endif
        default:
            __builtin_trap(); //"Not supported for wasm"
    }
}

static __inline__ long my_syscall3_handler(long num, long arg1, long arg2, long arg3) {
    switch (num) {
        case __NR_write:
            long fd = arg1;
            long buf = arg2;
            long count = arg3;
            return wasm_write(fd, (char*)buf, count);
    }
    return NOT_IMPLEMENTED_SYSCALL();
}

static __inline__ long my_syscall6_handler(long num, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    switch (num) {
#if defined(__NR_mmap2)
        case __NR_mmap2:
#endif
        case __NR_mmap:
            long addr = arg1;
            long len = arg2;
            return (long)wasm_mmap(addr, len);
            break;
    }
    return NOT_IMPLEMENTED_SYSCALL();
}

#define my_syscall0(num)                                                      \
({                                                                            \
    my_syscall0_handler(num);                                                            \
})

#define my_syscall1(num, arg1)                                                \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();													  \
})

#define my_syscall2(num, arg1, arg2)                                          \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();													  \
})

#define my_syscall3(num, arg1, arg2, arg3)                                    \
({                                                                            \
	my_syscall3_handler(num, (long)arg1, (long)arg2, (long)arg3);                               \
})
#define my_syscall4(num, arg1, arg2, arg3, arg4)                              \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();                                                \
})

#define my_syscall5(num, arg1, arg2, arg3, arg4, arg5)                        \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();												  \
})

#define my_syscall6(num, arg1, arg2, arg3, arg4, arg5, arg6)                \
({                                                                          \
    my_syscall6_handler(num, (long)arg1, (long)arg2, (long)arg3, (long)arg4, (long)arg5, (long)arg6);           \
})

#ifndef NOLIBC_NO_RUNTIME
/* startup code */
void __attribute__((weak, noreturn)) __no_stack_protector _start(void) {
    // TODO: extract all main arguments properly
    main(0, 0, 0);
}
#endif /* NOLIBC_NO_RUNTIME */

#endif /* _NOLIBC_ARCH_WASM_H */


#endif //NOLIBC_ARCH_WASM_H
