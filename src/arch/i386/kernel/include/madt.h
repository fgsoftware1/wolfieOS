#ifndef MADT_H
#define MADT_H

#include "libc/include/types.h"

#define MADT_TYPE_LOCAL_APIC 0
#define MADT_TYPE_IO_APIC 1
#define MADT_TYPE_INTERRUPT_OVERRIDE 2
#define MADT_TYPE_NMI_SOURCE 3
#define MADT_TYPE_LOCAL_APIC_NMI 4
#define MADT_TYPE_LOCAL_APIC_OVERRIDE 5
#define MADT_TYPE_IO_SAPIC 6
#define MADT_TYPE_LOCAL_SAPIC 7
#define MADT_TYPE_PLATFORM_INT_SOURCES 8
#define APIC_LOCAL_APIC_PRESENT 0x01
#define APIC_IO_APIC_PRESENT 0x02

struct acpi_header
{
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
};

struct rsdp
{
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_address;
    // ACPI 2.0+ fields
    u32 length;
    u64 xsdt_address;
    u8 extended_checksum;
    u8 reserved[3];
};

struct rsdt
{
    struct acpi_header header;
    u32 entry[0];
};

struct madt
{
    struct acpi_header header;
    u32 local_apic_address;
    u32 flags;
};

struct madt_entry_header
{
    u8 type;
    u8 length;
};

#endif // !MADT_H
