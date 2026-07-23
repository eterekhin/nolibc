//
// Created by Evgeny Terekhin on 08.07.26.
//

#ifndef NOLIBC_WASM_IMPORTS_H
#define NOLIBC_WASM_IMPORTS_H

void *wasm_mmap(long addr, long size, int prot);

int wasm_write(int fd, char *buf, int count);

int wasm_exit(int code);
#endif //NOLIBC_WASM_IMPORTS_H
