/* included to the .bss section */

*(.bss_begin .bss_begin.*)
*(.bss .bss.*)
*(.gnu.linkonce.b.*)
*(COMMON)
*(.bss_end .bss_end.*)

*(.dynsbss)
*(.sbss .sbss.* .gnu.linkonce.sb.*)
*(.scommon)
*(.dynbss)
