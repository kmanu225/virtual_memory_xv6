OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

PHDRS
{
headers PT_PHDR PHDRS ;
text PT_LOAD FILEHDR PHDRS ;
data PT_LOAD ;
rodata PT_LOAD ;
bss PT_LOAD ;
}

SECTIONS
{
. = SIZEOF_HEADERS;
. = ALIGN(0x1000);
.text :
{ *(.text) } :text

. = ALIGN(0x1000);
.rodata : { *(.rodata) *(.rodata*) } :rodata

. = ALIGN(0x1000);
.data : { *(.data) } :data

. = ALIGN(0x1000);
.bss : { *(.bss) *(.sbss*) } :bss

}

