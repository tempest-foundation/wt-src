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
#!/usr/bin/bash

find sys/ include/ -name "*.[cpph]" -print0 \
  | xargs -0 sed -i -E 's#^([[:space:]]*)/\*\s*(.*?)\s*\*/\s*$#\1// \2#'

while IFS= read -r -d '' file; do
  last_char=$(tail -c1 "$file")
  if [ "$last_char" != "" ]; then
    echo >> "$file"
  fi
done < <(find sys/ include/ -name "*.[cpph]" -print0)

find sys/ include/ \( -name "*.cpp" -o -name "*.h" \) -print0 | while IFS= read -r -d '' file; do
  if ! clang-format -i "$file"; then
    echo -e "\e[33mIgnored: $file (too long path or error)\e[0m"
  fi
done
