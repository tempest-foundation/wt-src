# Wind/Tempest Project - Compiling

## Prerequisites

- QEMU (for testing in virtual environment)
- NASM (for compiling .ASM files)
- Clang (for compiling .C files)
- Make (for compiling using the automatic way)

## Building

There are several ways to build Wind/Tempest:

### Using Make

To only compile and link Wind/Tempest, run:
```bash
make
```

to compile and link Wind/Tempest and run it in QEMU, run:
```bash
make run
```

### Using Toolchain

> [!IMPORTANT]
> If any .sh file is not executable, run `chmod +x <filename>`

To compile and link Wind/Tempest using toolchain, run:

```bash
./build.sh
```

There is some arguments that can be passed to the script, to see the available options:

```bash
./build.sh -h 
```

or you can use help in the extended way:

```bash
./build.sh --help
```