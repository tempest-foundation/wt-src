// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#include "integration.h"

#include <kstdio.h>

#include <dbg/logger.h>

// Global syscall infrastructure state
static syscall_status_t syscall_status       = SYSCALL_STATUS_UNINITIALIZED;
static uint8_t          syscall_logging      = 1;  // Logging enabled by default
static uint8_t          security_checks      = 0;  // Security checks disabled by default
static uint64_t         max_calls_per_second = 0;  // No rate limiting by default

// Syscall statistics
static syscall_stats_t syscall_stats                         = {};
static uint64_t        syscall_call_count[SYSCALL_MAX_COUNT] = {};

// Initialize the complete syscall infrastructure
namespace syscall {
	namespace infrastructure {
		// Enable/disable syscall logging
		void set_logging(uint8_t enabled) {
			syscall_logging = enabled;
			logger::debug::printf("syscall",
			                      "info",
			                      "Logging %s\n",
			                      enabled ? "ENABLED" : "DISABLED");
		}

		// Get syscall statistics
		void get_stats(syscall_stats_t *stats) {
			if( !stats ) {
				return;
			}

			// Update most used syscall
			syscall_stats.most_used_count = 0;
			for( uint64_t i = 0; i < SYSCALL_MAX_COUNT; i++ ) {
				if( syscall_call_count[i]
				    > syscall_stats.most_used_count ) {
					syscall_stats.most_used_count =
					    syscall_call_count[i];
					syscall_stats.most_used_syscall = i;
				}
			}

			*stats = syscall_stats;
		}

		// Validate syscall infrastructure integrity
		uint8_t validate(void) {
			logger::debug::printf("syscall",
			                      "info",
			                      "Validating infrastructure integrity...\n");

			// Check that essential syscalls are registered
			uint8_t validation_passed = 1;

			// Test core syscalls
			if( !syscall::is_valid(SYS_EXIT) ) {
				logger::debug::printf(
				    "syscall", "error", "SYS_EXIT not registered\n");
				validation_passed = 0;
			}

			if( !syscall::is_valid(SYS_GETPID) ) {
				logger::debug::printf(
				    "syscall", "error", "SYS_GETPID not registered\n");
				validation_passed = 0;
			}

			// Test that we can get syscall info
			const syscall_entry_t *info = syscall::get_info(SYS_GETPID);
			if( !info || !info->handler ) {
				logger::debug::printf(
				    "syscall",
				    "error",
				    "Cannot get info for SYS_GETPID syscall\n");
				validation_passed = 0;
			}

			// Test invalid syscall handling
			if( syscall::is_valid(999) ) {
				logger::debug::printf(
				    "syscall",
				    "error",
				    "Invalid syscall validation failed\n");
				validation_passed = 0;
			}

			if( validation_passed ) {
				logger::debug::printf(
				    "syscall",
				    "success",
				    "Infrastructure validation PASSED\n");
			} else {
				logger::debug::printf(
				    "syscall",
				    "error",
				    "Infrastructure validation FAILED\n");
			}

			return validation_passed;
		}

		// Quick test interface
		void quick_test(void) {
			logger::debug::printf(
			    "syscall", "info", "Running quick test...\n");

			if( syscall_status != SYSCALL_STATUS_READY ) {
				logger::debug::printf(
				    "syscall",
				    "error",
				    "Syscall infrastructure not ready\n");
				return;
			}

			// Test basic functionality - verify syscalls are registered
			if( !syscall::is_valid(SYS_GETPID) ) {
				logger::debug::printf(
				    "syscall", "error", " SYS_GETPID not registered\n");
				return;
			}

			// Test a basic syscall
			registers_t mock_regs = {};
			mock_regs.rax         = SYS_GETPID;
			syscall::handler(&mock_regs);
			logger::debug::printf("syscall",
			                      "notice",
			                      "getpid test returned: %llu\n",
			                      mock_regs.rax);

			logger::debug::printf(
			    "syscall", "success", "Quick test completed\n");
		}

		// Toggle security checks
		void toggle_security_checks(uint8_t enabled) {
			security_checks = enabled;
			logger::debug::printf("syscall",
			                      "info",
			                      "Security checks %s\n",
			                      enabled ? "ENABLED" : "DISABLED");
		}

		// Set maximum calls per second (rate limiting)
		void set_max_calls_per_second(uint64_t max_calls) {
			max_calls_per_second = max_calls;
			if( max_calls > 0 ) {
				logger::debug::printf(
				    "syscall",
				    "info",
				    "Rate limiting enabled: %llu calls/second\n",
				    max_calls);
			} else {
				logger::debug::printf(
				    "syscall", "success", "Rate limiting disabled\n");
			}
		}

