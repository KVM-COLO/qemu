# QEMU Memory Management

All memory that is available to the guest is organized in RAMBlocks, which, for example, represent the main memory or video memory. The RAMBlocks are organized as a linked list, which
allows iterating over them.

```
struct RAMBlock {
    uint8_t *host;
    ram_addr_t offset;
    ram_addr_t used_length;
    char idstr[256];
    QLIST_ENTRY(RAMBlock) next;
    ...
};
```
`|ram_addr_t|` is used as guest RAM memory offsets.
![RAMBlock](https://github.com/wangchenghku/qemu/blob/master/docs/ramblock.png)

send to peer
```
/* Write page header to wire */
static size_t save_page_header(QEMUFile *f, RAMBlock *block, ram_addr_t offset)
{
    size_t size, len;

    qemu_put_be64(f, offset);
    size = 8;

    if (!(offset & RAM_SAVE_FLAG_CONTINUE)) {
        len = strlen(block->idstr);
        qemu_put_byte(f, len);
        qemu_put_buffer(f, (uint8_t *)block->idstr, len);
        size += 1 + len;
    }
    return size;
}

qemu_put_buffer(f, p, TARGET_PAGE_SIZE);

```
