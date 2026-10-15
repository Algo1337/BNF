#include <fsl.h>

#define RTLD_LAZY   0x00001
#define RTLD_NOW    0x00002
#define RTLD_GLOBAL 0x00100
#define RTLD_LOCAL  0
#define RTLD_NODELETE 0x01000
#define RTLD_NOLOAD 0x00004

void *dlopen(const char *filename, int flags);
void *dlsym(void *handle, const char *symbol);
int dlclose(void *handle);
char *dlerror(void);

int main()
{
    println("A\n");

    init_mem();
    println("B\n");

    ptr dick = dlopen("./nig.so", RTLD_NOW);
    _printf("C: %p\n", dick);
    if (!dick) {
        char *err = dlerror();
        _printf("dlopen error: %s\n", err ? err : "(none)");
        return 1;
    }

    void (*foo)(void) = (void (*)(void))dlsym(dick, "_test");
    _printf("D: %p\n", foo);

    foo();
    println("E\n");

    dlclose(dick);
    println("F\n");

    println("DONE\n");
    return 0;
}