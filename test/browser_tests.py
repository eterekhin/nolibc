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
