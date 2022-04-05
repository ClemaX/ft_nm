#!/usr/bin/env bash

set -eu

TEST_DIR=$(dirname "$0")

FT_NM="$TEST_DIR/../ft_nm"

# Create temporary directory for fake path
export TMP_DIR=$(mktemp -d)

# Compile ft_nm if it does not exist
[ -f "$FT_NM" ] || make -C "$(dirname "$FT_NM")" NAME="$(basename "$FT_NM")"

# Link ft_nm to fake nm in temporary directory
ln -s "$PWD/$FT_NM" "$TMP_DIR/nm"

# Compare nm outputs and status codes
nm_diff() # arguments
{
	diff \
		<(LC_COLLATE=C nm "$1" 2>&1 || echo "status: $?") \
		<(PATH="$TMP_DIR"; LC_COLLATE=C nm "$1" 2>&1 || echo "status: $?")
}

for test in "$TEST_DIR/"*/
do
	if [ -f "$test/test.s" ]
	then
		nasm -f elf64 "$test/test.s" -o test.o
	elif [ -f "$test/test.c" ]
	then
		clang -c "$test/test.c" -o test.o
	else
		continue
	fi

	printf %-24s "$(basename "$test") " >&2

	nm_diff test.o && echo '✓' || :

	rm test.o
done
