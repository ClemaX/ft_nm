#!/usr/bin/env sh

trap '[ -z "$TMP_DIR" ] || rm -rf "$TMP_DIR"' EXIT

FT_NM=./ft_nm
BIN_DIR=/bin

# Sort using lexicographical comparison 
export LC_COLLATE=C

export TMP_DIR=$(mktemp -d)

ln -s "$PWD/$FT_NM" "$TMP_DIR/nm"

for binary in $(find "$BIN_DIR/")
do
	diff <(nm "$binary" 2>&1 || echo "status: $?") <(PATH="$TMP_DIR"; nm "$binary" 2>&1 || echo "status: $?") || echo "with '$binary'"
done
