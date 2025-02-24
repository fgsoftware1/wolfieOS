#ifndef ACPI_H
#define ACPI_H

#include "libc/include/types.h"

// ACPI table header structure
typedef struct {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

// Root System Description Pointer (RSDP) structure
typedef struct {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_address;
} __attribute__((packed)) acpi_rsdp_t;

// Extended System Description Table (XSDT) structure
typedef struct {
    acpi_sdt_header_t header;
    u64 entry[1];
} __attribute__((packed)) acpi_xsdt_t;

// Function declarations
bool acpi_init(void);
void *acpi_find_table(const char *signature);
void probe_acpi(void);

#endif // ACPI_H
