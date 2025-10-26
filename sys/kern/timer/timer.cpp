#include <ktime.h>

#include <arch/amd64/asm/io.h>
#include <arch/amd64/idt/idt.h>

// PIT frequency is 1193182 Hz
#define PIT_FREQUENCY    1193182
#define TARGET_FREQUENCY 100  // 100Hz = 10ms per tick

/**
 * @brief Timer interrupt handler
 * 
 * Called on every timer tick (IRQ0). Increments the uptime counter.
 * 
 * @param regs Pointer to saved register state
 */
static void
    timer_irq_handler(registers_t *regs) {
	(void) regs;  // Unused
	time::uptime_tick();
}

/**
 * @brief Initialize the Programmable Interval Timer (PIT)
 * 
 * Sets up the PIT to generate interrupts at TARGET_FREQUENCY Hz.
 * Also registers the timer IRQ handler.
 */
void
    timer_init(void) {
	// Calculate divisor for desired frequency
	uint16_t divisor = (uint16_t) (PIT_FREQUENCY / TARGET_FREQUENCY);

	// Initialize uptime tracking
	time::uptime_init();

	// Register the timer interrupt handler (IRQ0)
	amd64::irq::bind(0, timer_irq_handler);

	// Configure PIT Channel 0 (system timer)
	// Command: Channel 0, Access mode: lobyte/hibyte, Mode 3 (square wave), Binary
	outb(0x43, 0x36);

	// Send divisor
	outb(0x40, (uint8_t) (divisor & 0xFF));         // Low byte
	outb(0x40, (uint8_t) ((divisor >> 8) & 0xFF));  // High byte
}
