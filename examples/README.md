# libadt examples

Each example is a separate executable, so you can edit and run only the one you are experimenting with.

From the `examples` directory:

```sh
make list
make run-basic
make run-custom-type
make run-debug
```

The Makefile assumes this project layout:

```text
include/libadt/*.h
src/dynamic_array.c
src/comparators.c
src/printers.c
examples/*.c
```

Override paths or source files without editing the Makefile:

```sh
make run-debug PROJECT_ROOT=/path/to/libadt
make run-basic LIBADT_SOURCES="../src/dynamic_array.c ../src/comparators.c ../src/printers.c"
```
