#include "include/madt.h"
#include "include/console.h"
#include "include/drivers/pit.h"

struct rsdp *find_rsdp(void)
{
    u32 *addr;
    for (addr = (u32 *)0x000E0000; addr < (u32 *)0x00100000; addr += 4)
    {
        if (*addr == 0x20445352 && *(addr + 1) == 0x20525450)
        {
            return (struct rsdp *)addr;
        }
    }
    return NULL;
}

bool verify_checksum(void *table, u32 length)
{
    u8 sum = 0;
    for (u32 i = 0; i < length; i++)
    {
        sum += ((u8 *)table)[i];
    }
    return sum == 0;
}

void *find_madt(struct rsdp *rsdp)
{
    if (!rsdp)
    {
        printf("RSDP is NULL\n");
        return NULL;
    }

    printf("RSDP found at: 0x%08X\n", (u32)rsdp);
    printf("RSDT address: 0x%08X\n", rsdp->rsdt_address);

    if (!verify_checksum(rsdp, 20))
    {
        printf("RSDP checksum verification failed\n");
        return NULL;
    }

    struct rsdt *rsdt = (struct rsdt *)(uintptr_t)rsdp->rsdt_address;
    if (!rsdt)
    {
        printf("RSDT is NULL\n");
        return NULL;
    }

    printf("RSDT found at: 0x%08X\n", (u32)rsdt);

    if (!verify_checksum(rsdt, rsdt->header.length))
    {
        printf("RSDT checksum verification failed\n");
        return NULL;
    }

    u32 entries = (rsdt->header.length - sizeof(struct acpi_header)) / 4;
    printf("Number of RSDT entries: %d\n", entries);

    for (u32 i = 0; i < entries; i++)
    {
        struct acpi_header *header = (struct acpi_header *)(uintptr_t)rsdt->entry[i];
        printf("Table %d address: 0x%08X\n", i, (u32)header);

        if (header)
        {
            printf("Table %d signature: %.4s\n", i, header->signature);
            if (*(u32 *)header->signature == 0x43495041)
            {
                printf("MADT found at: 0x%08X\n", (u32)header);
                if (verify_checksum(header, header->length))
                {
                    return header;
                }
                else
                {
                    printf("MADT checksum verification failed\n");
                }
            }
        }
        else
        {
            printf("Table %d is NULL\n", i);
        }
    }

    printf("MADT not found in RSDT\n");
    return NULL;
}

bool check_apic_presence(struct madt *madt)
{
    u8 apic_presence = 0;
    u8 *entry = (u8 *)madt + sizeof(struct madt);
    u8 *end = (u8 *)madt + madt->header.length;

    while (entry < end)
    {
        struct madt_entry_header *header = (struct madt_entry_header *)entry;

        switch (header->type)
        {
        case 0: // Processor LAPIC
            apic_presence |= APIC_LOCAL_APIC_PRESENT;
            break;
        case 1: // I/O APIC
            apic_presence |= APIC_IO_APIC_PRESENT;
            break;
        }

        entry += header->length;
    }

    return apic_presence != 0;
}

void print_madt_info(struct madt *madt)
{
    if (!madt)
    {
        printf("Error: MADT pointer is NULL\n");
        return;
    }

    printf("MADT Information:\n");
    printf("  Signature: %.4s\n", madt->header.signature);
    printf("  Length: %u\n", madt->header.length);
    printf("  Revision: %u\n", madt->header.revision);
    printf("  Checksum: 0x%02X\n", madt->header.checksum);
    printf("  OEM ID: %.6s\n", madt->header.oem_id);
    printf("  OEM Table ID: %.8s\n", madt->header.oem_table_id);
    printf("  OEM Revision: %u\n", madt->header.oem_revision);
    printf("  Creator ID: 0x%08X\n", madt->header.creator_id);
    printf("  Creator Revision: %u\n", madt->header.creator_revision);
    printf("  LAPIC Address: 0x%08X\n", madt->local_apic_address);
    printf("  Flags: 0x%08X\n", madt->flags);

    bool apic_present = check_apic_presence(madt);

    if (apic_present != 0)
    {
        printf("Checking for APIC...\n");
    }

    if (apic_present)
    {
        printf("APIC is present\n");

        u32 offset = sizeof(struct madt);
        while (offset < madt->header.length)
        {
            struct madt_entry_header *entry = (struct madt_entry_header *)((u8 *)madt + offset);

            switch (entry->type)
            {
            case MADT_TYPE_LOCAL_APIC:
                printf("  Local APIC:\n");
                printf("    Processor ID: %u\n", *((u8 *)entry + 2));
                printf("    APIC ID: %u\n", *((u8 *)entry + 3));
                printf("    Flags: 0x%08X\n", *((u32 *)((u8 *)entry + 4)));
                break;
            case MADT_TYPE_IO_APIC:
                printf("  I/O APIC:\n");
                printf("    I/O APIC ID: %u\n", *((u8 *)entry + 2));
                printf("    I/O APIC Address: 0x%08X\n", *((u32 *)((u8 *)entry + 4)));
                printf("    Global System Interrupt Base: %u\n", *((u32 *)((u8 *)entry + 8)));
                sleep(5);
                console_clear(COLOR_WHITE, COLOR_BLACK);
                break;
            // case MADT_TYPE_INTERRUPT_OVERRIDE:
            //     printf("  Interrupt Source Override:\n");
            //     printf("    Bus: %u\n", *((u8 *)entry + 2));
            //     printf("    Source: %u\n", *((u8 *)entry + 3));
            //     printf("    Global System Interrupt: %u\n", *((u32 *)((u8 *)entry + 4)));
            //     printf("    Flags: 0x%04X\n", *((u16 *)((u8 *)entry + 8)));
            //     break;
            default:
                //TODO: Print unhandled MADT entry type
                //printf("  Unknown MADT entry type: %u\n", entry->type);
                break;
            }

            offset += entry->length;
        }
    }
    else
    {
        printf("No APIC found!\n");
        // TODO: Uncomment this when APIC is implemented
        //  printf("Falling back to PIC!\n");
    }
}

void probe_madt(void)
{
    struct rsdp *rsdp = find_rsdp();
    if (!rsdp)
    {
        printf("RSDP not found\n");
        return;
    }

    struct madt *madt = (struct madt *)find_madt(rsdp);
    if (madt)
    {
        printf("MADT found!\n");
        print_madt_info(madt);
    }
    else
    {
        printf("MADT not found!\n");
    }
}
