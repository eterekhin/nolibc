def test_c_program_output_in_browser(run_c_in_browser):
    output = run_c_in_browser(
        r'''
#include <stdio.h>

int main(int argc, char **argv, char **envp)
{
    printf("\n");
    printf("hello from wasm\n");
    printf("a\nb\n");
    return 0;
}
'''
    )

    assert ['', 'hello from wasm', 'a', 'b'] == output

def test_malloc_simple(run_c_in_browser):
    output = run_c_in_browser(
        r'''
#include <stdio.h>
#include <stdlib.h>

void copy_str(char* src, char* dst) {
    for (int i = 0; (int)src[i] != 0; i++)
		dst[i] = src[i];
}

int main(int argc, char **argv, char **envp)
{
    char* mem = malloc(16);
	char* s = "hello from wasm!\n";
	copy_str(s, mem);
	printf(mem);
	return 0;
}
'''
    )

    assert ['hello from wasm!'] == output

def test_malloc_2_calls(run_c_in_browser):
    output = run_c_in_browser(
        r'''
#include <stdio.h>
#include <stdlib.h>

void copy_str(char* src, char* dst) {
    for (int i = 0; (int)src[i] != 0; i++)
		dst[i] = src[i];
}

int main(int argc, char **argv, char **envp)
{
    char* mem = malloc(16);
	char* s = "hello from wasm!\n";
	copy_str(s, mem);
	
	char* mem2 = malloc(16);
	char* s2 = "hello from malloc!\n";
	copy_str(s2, mem2);
	
	printf(mem);
	printf(mem2);
	return 0;
}
'''
    )

    assert ['hello from wasm!', 'hello from malloc!'] == output

def test_malloc_huge_request(run_c_in_browser):
    output = run_c_in_browser(
        fr'''
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp)
{{
    char* mem = malloc((1 << 16) + 2);
    for (int i=0; i < (1 << 16); i++)
        mem[i] = 'a';
    mem[(1 << 16)] = '\n';
    mem[(1 << 16) + 1] = 0;
	printf(mem);
	return 0;
}}
'''
    )
    assert ['a' * (1 << 16)] == output

