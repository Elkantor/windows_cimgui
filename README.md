### Compile & Run

To compile & run the program, use your favorite C compiler on windows, and link against gdi32 and user32:

* with gcc:
```sh
gcc src/main.c -lgdi32 -luser32
a.exe
```

* with clang (or zig cc):
```sh
clang src/main.c -lgdi32 -luser32
a.exe

zig cc src/main.c -lgdi32 -luser32
a.out
```

