//
// Created by Karim on 2024-11-18.
//

#ifndef OS_CORE_H
#define OS_CORE_H

//- karim: memory
function void *os_reserve(U64 size);
function B32   os_commit(void *ptr, U64 size);
function void  os_decommit(void *ptr, U64 size);
function void  os_release(void *ptr, U64 size);
function U64 get_page_size(void);

//- karim: time
function U32 os_get_microseconds();

//- karim: threading
function U32 os_get_current_thread_id();

#endif //OS_CORE_H
