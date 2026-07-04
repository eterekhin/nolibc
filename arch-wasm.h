//
// Created by Evgeny Terekhin on 04.07.26.
//

#ifndef NOLIBC_ARCH_WASM_H
#define NOLIBC_ARCH_WASM_H

#ifndef _NOLIBC_ARCH_ARM_H
#define _NOLIBC_ARCH_ARM_H

#include "compiler.h"
#include "crt.h"

#define __ARCH_WANT_SYS_OLD_SELECT

int main(int argc, char **argv, char **envp);

long NOT_IMPLEMENTED_SYSCALL() {
	return 1;
}

#define my_syscall0(num)                                                      \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();                                                \
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
	NOT_IMPLEMENTED_SYSCALL();													  \
})

#define my_syscall4(num, arg1, arg2, arg3, arg4)                              \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();                                                \
})

#define my_syscall5(num, arg1, arg2, arg3, arg4, arg5)                        \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();												  \
})

#define my_syscall6(num, arg1, arg2, arg3, arg4, arg5, arg6)                  \
({                                                                            \
	NOT_IMPLEMENTED_SYSCALL();												\
})

#ifndef NOLIBC_NO_RUNTIME
/* startup code */
void __attribute__((weak, noreturn)) __no_stack_protector _start(void) {
	// TODO: extract all main arguments properly
	main(0, 0,0);
}
#endif /* NOLIBC_NO_RUNTIME */

#endif /* _NOLIBC_ARCH_ARM_H */


#endif //NOLIBC_ARCH_WASM_H
