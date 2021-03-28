# ft_nm
List symbols from object files.

## Executable Linkable Format (ELF)

The ELF is a cross-platform standard for executables, objects, libraries and
core dumps.

The ELF header is composed of an identity header, which defines the file's ABI
and architecture, followed by an architecture dependant file-header.

The file-header specifies the type, flags, entry-point, and instruction set of
the object as well as the position and dimensions of the program- and
section-header tables, as well as the `.shstrtab` section-header's index.

Program headers define segment dimensions and positions in the file and in
memory, as well as the segment's type.

Section headers define section dimensions and positions in the file and in
memory, as well as the section's type and pointers to strings representing the
name in the special `.shstrtab` section.

This diagram shows the layout and loading process of a simple `Hello world!`
program:
![ELF diagram][elfDiagram]

## Memory Mapped IO
TODO

[elfDiagram]: https://upload.wikimedia.org/wikipedia/commons/e/e4/ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini.png
