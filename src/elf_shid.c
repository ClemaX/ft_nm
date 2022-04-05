#include <libft.h>

#include <elf_map.h>

/**
 * @brief	Compare a section's attributes to an identification hint.
 * 
 * @param	hint	Identification hint.
 * @param	name	Section name.
 * @param	type	Section type.
 * @param	flags	Section flags.
 * @return	int		Zero if matching, one otherwise.
 */
static int  elf_map_shcmp(const t_elf_section_hint *hint, const char *name,
	unsigned int type, Elf64_Xword flags)
{
	/*
    ft_printf("%s: Hint flags: %llu, flags: %llu, match: %d\n",
        name, hint->flags, flags, (hint->flags & flags) == hint->flags);
    */
	return (!((hint->type == 0 || hint->type == type)
		&& (hint->flags & flags) == hint->flags
		&& (hint->name == NULL || ft_strcmp(hint->name, name) == 0)));
}

/**
 * @brief	Identify an ELF section given it's name, type and flags.
 * 
 * @param	name	Section name.
 * @param	type	Section type.
 * @param	flags	Section flags.
 * @return	char	Section identifier.
 */
char	    elf_map_shid(const char *name, unsigned int type, Elf64_Xword flags)
{
	static const t_elf_section_hint	hints[] = {
		{SHT_STRTAB,	0,											".strtab",	ELF_SHID_STRTAB},
		{SHT_SYMTAB,	0,											".symtab",	ELF_SHID_SYMTAB},
		{SHT_PROGBITS,	0,											".stab",	ELF_SHID_STAB},
		{SHT_NOBITS,	SHF_ALLOC | SHF_WRITE,						NULL,		ELF_SHID_BSS},
		{SHT_PROGBITS,	SHF_ALLOC | SHF_EXECINSTR,					NULL,		ELF_SHID_TEXT},
		{0,				SHF_ALLOC | SHF_WRITE | SHF_IA_64_SHORT,	NULL,		ELF_SHID_SDATA},
		{0,				SHF_ALLOC | SHF_WRITE,						NULL,		ELF_SHID_DATA},
		{0,				SHF_ALLOC,									NULL,		ELF_SHID_READONLY},
	};
	unsigned char	i;
	char			identifier;

	i = 0;
	while (i != sizeof(hints) / sizeof(*hints)
	&& elf_map_shcmp(hints + i, name, type, flags) != 0)
		i++;
	if (i == sizeof(hints) / sizeof(*hints))
		identifier = ELF_SHID_UNKNOWN;
	else
		identifier = hints[i].identifier;
	//ft_printf(" %c\n", identifier);
	return identifier;
}
