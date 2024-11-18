//
// Created by Abdik on 2024-11-18.
//

#ifndef OS_CORE_H
#define OS_CORE_H

//- karim: basic
function void *os_reserve(U64 size);
function B32   os_commit(void *ptr, U64 size);
function void  os_decommit(void *ptr, U64 size);
function void  os_release(void *ptr, U64 size);

#endif //OS_CORE_H
