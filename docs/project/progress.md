# Wind/Tempest Project - Progress

> [!WARNING]
> This progress list is not final and will probably change as the project evolves.

## Basic Stuff (Done)

- [x] Bootloader: Limine + Multiboot2
- [x] Basic VGA text mode (replaced with framebuffer)
- [x] Basic shell
- [x] Debugging tools (serial logging, asserts, etc.)

## Core Features (In Progress / Partially Done)

- [x] LibC (minimal or musl-based)
- [x] Memory management (paging, heap, allocators) (basic but still functional)
- [x] Timers (PIT + HPET)
- [x] ACPI & Power management (shutdown, reboot, etc.)
- [x] Filesystem support (ext2)
- [ ] Multitasking
- [x] Kernel Panic

## System Services & Architecture

- [x] Syscall interface
- [ ] User-space binary loading (.ELF)
- [x] TTY-like subsystem
- [x] Privilege separation (rings, even if basic)
- [ ] Module loading (e.g., drivers)
- [ ] Package manager (not sure yet)

## Hardware Support

- [x] Amd64 support (mostly stable)
- [x] Drivers: video, serial, keyboard (more to come)
- [ ] Networking stack
- [ ] Sound support (optional/future)

## UI / UX

- [ ] Simple GUI (framebuffer-based)
- [ ] Input abstraction (mouse, advanced keyboard input)

---

Don’t expect miracles. This is a kernel, not a toy.
