#include "include/acpi.h"
#include "libc/include/string.h"
#include "libc/include/stdio.h"

static acpi_rsdp_t *rsdp = NULL;
static acpi_xsdt_t *xsdt = NULL;

static bool validate_table(acpi_sdt_header_t *table_header) {
    u8 sum = 0;
    for (u32 i = 0; i < table_header->length; i++) {
        sum += ((u8 *) table_header)[i];
    }
    return sum == 0;
}

bool acpi_init(void) {
    // Search for the RSDP
    for (u8 *addr = (u8 *)0x000E0000; addr < (u8 *)0x00100000; addr += 16) {
        if (memcmp(addr, "RSD PTR ", 8) == 0) {
            rsdp = (acpi_rsdp_t *)addr;
            break;
        }
    }

    if (!rsdp) {
        printf("ACPI: RSDP not found\n");
        return false;
    }

    // Validate RSDP checksum
    u8 sum = 0;
    for (int i = 0; i < sizeof(acpi_rsdp_t); i++) {
        sum += ((u8 *)rsdp)[i];
    }
    if (sum != 0) {
        printf("ACPI: RSDP checksum invalid\n");
        return false;
    }

    // Get XSDT
    xsdt = (acpi_xsdt_t *)(u32)rsdp->rsdt_address;

    if (!validate_table(&xsdt->header)) {
        printf("ACPI: XSDT checksum invalid\n");
        return false;
    }

    printf("ACPI: Initialization successful\n");
    return true;
}

void *acpi_find_table(const char *signature) {
    u32 entries = (xsdt->header.length - sizeof(acpi_sdt_header_t)) / sizeof(u64);

    for (u32 i = 0; i < entries; i++) {
        acpi_sdt_header_t *header = (acpi_sdt_header_t *)(u32)xsdt->entry[i];
        if (memcmp(header->signature, signature, 4) == 0) {
            if (validate_table(header)) {
                return header;
            } else {
                printf("ACPI: Found table %.4s but checksum invalid\n", signature);
                return NULL;
            }
        }
    }

    printf("ACPI: Table %.4s not found\n", signature);
    return NULL;
}

void probe_acpi(void) {
    printf("ACPI: Probing tables\n");
    printf("ACPI: RSDP found at 0x%x\n", (u32)rsdp);
    printf("ACPI: XSDT found at 0x%x\n", (u32)xsdt);

    u32 entries = (xsdt->header.length - sizeof(acpi_sdt_header_t)) / sizeof(u64);
    printf("ACPI: XSDT contains %d entries\n", entries);

    for (u32 i = 0; i < entries; i++) {
        acpi_sdt_header_t *header = (acpi_sdt_header_t *)(u32)xsdt->entry[i];
        printf("ACPI: Found table %.4s at 0x%x\n", header->signature, (u32)header);
    }
}
