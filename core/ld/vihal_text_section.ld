/* must be included into main code section */

KEEP(*(.init))
KEEP(*(.fini))

. = ALIGN(4);

PROVIDE_HIDDEN (__preinit_array_start = .);
KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
KEEP(*(.preinit_array_platform .preinit_array_platform.*))
KEEP(*(.preinit_array .preinit_array.*))
PROVIDE_HIDDEN (__preinit_array_end = .);

. = ALIGN(4);

PROVIDE_HIDDEN (__init_array_start = .);
KEEP(*(SORT(.init_array.*)))
KEEP(*(.init_array))
PROVIDE_HIDDEN (__init_array_end = .);

. = ALIGN(4);

PROVIDE_HIDDEN (__fini_array_start = .);
KEEP(*(SORT(.fini_array.*)))
KEEP(*(.fini_array))
PROVIDE_HIDDEN (__fini_array_end = .);

*(.rela.iplt)
*(.rela.plt)

*(.text .text.* )

*(.text.unlikely .text.*_unlikely .text.unlikely.*)
*(.text.exit .text.exit.*)
*(.text.startup .text.startup.*)
*(.text.hot .text.hot.*)
*(SORT(.text.sorted.*))
*(.stub)
*(.gnu.warning)

*(.srodata .srodata.*)
*(.rodata .rodata.* .constdata .constdata.* .gnu.linkonce.r.*)
*(.ARM.extab* .gnu.linkonce.armextab.*)
*(vtable)

KEEP(*(.eh_frame*))
*(.gnu.linkonce.t.*)

*(.glue_7) *(.glue_7t)
*(.gcc_except_table)

. = ALIGN(4);
KEEP (*crtbegin.o(.ctors))
KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
KEEP (*(SORT(.ctors.*)))
KEEP (*crtend.o(.ctors))