		// Debugging utilities
		void dump_registers(const registers_t *regs) {
			if( !regs ) {
				logger::debug::printf(
				    "syscall", "error", "NULL registers pointer\n");
				return;
			}

			logger::debug::printf(
			    "syscall", nullptr, "info", "Register dump:\n");
			logger::debug::printf("syscall",
			                      "info",
			                      "RAX: 0x%016llx  RBX: 0x%016llx\n",
			                      regs->rax,
			                      regs->rbx);
			logger::debug::printf("syscall",
			                      "info",
			                      "RCX: 0x%016llx  RDX: 0x%016llx\n",
			                      regs->rcx,
			                      regs->rdx);
			logger::debug::printf("syscall",
			                      "info",
			                      "RSI: 0x%016llx  RDI: 0x%016llx\n",
			                      regs->rsi,
			                      regs->rdi);
			logger::debug::printf("syscall",
			                      "info",
			                      "RBP: 0x%016llx  R8:  0x%016llx\n",
			                      regs->rbp,
			                      regs->r8);
			logger::debug::printf("syscall",
			                      "info",
			                      "R9:  0x%016llx  R10: 0x%016llx\n",
			                      regs->r9,
			                      regs->r10);
			logger::debug::printf("syscall",
			                      "info",
			                      "R11: 0x%016llx  R12: 0x%016llx\n",
			                      regs->r11,
			                      regs->r12);
			logger::debug::printf("syscall",
			                      "info",
			                      "R13: 0x%016llx  R14: 0x%016llx\n",
			                      regs->r13,
			                      regs->r14);
			logger::debug::printf(
			    "syscall", "info", "R15: 0x%016llx\n", regs->r15);
			logger::debug::printf("syscall",
			                      "info",
			                      "INT: 0x%016llx  ERR: 0x%016llx\n",
			                      regs->int_no,
			                      regs->err_code);
		}

		void trace_call(uint64_t syscall_no,
		                uint64_t arg0,
		                uint64_t arg1,
		                uint64_t arg2,
		                uint64_t arg3,
		                uint64_t arg4,
		                uint64_t arg5) {
			if( !syscall_logging ) {
				return;
			}

			const syscall_entry_t *info = syscall::get_info(syscall_no);
			const char            *name = info ? info->name : "UNKNOWN";

			logger::debug::printf(
			    "syscall",
			    "info",
			    "TRACE: %s(%llu) args=[0x%llx, 0x%llx, 0x%llx, "
			    "0x%llx, 0x%llx, "
			    "0x%llx]\n",
			    name,
			    syscall_no,
			    arg0,
			    arg1,
			    arg2,
			    arg3,
			    arg4,
			    arg5);
		}

		// Reset syscall statistics
		void reset_stats(void) {
			syscall_stats.total_calls       = 0;
			syscall_stats.successful_calls  = 0;
			syscall_stats.failed_calls      = 0;
			syscall_stats.invalid_calls     = 0;
			syscall_stats.most_used_syscall = 0;
			syscall_stats.most_used_count   = 0;

			// Reset per-syscall counters
			for( uint64_t i = 0; i < SYSCALL_MAX_COUNT; i++ ) {
				syscall_call_count[i] = 0;
			}

			logger::debug::printf("syscall", "success", "Statistics reset\n");
		}

