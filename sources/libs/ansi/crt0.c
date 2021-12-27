#include <stdlib.h>
#include <navy/debug.h>

extern int main(int argc, char const **argv);

[[gnu::noreturn]] void _start(int argc, char const **argv)
{
    int ex = main(argc, argv);
    exit(ex);
    __builtin_unreachable();
}