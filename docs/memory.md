# QEMU Memory Management

The memory allocated by the QEMU process is organized in MemoryRegion objects, each representing a contiguous region of memory. All memory that is available to the guest is organized in RAMBlocks, which, for example, represent the main memory or video memory, and each occupy one MemoryRegion. The RAMBlocks are organized as a linked list, which
allows iterating over them.

```
typedef struct RAMBlock {
    struct MemoryRegion *mr;
    uint8_t *host;
    ram_addr_t offset;
    ram_addr_t length;
    char idstr[256];
    QLIST_ENTRY(RAMBlock) next;
    ...
} RAMBlock;

struct MemoryRegion {
    ...
    ram_addr_t ram_addr;
    ...
};
```

`|ram_addr_t|` is used as guest RAM memory offsets.
![RAMBlock](https://github.com/wangchenghku/qemu/blob/master/docs/ramblock.png)

Inside these RAMBlocks, pages can be accessed using the base address of a RAMBlock’s MemoryRegion and an offset to the actual page, which is a multiple of TARGET_PAGE_SIZE (i.e., 4KiB for x86 and x86-64 guests).

```
MemoryRegion *mr;
mr = block->mr;
addr = memory_region_get_ram_ptr(mr); // return a host pointer to a RAM memory region.
addr + offset;
```

send to peer
```
/* write the block identification */
static void save_block_hdr(QEMUFile *f, RAMBlock *block, ram_addr_t offset, int cont, int flag)
{
        qemu_put_be64(f, offset | cont | flag);
        if (!cont) {
                qemu_put_byte(f, strlen(block->idstr));
                qemu_put_buffer(f, (uint8_t *)block->idstr,
                                strlen(block->idstr));
        }
}

/* write a page of memory */
qemu_put_buffer(f, p, TARGET_PAGE_SIZE);

```
