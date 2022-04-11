#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <libft.h>

#include <ft_nm.h>

static const char	*option_descriptions[] =
{
	"Display this information",
	"Display debugger-only symbols",
	"Display only external symbols",
	"Display only undefined (external) symbols",
	"Reverse the order of the sort",
};

static const char	*option_names[] =
{
	"help",
	"debug-syms",
	"extern-only",
	"undefined-only",
	"reverse-sort",
	"no-sort",
};

static const char	option_short_names[] =
{
	'h',
	'a',
	'g',
	'u',
	'r',
	'p',
};

static t_elf_opt	get_long_option(const char *name)
{
	t_elf_opt			option;

	option = 0;
	while ((unsigned)option < sizeof(option_names) / sizeof(*option_names) 
	&& ft_strcmp(option_names[option], name) != 0)
		option++;
	if (option != sizeof(option_names) / sizeof(*option_names))
		option = 1 << option;
	else
		option = ELF_OERROR;
	return (option);
}

static t_elf_opt	get_short_option(char c)
{
	const int	i = ft_strpos(option_short_names, c);
	t_elf_opt	option;

	if (i != -1)
		option = 1 << i;
	else
		option = ELF_OERROR;
	return (option);

}

static t_elf_opt	get_options(int *i , const char *const *const av)
{
	t_elf_opt	options;
	t_elf_opt	option;
	size_t		j;

	options = 0;
	j = 0;
	while (options != ELF_OERROR && av[*i] != NULL && av[*i][j] == '-')
	{
		++j;
		if (av[*i][j] == '-')
		{
			++j;
			option = get_long_option(av[*i] + j);
			if (option != ELF_OERROR)
				options |= option;
			else
				options = option;
		}
		else
		{
			while (option != ELF_OERROR && av[*i][j] != '\0')
			{
				option = get_short_option(av[*i][j]);
				if (option != ELF_OERROR)
				{
					options |= option;
					++j;
				}
				else
					options = option;
			}
		}
		(*i)++;
		j = 0;
	}
	return (options);
}

static void	print_usage(const char *prog)
{
	const char	*fmt;
	unsigned	i;

	ft_printf(
"Usage: %s [OPTION]... [FILE]...\n\
\
List symbols in [FILE]... (a.out by default).\n\n\
The options are:\n\
", prog
	);
	i = 0;
	while (i < sizeof(option_short_names))
	{
		if (option_names[i] == NULL)
			fmt =  " -%c%-22.0s%s\n";
		else
			fmt = " -%c, --%-18s%s\n";
		ft_dprintf(2, fmt, option_short_names[i], option_names[i],
			option_descriptions[i]);
		i++;
	}
}

// TODO: Use 'a.out' as default objfile argument
// TODO: Handle big-endian encoding
// TODO: Handle 32-bit files (using unions?)
int			main(int ac, const char *const *const av)
{
	int			i;
	int			ret;
	t_elf_opt	options;
	t_elf_err	err;

	i = 1;
	ret = 0;
	err = 0;
	if (ac == 1)
		ac++;
	options = get_options(&i, av);
	ret = options == ELF_OERROR;
	if (ret == 0 && (options & ELF_OHELP) == 0)
	{
		while (ret != -1 && i < ac)
		{
			err = ft_nm(av[i], options, av[0]);
			if (err == ELF_ESYS)
				ret = -1;
			else if (err != ELF_EOK && err != ELF_ENOSYMS)
				ret += 1;
			i++;
		}
		if (ret == -1)
			ret = -ret;
	}
	else
		print_usage(av[0]);
	return (ret);
}
