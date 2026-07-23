//
// Created by Evgeny Terekhin on 04.07.26.
//

#ifndef _NOLIBC_ARCH_WASM_H
#define _NOLIBC_ARCH_WASM_H

#include "linux/unistd.h"
#include "errno.h"
#include "compiler.h"
#include "crt.h"
#include "wasm_imports.h"

// #define __ARCH_WANT_SYS_OLD_SELECT

#define DEBUGGING_SYSCALLS

// wasm-ld adds this symbol, that goes after all static data
// https://github.com/llvm/llvm-project/blob/ef1b46f29293db7b16dee598fb1e8f2a62555c33/lld/wasm/Config.h#L191
extern char __heap_base;

unsigned long wasm_heap_base()
{
    return (unsigned long)&__heap_base;
}

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

static __inline__ long my_syscall1_handler(long num, long arg1) {
    switch (num) {
        case __NR_exit:
            long code = arg1;
            wasm_exit(code);
            __builtin_unreachable();
    }

    return NOT_IMPLEMENTED_SYSCALL();
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
            long prot = arg3;
            return (long)wasm_mmap(addr, len, prot);
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
	my_syscall1_handler(num, (long)(arg1)); 										  \
})

#define my_syscall2(num, arg1, arg2)                                          \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();													  \
})

#define my_syscall3(num, arg1, arg2, arg3)                                    \
({                                                                            \
	my_syscall3_handler(num, (long)(arg1), (long)(arg2), (long)(arg3));       \
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
    my_syscall6_handler(num, (long)(arg1), (long)(arg2), (long)(arg3), (long)(arg4), (long)(arg5), (long)(arg6));  \
})

#ifndef NOLIBC_NO_RUNTIME

extern int put_args(char **storage, int *argc);
extern int put_env(char **storage, int *env_size);

static __inline__ void copy(char *src, char **dst, int count) {
    char *cur = src;
    for (int i = 0; i < count; i++) {
        if (src[i] == 0) {
            *dst = cur;
            cur = src + i + 1;
            dst++;
        }
    }
}

/* startup code */
void __attribute__((weak, noreturn)) __no_stack_protector wasm_start() {
    int argc = 0, envc = 0;
    int argv_size = put_args((char **) 0, &argc);
    int env_size = put_env((char **) 0, &envc);

    char argv_data[argv_size];
    char env_data[env_size];
    void *data[
        1 + // argc_size
        argc + 1 /*NULL*/ +
        envc + 1 /*NULL*/ +
        + 2 // auxv: NULL(AT_NULL), NULL
    ];

    // argc
    ((int*)data)[0] = argc;

    //argv
    put_args((char **) argv_data, 0);
    copy(argv_data, (char**)(data + 1), argv_size);
    data[1 + argc] = 0;

    //env
    put_env((char**)env_data, 0);
    copy(env_data, (char**)(data + 1 + argc + 1), env_size);
    data[1 + argc + 1 + envc] = 0;

    // auxv
    // Setting 2 first words to null as expected for auxiliary vector :https://refspecs.linuxfoundation.org/LSB_1.3.0/IA64/spec/auxiliaryvector.html
    data[1 + argc + 1 + envc + 1] = 0;
    data[1 + argc + 1 + envc + 1 + 1] = 0;

    _start_c((long*)data);
}
#endif /* NOLIBC_NO_RUNTIME */

#endif /* _NOLIBC_ARCH_WASM_H */
