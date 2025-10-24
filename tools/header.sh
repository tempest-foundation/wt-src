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

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <generate|remove>" >&2
    exit 1
fi

mode="$1"

if [[ "$mode" != "generate" && "$mode" != "remove" ]]; then
    echo "Usage: $0 <generate|remove>" >&2
    exit 1
fi

changes_made=false

process_file() {
    local file="$1"

    if [[ "$mode" = "remove" ]]; then
        remove_header "$file"
    elif [[ "$mode" = "generate" ]]; then
        generate_header "$file"
    fi
}

remove_header() {
    local file="$1"

    if ! grep -q "Tempest Foundation" "$file"; then
        return 0
    fi

    echo "Removing header of: $file"

    local tmpfile="$file.tmp"
    rm -f "$tmpfile"
    : > "$tmpfile"

    local in_license_header=false
    local skip_spdx=true
    mapfile -t lines < "$file"
    local i=0
    local len=${#lines[@]}

    while [ $i -lt $len ]; do
        local line="${lines[$i]}"
        i=$((i + 1))

        if [[ "$skip_spdx" = true ]] && echo "$line" | grep -q "^// SPDX-License-Identifier:"; then
            skip_spdx=false
            continue
        fi
        skip_spdx=false

        if echo "$line" | grep -q "^/\*" && [[ "$in_license_header" = false ]]; then
            local temp_lines=("$line")
            local is_license_header=false
            local j=$i
            while [ $j -lt $len ]; do
                local next_line="${lines[$j]}"
                j=$((j + 1))
                temp_lines+=("$next_line")
                if echo "$next_line" | grep -q -- "-- BEGIN METADATA HEADER --"; then
                    is_license_header=true
                    in_license_header=true
                    i=$j
                    break
                fi
                if echo "$next_line" | grep -q "\*/"; then
                    break
                fi
            done
            if [[ "$is_license_header" = false ]]; then
                for temp_line in "${temp_lines[@]}"; do
                    echo "$temp_line" >> "$tmpfile"
                done
                i=$j
            fi
            continue
        fi

        if [[ "$in_license_header" = true ]]; then
            if echo "$line" | grep -q -- "-- END OF METADATA HEADER --"; then
                i=$((i + 1))
                in_license_header=false
            fi
            continue
        fi

        echo "$line" >> "$tmpfile"
    done

    awk 'NF || started { started=1; print }' "$tmpfile" > "$file"
    rm -f "$tmpfile"

    changes_made=true
}

generate_header() {
    local file="$1"
    local header
    header=$(head -n 25 "$file")

    if echo "$header" | grep -q "Copyright (c) Tempest Foundation, 2025"; then
        return 0
    fi

    echo "Inserting header in: $file"
    local tmpfile="$file.tmp"

    cat > "$tmpfile" <<EOF
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
EOF
    cat "$file" >> "$tmpfile"
    mv "$tmpfile" "$file"
    changes_made=true
}

export -f process_file remove_header generate_header
export changes_made mode
find . -type f \( -name "*.cpp" -o -name "*.h" \) -print0 | while IFS= read -r -d '' file; do
    process_file "$file"
done

if [[ "$mode" = "generate" && "$changes_made" = false ]]; then
    echo "Nah, everything is good."
fi