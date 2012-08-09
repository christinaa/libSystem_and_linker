/*
 * core: gcc_atomic.s
 * Copyright (c) 2012 Christina Brooks
 *
 * Fix for clang thinking these are built-ins.
 */

#define declare_shim(x) .globl _##x; \
_##x: \
b ___clang$shim$##x;

.text
.align 2

declare_shim(__sync_fetch_and_sub_4)
declare_shim(__sync_fetch_and_add_4)
declare_shim(__sync_fetch_and_or_4)
declare_shim(__sync_fetch_and_and_4)
declare_shim(__sync_fetch_and_xor_4)
declare_shim(__sync_add_and_fetch_4)
declare_shim(__sync_sub_and_fetch_4)
declare_shim(__sync_or_and_fetch_4)
declare_shim(__sync_and_and_fetch_4)
declare_shim(__sync_xor_and_fetch_4)
declare_shim(__sync_bool_compare_and_swap_4)
declare_shim(__sync_val_compare_and_swap_4)
declare_shim(__sync_lock_test_and_set_4)
declare_shim(__sync_lock_release_4)

