//
// Created by Karim on 2024-11-18.
//

//- karim: basic
function void *
os_reserve(U64 size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

function B32
os_commit(void *ptr, U64 size)
{
    B32 result = (mprotect(ptr, size, PROT_READ | PROT_WRITE) == 0);
    return result;
}

function void
os_decommit(void *ptr, U64 size)
{
    mprotect(ptr, size, PROT_NONE);
}

function void
os_release(void *ptr, U64 size)
{
    munmap(ptr, size);
}

function U64 get_page_size(void) {
    return sysconf(_SC_PAGESIZE);
}