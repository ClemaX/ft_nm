

// TODO: 32 bit version
int	dump_elf_64(int fd, Elf64_Ehdr *elf_header)
{
	size_t		current_offset;
	ssize_t		ret;
	Elf64_Phdr	program_header;
	Elf64_Shdr	section_header;
	Elf64_Half	i;

	current_offset = sizeof(*elf_header);
	printf("Program header table offset: %lu\n", elf_header->e_phoff);
	printf("Program header entry count: %hu\n", elf_header->e_phnum);
	printf("Section header table offset: %lu\n", elf_header->e_shoff);
	printf("Section header string table index: %hu\n", elf_header->e_shstrndx);

	// Seek to program headers
	fake_seek(fd, elf_header->e_phoff - current_offset);

	// Iterate over program headers
	printf("Program headers:\n");
	i = 0;
	while (i < elf_header->e_phnum
	&& (ret = read_elf_64_ph(fd, &program_header)) != -1)
	{
		printf("%02d: %04lx\n", i, program_header.p_offset);
		current_offset += sizeof(program_header);
		i++;
	}

	// Seek to section headers
	fake_seek(fd, elf_header->e_shoff - current_offset);

	// Iterate over section headers
	printf("Section headers:\n");
	i = 0;
	while (i < elf_header->e_shnum
	&& (ret = read_elf_64_sh(fd, &section_header)) != -1)
	{
		printf("%02d: %04lx, %02x\n", i, section_header.sh_offset,
			section_header.sh_name);
		current_offset += sizeof(program_header);
		i++;
	}

	printf("Section header names:\n");

	// Seek to shstrndx
	i = 0;
	while (i < elf_header->e_shnum)
	{

	}

	return (i < elf_header->e_phnum ? ret : 0);
}
