//#include <ft_elf.h>
#include <elf.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>

#include <fcntl.h>

typedef struct __attribute__((__packed__))
{
	// 0x7F followed by ELF (45 4C 46)
	uint32_t	magic_number;
	// 1 for 32 or 2 for 64 bit format
	uint8_t		file_class;
	// 1 for little or 2 for big endianness
	uint8_t		endianness;
	// 1 for the current version
	uint8_t		elf_version;
	// ABI Variant
	uint8_t		abi;
	// ABI Version
	uint8_t		abi_version;
	char		padding[7];
}						t_ident;

typedef union elf
{
	struct
	{
		unsigned char	ident[EI_NIDENT];
		unsigned char	data;
	}	raw;
	t_ident			ident;
	Elf32_Ehdr		h32;
	Elf64_Ehdr		h64;
}			t_elf;


int	read_elf_ident(int fd, t_elf *elf_header)
{
	return (read(fd, &elf_header->raw.ident, sizeof(elf_header->ident)));
}

int	read_elf_32(int fd, t_elf *elf_header)
{
	return (read(fd, &elf_header->raw.data, sizeof(elf_header->h32) - EI_NIDENT));
}

int	read_elf_64(int fd, t_elf *elf_header)
{
	return (read(fd, &elf_header->raw.data, sizeof(elf_header->h64) - EI_NIDENT));
}

int	read_elf(int fd, t_elf *elf_header)
{
	int		ret;

	if ((ret = read_elf_ident(fd, elf_header)) == -1)
		perror("read");
	else if (elf_header->ident.file_class == 1)
	{
		if ((ret = read_elf_32(fd, elf_header)) == -1)
			perror("read_elf_32");
	}
	else if (elf_header->ident.file_class == 2)
	{
		if ((ret = read_elf_64(fd, elf_header)) == -1)
			perror("read_elf_64");
	}
	else
	{
		write(STDERR_FILENO, "Invalid elf!\n", 13);
		ret = -2;
	}
	return (ret);
}

int	read_elf_32_sh(int fd, Elf32_Shdr *section_header)
{
	return (read(fd, section_header, sizeof(*section_header)));
}

int	read_elf_64_sh(int fd, Elf64_Shdr *section_header)
{
	return (read(fd, section_header, sizeof(*section_header)));
}

int	read_elf_32_ph(int fd, Elf32_Phdr *program_header)
{
	return (read(fd, program_header, sizeof(*program_header)));
}

int	read_elf_64_ph(int fd, Elf64_Phdr *program_header)
{
	return (read(fd, program_header, sizeof(*program_header)));
}

int	fake_seek(int fd, size_t bytes)
{
	static char	buffer[64];
	char		toRead;
	ssize_t		ret;

	ret = 1;
	printf("Seeking forward %zu bytes.\n", bytes);
	while (bytes && ret > 0)
	{
		toRead = bytes > sizeof(buffer) ? sizeof(buffer) : bytes;
		ret = read(fd, buffer, toRead);
		bytes -= toRead;
	}
	return (ret);
}
