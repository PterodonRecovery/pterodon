/* tools/mkbootimg/bootimg.h
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/

#include <stdint.h>

typedef uint8_t byte;
typedef uint32_t dword;
typedef struct dumwolf dumwolf;
typedef unsigned char cbyte;


#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512
#define BOOT_EXTRA_ARGS_SIZE 1024
#define DUMWOLF_DRIVER_DEBUG 1

struct dumwolf
{
    byte magic[BOOT_MAGIC_SIZE];
    dword kernel_size; 
    dword kernel_addr; 

    dword ramdisk_size;
    dword ramdisk_addr;

    dword second_size; 
    dword second_addr;

    dword tags_addr;
    dword page_size;
    dword dt_size;
    dword os_version;
    byte name[BOOT_NAME_SIZE];
    byte cmdline[BOOT_ARGS_SIZE];
    dword id[8];
    byte extra_cmdline[BOOT_EXTRA_ARGS_SIZE];
} __attribute__((packed));

enum hash_alg {
    HASH_UNKNOWN = -1,
    HASH_SHA1 = 0,
    HASH_SHA256,
};

struct hash_name {
    const char *name;
    enum hash_alg alg;
};

const struct hash_name hash_names[] = {
    { "sha1", HASH_SHA1 },
    { "sha256", HASH_SHA256 },
    { NULL, /* LX - LMTK */},
};
