// Simple init program that doesn't require libc
// Uses syscall directly to exit

void _start(void) {
	// Syscall 60 = exit
	// Just loop forever for now since we don't have syscalls working yet
	while(1) {
		// Busy loop - we successfully entered userspace!
	}
	
	// If we get here, exit
	__asm__ volatile(
	    "mov $60, %%rax\n"    // syscall number for exit
	    "mov $0, %%rdi\n"     // exit code 0
	    "syscall\n"
	    :
	    :
	    : "rax", "rdi"
	);
}
