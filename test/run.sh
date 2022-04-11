#!/usr/bin/env bash

set -eu

TEST_DIR=$(dirname "$0")

FT_NM="$TEST_DIR/../ft_nm"

ARCHITECTURES=("elf64" "elf32")

AS=nasm
CC=clang

CFLAGS="-Wall -Wextra -Wno-unused-command-line-argument"

UNAME=$(uname -s)

# Create temporary directory for fake path
export TMP_DIR=$(mktemp -d)

# Compile ft_nm if it does not exist
[ -f "$FT_NM" ] || make -C "$(dirname "$FT_NM")" NAME="$(basename "$FT_NM")"

# Link ft_nm to fake nm in temporary directory
ln -s "$PWD/$FT_NM" "$TMP_DIR/nm"

# Compare nm outputs and status codes
nm_diff() # arguments
{
	local GREP=$(which grep)

	diff $DIFF_FLAGS \
		<(set -o pipefail; PATH="$TMP_DIR"; LC_COLLATE=C \
			nm "$@" 2>&1 | "$GREP" -v "bfd plugin" \
			|| echo "status: $?") \
		<(set -o pipefail; LC_COLLATE=C \
			nm "$@" 2>&1 | "$GREP" -v "bfd plugin" \
			|| echo "status: $?")
}

unit() # arch name args
{
	local arch="$1"; shift
	local name="$1"; shift


	printf "$COLOR_ARCH%-5s$COLOR_RESET %-23s " "$arch" "$name"

	DIFF=$(nm_diff "$@") \
	&& printf "$COLOR_PASS%s$COLOR_RESET\n" '✓' \
	|| printf "$COLOR_FAIL%s$COLOR_RESET\n%s\n" '✗' "$DIFF"
}

if [ -t 1 ]
then
	COLOR_ARCH='\e[36m'
	COLOR_PASS='\e[32m'
	COLOR_FAIL='\e[31m'
	COLOR_RESET='\e[0m'
	[ "$UNAME" != Darwin ] && DIFF_FLAGS="--color=always" || DIFF_FLAGS=
else
	COLOR_ARCH= COLOR_PASS= COLOR_FAIL= COLOR_RESET=
	DIFF_FLAGS=
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

		unit "$arch" "$(basename "$test")" "$OUTFILE"

		$CLEANUP >/dev/null
	done
	echo
done

FT_NM_ELFCLASS=$(od -An -t d1 -j 4 -N 1 "$FT_NM")
case "$FT_NM_ELFCLASS" in
	*2)	FT_NM_ARCH=elf64;;
	*1)	FT_NM_ARCH=elf32;;
	*)	FT_NM_ARCH=none;;
esac

unit "$FT_NM_ARCH" "90-no_arguments"

unit "$FT_NM_ARCH" "91-debug_syms" --debug-syms "$FT_NM"

unit "$FT_NM_ARCH" "92-extern_only" --extern-only "$FT_NM"

unit "$FT_NM_ARCH" "93-undefined_only" --undefined-only "$FT_NM"

unit "$FT_NM_ARCH" "94-reverse_sort" --reverse-sort "$FT_NM"

unit "$FT_NM_ARCH" "95-no_sort" --no-sort "$FT_NM"