		// Print syscall subsystem information
		void print_info(void) {
			logger::debug::printf(
			    "syscall", nullptr, "\nInfrastructure Information\n");

			const char *status_str;
			switch( syscall_status ) {
				case SYSCALL_STATUS_UNINITIALIZED:
					status_str = "UNINITIALIZED";
					break;
				case SYSCALL_STATUS_INITIALIZING:
					status_str = "INITIALIZING";
					break;
				case SYSCALL_STATUS_READY:
					status_str = "READY";
					break;
				case SYSCALL_STATUS_ERROR:
					status_str = "ERROR";
					break;
				default:
					status_str = "UNKNOWN";
					break;
			}

			logger::debug::printf(
			    "syscall", "info", "Status: %s\n", status_str);
			logger::debug::printf("syscall",
			                      "info",
			                      "Logging: %s\n",
			                      syscall_logging ? "ENABLED" : "DISABLED");
			logger::debug::printf("syscall",
			                      "info",
			                      "Security Checks: %s\n",
			                      security_checks ? "ENABLED" : "DISABLED");
			logger::debug::printf("syscall",
			                      "info",
			                      "Rate Limiting: %s\n",
			                      max_calls_per_second > 0 ? "ENABLED"
			                                               : "DISABLED");
			if( max_calls_per_second > 0 ) {
				logger::debug::printf("syscall",
				                      "info",
				                      "Max Calls/Second: %llu\n",
				                      max_calls_per_second);
			}

			logger::debug::printf("syscall", nullptr, "\nStatistics:\n");
			logger::debug::printf("syscall",
			                      "info",
			                      "Total Calls: %llu\n",
			                      syscall_stats.total_calls);
			logger::debug::printf("syscall",
			                      "info",
			                      "Successful: %llu\n",
			                      syscall_stats.successful_calls);
			logger::debug::printf("syscall",
			                      "info",
			                      "Failed: %llu\n",
			                      syscall_stats.failed_calls);
			logger::debug::printf("syscall",
			                      "info",
			                      "Invalid: %llu\n",
			                      syscall_stats.invalid_calls);
			if( syscall_stats.most_used_count > 0 ) {
				logger::debug::printf(
				    "syscall",
				    "info",
				    "Most Used: Syscall %llu (%llu calls)\n",
				    syscall_stats.most_used_syscall,
				    syscall_stats.most_used_count);
			}
		}

		// Print all registered syscalls
		void print_table(void) {
			logger::debug::printf(
			    "syscall", nullptr, "\nRegistered Syscalls\n");
			logger::debug::printf(
			    "syscall",
			    nullptr,
			    "=====================================================\n");
			logger::debug::printf(
			    "syscall",
			    nullptr,
			    "No.  | Name                    | Args | Calls\n");
			logger::debug::printf(
			    "syscall",
			    nullptr,
			    "-----|-------------------------|------|----------\n");

			uint32_t registered_count = 0;
			for( uint64_t i = 0; i < SYSCALL_MAX_COUNT; i++ ) {
				if( syscall::is_valid(i) ) {
					const syscall_entry_t *info =
					    syscall::get_info(i);
					if( info && info->name ) {
						logger::debug::printf(
						    "syscall",
						    "info",
						    "%-4llu | %-23s | %-4u | %-8llu\n",
						    i,
						    info->name,
						    info->arg_count,
						    syscall_call_count[i]);
						registered_count++;
					}
				}
			}

			logger::debug::printf("syscall",
			                      "info",
			                      "Total registered syscalls: %u\n",
			                      registered_count);
		}

		// Shutdown the syscall infrastructure
		void shutdown(void) {
			logger::debug::printf(
			    "syscall", "info", "Shutting down syscall infrastructure\n");

			// Print final statistics
			print_info();

			// Reset all syscalls (unregister them)
			for( uint64_t i = 0; i < SYSCALL_MAX_COUNT; i++ ) {
				if( syscall::is_valid(i) ) {
					syscall::unbind(i);
				}
			}

			syscall_status = SYSCALL_STATUS_UNINITIALIZED;
			logger::debug::printf(
			    "syscall", "success", "Infrastructure shutdown complete\n");
		}

		// Get the current status of the syscall subsystem
		syscall_status_t get_status(void) {
			return syscall_status;
		}

		void init(void) {
			logger::debug::printf(
			    "syscall", "info", "Initializing infrastructure\n");

			syscall_status = SYSCALL_STATUS_INITIALIZING;

			// Initialize the core syscall subsystem
			syscall::init();

// Setup IDT for syscalls
#ifdef ARCH_AMD64
			amd64::idt::setup_syscall();
#endif

			// Reset statistics
			reset_stats();

			// Validate infrastructure
			if( !validate() ) {
				logger::debug::printf(
				    "syscall",
				    "error",
				    "Infrastructure validation failed!\n");
				syscall_status = SYSCALL_STATUS_ERROR;
				return;
			}

			syscall_status = SYSCALL_STATUS_READY;

			logger::debug::printf("syscall",
			                      "info",
			                      "Infrastructure initialization complete\n");
			logger::debug::printf("syscall", "info", "Status: READY\n");
			logger::debug::printf("syscall",
			                      "info",
			                      "Logging: %s\n",
			                      syscall_logging ? "ENABLED" : "DISABLED");
			logger::debug::printf("syscall",
			                      "info",
			                      "Security checks: %s\n",
			                      security_checks ? "ENABLED" : "DISABLED");
		}
	}  // namespace infrastructure
}  // namespace syscall
