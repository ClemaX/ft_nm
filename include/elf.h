#ifndef ELF_H
# define ELF_H

# include <stdint.h>

// reference: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

// identity header

struct __attribute__((__packed__)) s_ident
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
};

// file header

struct __attribute__((__packed__)) s_elf_32
{
	// object file type
	uint16_t	e_type;
	// instruction set architecture
	uint16_t	e_machine;
	// 1 for the current version
	uint32_t	e_version;
	// entry point address
	uint32_t	e_entry;
	// program header table address
	uint32_t	e_phoff;
	// section header table address
	uint32_t	e_shoff;
	// architecture dependent flags
	uint32_t	e_flags;
	// header size (52)
	uint16_t	e_ehsize;
	// program header table entry size
	uint16_t	e_phentsize;
	// number of entries in the program header table
	uint16_t	e_phnum;
	// section header table entry size
	uint16_t	e_shentsize;
	// number of entries in the section header table
	uint16_t	e_shnum;
	// Index of the section header table's section names
	uint16_t	e_shstrndx;
};

struct __attribute__((__packed__)) s_elf_64
{
	// object file type
	uint16_t	e_type;
	// instruction set architecture
	uint16_t	e_machine;
	// 1 for the current version
	uint32_t	e_version;
	// entry point address
	uint64_t	e_entry;
	// program header table offset
	uint64_t	e_phoff;
	// section header table offset
	uint64_t	e_shoff;
	// architecture dependent flags
	uint32_t	e_flags;
	// header size (64)
	uint16_t	e_ehsize;
	// program header table entry size
	uint16_t	e_phentsize;
	// number of entries in the program header table
	uint16_t	e_phnum;
	// section header table entry size
	uint16_t	e_shentsize;
	// number of entries in the section header table
	uint16_t	e_shnum;
	// index of the section header table's section names
	uint16_t	e_shstrndx;
};

// program header

struct __attribute__((__packed__)) s_elf_32_ph
{
	// type of the segment
	uint32_t	p_type;
	// offset of the segment in the file
	uint32_t	p_offset;
	// virtual address of the segment in memory
	uint32_t	p_vaddr;
	// phyisical address of the segment in memory (if present)
	uint32_t	p_paddr;
	// segment size in bytes in the file
	uint32_t	p_filesz;
	// segment size in bytes in memory
	uint32_t	p_memsz;
	// segment dependant flags
	uint32_t	p_flags;
	// 0 and 1 specify no alignment. Otherwise a positive int power of 2,
	// with p_vaddr == p_offset % p_align
	uint32_t	p_align;
};

struct __attribute__((__packed__)) s_elf_64_ph
{
	// type of the segment
	uint32_t	p_type;
	// segment dependant flags
	uint32_t	p_flags;
	// offset of the segment in the file image
	uint64_t	p_offset;
	// virtual address of the segment in memory
	uint64_t	p_vaddr;
	// phyisical address of the segment in memory (if present)
	uint64_t	p_paddr;
	// segment size in bytes in the file
	uint64_t	p_filesz;
	// segment size in bytes in memory
	uint64_t	p_memsz;
	// TODO: flags 32
	// 0 and 1 specify no alignment. Otherwise a positive int power of 2,
	// with p_vaddr == p_offset % p_align
	uint64_t	p_align;
};

// section header

struct __attribute__((__packed__)) s_elf_32_sh
{
	// offset of the string representing the name in the .shstrtab secion
	uint32_t	sh_name;
	// type of the section
	uint32_t	sh_type;
	// attributes of the section
	uint32_t	sh_flags;
	// virtual address of the section in memory (if present)
	uint32_t	sh_addr;
	// offset of the section in the file image
	uint32_t	sh_offset;
	// size in bytes of the section in the file image
	uint32_t	sh_size;
	// section index of an associated section, depending on the section type
	uint32_t	sh_link;
	// the required alignment for the section (must be a power of 2)
	uint32_t	sh_addralign;
	// size in bytes of each entry (if present)
	uint32_t	sh_entsize;
};


struct __attribute__((__packed__)) s_elf_64_sh
{
	// offset of the string representing the name in the .shstrtab section
	uint32_t	sh_name;
	// type of the section
	uint32_t	sh_type;
	// attributes of the section
	uint32_t	sh_flags;
	// virtual address of the section in memory (if present)
	uint64_t	sh_addr;
	// offset of the section in the file image
	uint64_t	sh_offset;
	// size in bytes of the section in the file image
	uint64_t	sh_size;
	// section index of an associated section, depending on the section type
	uint32_t	sh_link;
	// extra information about the section
	uint32_t	sh_info;
	// the required alignment for the section (must be a power of 2)
	uint64_t	sh_addralign;
	// size in bytes of each entry (if present)
	uint64_t	sh_entsize;
};

#endif
