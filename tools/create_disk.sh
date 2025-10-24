# SPDX-License-Identifier: GPL-3.0-only
# * -- BEGIN METADATA HEADER --
# * 
# * <*---The Wind/Tempest Project---*>
# * 
# * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
# * Maintainer : Tempest Foundation <development@tempestfoundation.org>
# * 
# * Copyright (c) Tempest Foundation, 2025
# * -- END OF METADATA HEADER --
#!/usr/bin/env bash
# create_disk.sh -- helper to create an EXT2 disk image for QEMU
#
# Usage: ./create_disk.sh [SIZE]
#   SIZE -- optional disk size (default 64M). Accepts any dd-compatible
#           size suffix (e.g. 32M, 128M, 1G).
#
# The script produces build/disk.img, formatted with an empty EXT2
# filesystem.  Add it to your QEMU command line like so:
#   -drive file=build/disk.img,format=raw,if=ide
#
# Requirements: dd, mkfs.ext2 (from e2fsprogs)

SIZE="${1:-64M}"
IMAGE_DIR="build"
IMAGE_PATH="$IMAGE_DIR/hard-drive.img"

mkdir -p "$IMAGE_DIR"

if [[ -f "$IMAGE_PATH" ]]; then
  echo "[*] $IMAGE_PATH already exists. Removing and recreating."
  rm -f "$IMAGE_PATH"
fi

echo "[*] Creating disk image ($SIZE) at $IMAGE_PATH"

dd if=/dev/zero of="$IMAGE_PATH" bs="$SIZE" count=0 seek=1

echo "[*] Formatting image with EXT2 filesystem"
mkfs.ext2 -F -q -O ^has_journal -b 1024 -I 128 "$IMAGE_PATH"

# Remove lost+found directory from the image
if debugfs -R "ls /lost+found" "$IMAGE_PATH" >/dev/null 2>&1; then
  echo "[*] Removing lost+found from disk image"
  debugfs -w -R "rmdir /lost+found" "$IMAGE_PATH"
fi

# Copy contents from tests/vfst instead of ./vfst
SOURCE_DIR="tests/vfst"
if [[ -d "$SOURCE_DIR" ]]; then
  echo "[*] Copying contents of $SOURCE_DIR into disk.img via debugfs"
  
  # Create directories first
  find "$SOURCE_DIR" -mindepth 1 -type d -print0 | while IFS= read -r -d '' dir; do
    rel="${dir#$SOURCE_DIR/}"
    debugfs -w -R "mkdir /$rel" "$IMAGE_PATH" >/dev/null 2>&1 || true
  done

  # Copy files
  find "$SOURCE_DIR" -type f -print0 | while IFS= read -r -d '' file; do
    rel="${file#$SOURCE_DIR/}"
    debugfs -w -R "write $file /$rel" "$IMAGE_PATH" >/dev/null 2>&1
  done
else
  echo "[*] No $SOURCE_DIR directory found; skipping copy step."
fi
