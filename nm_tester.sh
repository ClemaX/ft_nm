#!/usr/bin/env bash

trap '[ -z "$TMP_DIR" ] || rm -rf "$TMP_DIR"' EXIT

FT_NM="$(dirname "$0")/ft_nm"
BIN_DIR=/bin

UNAME=$(uname -s)

# Create temporary directory for fake path
export TMP_DIR=$(mktemp -d)

# Compile ft_nm if it does not exist
[ -f "$FT_NM" ] || make -C "$(dirname "$FT_NM")" NAME="$(basename "$FT_NM")"

# Link ft_nm to fake nm in temporary directory
ln -s "$PWD/$FT_NM" "$TMP_DIR/nm"

# Set diff colors
[ "$UNAME" != Darwin -a -t 1 ] && DIFF_FLAGS="--color=always"

# Compare nm outputs and status codes
nm_diff() # arguments
{
	local GREP=$(which grep)

	diff $DIFF_FLAGS \
		<(set -o pipefail; LC_COLLATE=C \
			nm "$1" 2>&1 | "$GREP" -v "bfd plugin" \
			|| echo "status: $?") \
		<(set -o pipefail; PATH="$TMP_DIR"; LC_COLLATE=C \
			nm "$1" 2>&1 | "$GREP" -v "bfd plugin" \
			|| echo "status: $?")
}

truncate --size 0 errors.log

for binary in $(find "$BIN_DIR/")
do
	DIFF=$(nm_diff "$binary") \
	|| (printf '%-24s:\n%s\n' "$binary" "$DIFF" && echo "$binary" >> errors.log)
done
