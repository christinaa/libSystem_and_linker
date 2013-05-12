/*
 * core: Unimplemented.c
 * Copyright (c) 2012 Christina Brooks
 *
 * 
 */

#include <stdio.h>

/*
 [dyldwarn]: [resolve, 156]: can't resolve '__NSGetEnviron' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 157]: can't resolve '___cmpdi2' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 158]: can't resolve '_chflags' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 159]: can't resolve '_class_createInstanceFromZone' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 160]: can't resolve '_copyfile' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 161]: can't resolve '_fgetxattr' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 162]: can't resolve '_freehostent' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 163]: can't resolve '_freeifaddrs' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 164]: can't resolve '_fsetxattr' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 165]: can't resolve '_fsync' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 166]: can't resolve '_ftruncate' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 167]: can't resolve '_getattrlist' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 168]: can't resolve '_getifaddrs' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 169]: can't resolve '_getipnodebyname' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 170]: can't resolve '_getnameinfo' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 171]: can't resolve '_mach_absolute_time' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 172]: can't resolve '_malloc_get_zone_name' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 173]: can't resolve '_object_copyFromZone' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 174]: can't resolve '_pthread_cond_destroy' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 175]: can't resolve '_pthread_cond_init' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 176]: can't resolve '_pthread_cond_signal' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 177]: can't resolve '_pthread_cond_timedwait_relative_np' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 178]: can't resolve '_pthread_exit' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 179]: can't resolve '_pthread_is_threaded_np' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 180]: can't resolve '_pthread_rwlock_destroy' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 181]: can't resolve '_pthread_rwlock_init' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 182]: can't resolve '_pthread_rwlock_rdlock' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 183]: can't resolve '_pthread_rwlock_tryrdlock' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 184]: can't resolve '_pthread_rwlock_trywrlock' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 185]: can't resolve '_pthread_rwlock_unlock' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 186]: can't resolve '_pthread_rwlock_wrlock' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 187]: can't resolve '_pthread_setschedparam' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 188]: can't resolve '_removefile' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 189]: can't resolve '_removefile_state_alloc' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 190]: can't resolve '_removefile_state_free' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 191]: can't resolve '_removefile_state_get' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 192]: can't resolve '_removefile_state_set' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 193]: can't resolve '_removexattr' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 194]: can't resolve '_sched_get_priority_max' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 195]: can't resolve '_sched_get_priority_min' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 196]: can't resolve '_setattrlist' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 197]: can't resolve '_setxattr' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 198]: can't resolve '_sscanf_l' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 199]: can't resolve '_statfs' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 200]: can't resolve '_strncasecmp_l' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 201]: can't resolve '_symlink' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 202]: can't resolve '_sysctl' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 203]: can't resolve '_sysctlbyname' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 204]: can't resolve '_thread_stack_pcs' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 205]: can't resolve '_utimes' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 206]: can't resolve '_vfork' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 207]: can't resolve '_vm_copy' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 208]: can't resolve '_wait4' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 209]: can't resolve '_sys_errlist' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 210]: can't resolve '_sys_nerr' in <flat_namespace>, needed by Foundation
 [dyldwarn]: [resolve, 211]: can't resolve '_CCCryptorCreate' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 212]: can't resolve '_CCCryptorRelease' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 213]: can't resolve '_CCCryptorUpdate' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 214]: can't resolve '_CC_MD5_Final' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 215]: can't resolve '_CC_MD5_Init' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 216]: can't resolve '_CC_MD5_Update' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 217]: can't resolve '____tolower_l' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 218]: can't resolve '____toupper_l' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 219]: can't resolve '___cmpdi2' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 220]: can't resolve '___maskrune_l' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 221]: can't resolve '_asl_close' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 222]: can't resolve '_asl_open' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 223]: can't resolve '_asl_vlog' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 224]: can't resolve '_cosf' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 225]: can't resolve '_floorf' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 226]: can't resolve '_fmin' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 227]: can't resolve '_fminf' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 228]: can't resolve '_lroundf' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 229]: can't resolve '_mach_absolute_time' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 230]: can't resolve '_round' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 231]: can't resolve '_strcasestr_l' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 232]: can't resolve '_strtoimax_l' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 233]: can't resolve '_sysctl' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 234]: can't resolve '_sysctlbyname' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 235]: can't resolve '_vm_copy' in <flat_namespace>, needed by CoreGraphics
 [dyldwarn]: [resolve, 236]: can't resolve '_backtrace' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 237]: can't resolve '_backtrace_symbols' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 238]: can't resolve '_backtrace_symbols_fd' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 239]: can't resolve '_dyld_image_path_containing_address' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 240]: can't resolve '_mach_error' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 241]: can't resolve '_task_threads' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 242]: can't resolve '_thread_get_state' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 243]: can't resolve '_thread_switch' in <flat_namespace>, needed by libobjc.A.dylib
 [dyldwarn]: [resolve, 244]: can't resolve '_timezone' in <flat_namespace>, needed by libicucore.A.dylib
 [dyldwarn]: [resolve, 245]: can't resolve '_posix_madvise' in <flat_namespace>, needed by libicucore.A.dylib
 [dyldwarn]: [resolve, 246]: can't resolve '_NSVersionOfLinkTimeLibrary' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 247]: can't resolve '_NXFindBestFatArch' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 248]: can't resolve '_NXGetLocalArchInfo' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 249]: can't resolve '___cmpdi2' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 250]: can't resolve '__dyld_get_image_name' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 251]: can't resolve '_asl_close' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 252]: can't resolve '_asl_free' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 253]: can't resolve '_asl_new' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 254]: can't resolve '_asl_open' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 255]: can't resolve '_asl_send' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 256]: can't resolve '_asl_set' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 257]: can't resolve '_class_createInstanceFromZone' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 258]: can't resolve '_fpathconf' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 259]: can't resolve '_fsync' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 260]: can't resolve '_mach_absolute_time' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 261]: can't resolve '_mach_error_string' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 262]: can't resolve '_mach_wait_until' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 263]: can't resolve '_notify_cancel' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 264]: can't resolve '_notify_post' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 265]: can't resolve '_notify_register_mach_port' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 266]: can't resolve '_pthread_atfork' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 267]: can't resolve '_pthread_is_threaded_np' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 268]: can't resolve '_strncasecmp_l' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 269]: can't resolve '_sysctl' in <flat_namespace>, needed by CoreFoundation
 [dyldwarn]: [resolve, 270]: can't resolve '_task_get_special_port' in <flat_namespace>, needed by CoreFoundation
 */

// newline! \n

