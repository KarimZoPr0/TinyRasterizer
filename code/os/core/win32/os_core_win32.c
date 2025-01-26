//
// Created by Karim on 2024-11-18.
//

//- karim: basic
function void*
os_reserve(U64 size)
{
    void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

function B32
os_commit(void* ptr, U64 size)
{
    B32 result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
    return result;
}

function void
os_decommit(void* ptr, U64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void
os_release(void* ptr, U64 size)
{
    VirtualFree(ptr, 0, MEM_RELEASE);
}

static U64 get_page_size(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
}

FILETIME get_last_write_time(char* filename)
{
    FILETIME LastWriteTime = {0};
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    {
        LastWriteTime = data.ftLastWriteTime;
    }

    return LastWriteTime;
}
