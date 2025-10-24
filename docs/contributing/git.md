# Wind/Tempest Project - Commit Message Style Guide

This document defines the commit message convention used in the Wind/Tempest project. Following this convention keeps the Git history clean and searchable.

## Commit Structure

```
<subsystem>/<component>: Brief summary (≤50 chars)

(Optional longer description explaining what changed, why it changed, and any side effects)
(Or you can add additional parts of change in description)

Fixes: <commit-hash> ("Original summary")   # optional
Reported-by: Name <email>                   # optional
Co-authored-by: Name <email>                # optional
Tested-by: Name <email>                     # optional
Signed-off-by: Name <email>
```

## Examples

```
kern/init: Fix panic in division by zero

There is no check in `start_kernel()` and if the value is 0 then we panic.
This commit fixes this.

Fixes: f5c3e8a7b1d9c2f4e6a7d8b9c0e1f2a3b4c5d6e7 ("kern/panic: Initial support for kpanic")
Reported-by: Isaque Pereira Mota <isaque@tempestfoundation.org>
Co-authored-by: Isaque Pereira Mota <isaque@tempestfoundation.org>
Tested-by: Isaque Pereira Mota <isaque@tempestfoundation.org>
Signed-off-by: Russian95CrE <russian95@tempestfoundation.org>
```

## Common Subsystems

- `kern:` core kernel logic
- `arch:` platform-specific code (like x86, arm, etc...)
- `mm:` memory management
- `fs:` file systems
- `drivers:` generic drivers (like video, serial, PCI, USB, etc...)
- `boot:` bootloader and early setup
- `build:` build system (Makefiles, scripts)
- `docs:` documentation
- `debug:` debugging tools and logging
- `lib:` shared helper libraries
- `ci:` CI (continuous integration)

## Quick Rules

- Use imperative present tense (`fix`, `add`, `remove`, `refactor`)
- Prefix should be lowercase
- Leave a blank line between the title and body
- Do **not** end the title with a period
- Always include a `Signed-off-by` line
