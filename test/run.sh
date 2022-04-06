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
	diff "--color=$DIFF_COLOR" \
		<(LC_COLLATE=C nm "$1" 2>&1 || echo "status: $?") \
		<(PATH="$TMP_DIR"; LC_COLLATE=C nm "$1" 2>&1 || echo "status: $?")
}

if [ -t 1 ]
then
	COLOR_ARCH='\e[36m'
	COLOR_PASS='\e[32m'
	COLOR_FAIL='\e[31m'
	COLOR_RESET='\e[0m'
	DIFF_COLOR=always
else
	COLOR_ARCH= COLOR_PASS= COLOR_FAIL= COLOR_RESET=
	DIFF_COLOR=never
fi

OUTFILE="$TMP_DIR/test.o"

for arch in "${ARCHITECTURES[@]}"
do
	COMPILE_s="$AS -f$arch -o $OUTFILE"
	COMPILE_c="$CC $CFLAGS -arch$arch -o $OUTFILE"
	MAKE="make NAME=$OUTFILE FORMAT=$arch"

	CLEANUP="rm $OUTFILE"

	for test in "$TEST_DIR/"*/
	do
		if [ -f "$test/test.s" ]
		then
			$COMPILE_s "$test/test.s"
		elif [ -f "$test/test.c" ]
		then
			$COMPILE_c -c "$test/test.c"
		elif [ -f "$test/Makefile" ]
		then
			$MAKE -C "$test" >/dev/null
			CLEANUP="$MAKE -C "$test" fclean"
		else
			continue
		fi

		printf "$COLOR_ARCH%-5s$COLOR_RESET %-23s " "$arch" "$(basename "$test")"

		DIFF=$(nm_diff $OUTFILE) \
		&& printf "$COLOR_PASS%s$COLOR_RESET\n" '✓' \
		|| printf "$COLOR_FAIL%s$COLOR_RESET\n%s\n" '✗' "$DIFF"

		$CLEANUP >/dev/null
	done
done