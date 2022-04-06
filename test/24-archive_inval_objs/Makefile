NAME	=	test.a
SRCDIR	=	src
OBJDIR	=	obj
ASM		=	nasm
AFLAGS	=
CC		=	/usr/bin/clang
CFLAGS	=	-Wall -Wextra -Werror
AR		=	/usr/bin/ar
ARFLAGS	=	rcs
RM		=	/bin/rm
FORMAT	=	elf64
FLAGS	=
UNITS	=	function_a function_b function_c
SRCS	=	$(addprefix $(SRCDIR)/, $(addsuffix .s, $(UNITS)))
OBJS	=	$(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(SRCS))

all:			$(NAME)

$(NAME):		$(OBJS)
	@printf "%-3s $@ $(OBJS)\n" AR
	@$(AR) $(ARFLAGS) $@ $(OBJS)

$(OBJDIR):
	@printf "%-3s $@\n" MK
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.s $(OBJDIR) Makefile
	@printf "%-3s $<\n" ASM
	@$(ASM) $(FLAGS) $(AFLAGS) -f$(FORMAT) $< -o $@
	echo -n "4" | dd of=$@ bs=1 seek=4 count=1 conv=notrunc 2>&1

clean:
	@printf "%-3s $(OBJDIR)\n" RM
	@$(RM) -rf $(OBJDIR)

fclean:			clean
	@printf "%-3s $(NAME) test\n" RM
	@$(RM) -rf $(NAME) test

re:				fclean all
