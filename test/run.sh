#!/usr/bin/env bash

set -eu

TEST_DIR=$(dirname "$0")

FT_NM="$TEST_DIR/../ft_nm"

ARCHITECTURES=("elf64" "elf32")

AS=nasm
CC=clang

CFLAGS="-Wall -Wextra -Wno-unused-command-line-argument"

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

for arch in "${ARCHITECTURES[@]}"
do
	COMPILE_s="$AS -f$arch"
	COMPILE_c="$CC $CFLAGS -arch$arch"

	for test in "$TEST_DIR/"*/
	do
		if [ -f "$test/test.s" ]
		then
			$COMPILE_s "$test/test.s" -o test.o
		elif [ -f "$test/test.c" ]
		then
			$COMPILE_c -c "$test/test.c" -o test.o
		else
			continue
		fi

		printf '%-5s %-23s ' "$arch" "$(basename "$test")" >&2

		nm_diff test.o && echo '✓' || :

		rm test.o
	done
done