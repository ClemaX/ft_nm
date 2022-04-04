#!/usr/bin/env bash

set -eu

TEST_DIR=$(dirname "$0")

FT_NM="$TEST_DIR/../ft_nm"

# Sort using lexicographical comparison
export LC_COLLATE=C

export TMP_DIR=$(mktemp -d)

ln -s "$PWD/$FT_NM" "$TMP_DIR/nm"

nm_diff() # object
{
	diff \
		<(nm "$1" 2>&1 || echo "status: $?") \
		<(PATH="$TMP_DIR"; nm "$1" 2>&1 || echo "status: $?")
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
