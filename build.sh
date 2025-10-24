#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

readonly TOOLS_PATH="tools"
readonly CPU_COUNT="$(nproc)"

# Colors
readonly RED="\e[31m"
readonly GREEN="\e[32m"
readonly RESET="\e[0m"
readonly GRAY="\e[90m"

log() {
	printf "[%b%9s%b] %s\n" "$GRAY" "$(date '+%H:%M:%S')" "$RESET" "$*"
}

log_color() {
	local color="$1"; shift
	printf "%b[%s] %s%b\n" "$color" "$(date '+%H:%M:%S')" "$*" "$RESET"
}

error_exit() {
	log_color "$RED" "ERROR: $*"
	exit 1
}

run_step() {
	log "➤ Running: $*"
	if ! "$@"; then
		error_exit "Step failed: $*"
	fi
}

usage() {
	cat <<EOF
Usage: $0 [OPTIONS]

  -b, --build-only    Only build (do not launch QEMU)
  -r, --run-only      Only launch QEMU (assumes build is up-to-date)
  -h, --help          Show this help message

If no options are given, this script will build AND then run.
EOF
	exit 0
}

# Parse args
SHOULD_BUILD=true
SHOULD_RUN=true

while (( $# )); do
	case "$1" in
		-b|--build-only) SHOULD_BUILD=true; SHOULD_RUN=false ;;
		-r|--run-only)   SHOULD_BUILD=false; SHOULD_RUN=true ;;
		-h|--help)       usage ;;
		*) error_exit "Invalid option: $1" ;;
	esac
	shift
done

# Check required tools
for tool in make qemu-system-x86_64; do
	if ! command -v "$tool" &>/dev/null; then
		error_exit "Required tool not found: $tool"
	fi
done

if [ ! -d "$TOOLS_PATH" ]; then
	error_exit "Tools directory not found: $TOOLS_PATH"
fi

if $SHOULD_BUILD; then
	run_step "${TOOLS_PATH}/header.sh" "generate"
	run_step "${TOOLS_PATH}/format.sh"
	run_step make "-j${CPU_COUNT}"
fi

if $SHOULD_RUN; then
	log "Launching QEMU..."
	run_step make run
fi

log_color "$GREEN" "All done!"
