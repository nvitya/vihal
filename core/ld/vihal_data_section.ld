/* included to the .data section */

*(.data_begin .data_begin.*)
*(.data .data.* .gnu.linkonce.d.*)
*(.data_end .data_end.*)

*(.sdata2 .sdata2.* .gnu.linkonce.s2.*)

SORT(CONSTRUCTORS)

*(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
*(.sdata .sdata.* .gnu.linkonce.s.*)
*(.tdata .tdata.* .gnu.linkonce.td.*)
