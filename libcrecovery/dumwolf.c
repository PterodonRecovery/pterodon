
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mincrypt/sha.h"
#include "mincrypt/sha256.h"
#include "dumwolf.h"
#ifndef _DUMWOLF_LOGGING
#include <time.h>
#define LOGDUMWOLF(...) printf("DW:" __VA_ARGS__)
#else
#define LOGDUMWOLF(...) do {} while (0)
#endif

int parse_os_version(char *ver)
{
    char *token;
    int verArray[3] = {0};
    int a,b,c = 0;
    int i = 0;

    token = strtok(ver, ".");
    while(token != NULL) {
        sscanf(token, "%d", &verArray[i]);
        token = strtok(NULL, ".");
        i++;
    }
    a = verArray[0];
    b = verArray[1];
    c = verArray[2];
    if((a < 128) && (b < 128) && (c < 128))
        return (a << 14) | (b << 7) | c;
    return 0;
}

int parse_os_patch_level(char *lvl)
{
    char *token;
    int lvlArray[2] = {0};
    int y,m = 0;
    int i = 0;

    token = strtok(lvl, "-");
    while(token != NULL) {
        sscanf(token, "%d", &lvlArray[i]);
        token = strtok(NULL, "-");
        i++;
    }
    y = lvlArray[0] - 2000;
    m = lvlArray[1];
    if((y >= 0) && (y < 128) && (m > 0) && (m <= 12))
        return (y << 4) | m;
    return 0;
}
     
static void *extract_to_memory(const char *fn, unsigned *_sz)
{
    char *data;
    int sz;
    int fd;
    int errno_tmp;
    
    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;
    
    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;
    data = (char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops;
    close(fd);

    if(_sz) *_sz = sz;
    return data;

oops:
    errno_tmp = errno;
    close(fd);
    if(data != 0) free(data);
    errno = errno_tmp;
    return 0;
}

enum hash_alg get_hash_algorithm(const char *name)
{
    const struct hash_name *ptr = hash_names;

    while (ptr->name) {
        if (!strcmp(ptr->name, name))
            return ptr->alg;
        ptr++;
    }

    return HASH_UNKNOWN;
}

void generate_id_sha1(dumwolf *hdr, void *kernel_data, void *ramdisk_data,
                      void *second_data, void *dt_data)
{
    SHA_CTX ctx;
    const byte *sha;

    SHA_init(&ctx);
    SHA_update(&ctx, kernel_data, hdr->kernel_size);
    SHA_update(&ctx, &hdr->kernel_size, sizeof(hdr->kernel_size));
    SHA_update(&ctx, ramdisk_data, hdr->ramdisk_size);
    SHA_update(&ctx, &hdr->ramdisk_size, sizeof(hdr->ramdisk_size));
    SHA_update(&ctx, second_data, hdr->second_size);
    SHA_update(&ctx, &hdr->second_size, sizeof(hdr->second_size));
    if(dt_data) {
        SHA_update(&ctx, dt_data, hdr->dt_size);
        SHA_update(&ctx, &hdr->dt_size, sizeof(hdr->dt_size));
    }
    sha = SHA_final(&ctx);
    memcpy(hdr->id, sha,
           SHA_DIGEST_SIZE > sizeof(hdr->id) ? sizeof(hdr->id) : SHA_DIGEST_SIZE);
}

void generate_id_sha256(dumwolf *hdr, void *kernel_data, void *ramdisk_data,
                        void *second_data, void *dt_data)
{
    SHA256_CTX ctx;
    const byte *sha;

    SHA256_init(&ctx);
    SHA256_update(&ctx, kernel_data, hdr->kernel_size);
    SHA256_update(&ctx, &hdr->kernel_size, sizeof(hdr->kernel_size));
    SHA256_update(&ctx, ramdisk_data, hdr->ramdisk_size);
    SHA256_update(&ctx, &hdr->ramdisk_size, sizeof(hdr->ramdisk_size));
    SHA256_update(&ctx, second_data, hdr->second_size);
    SHA256_update(&ctx, &hdr->second_size, sizeof(hdr->second_size));
    if(dt_data) {
        SHA256_update(&ctx, dt_data, hdr->dt_size);
        SHA256_update(&ctx, &hdr->dt_size, sizeof(hdr->dt_size));
    }
    sha = SHA256_final(&ctx);
    memcpy(hdr->id, sha,
           SHA256_DIGEST_SIZE > sizeof(hdr->id) ? sizeof(hdr->id) : SHA256_DIGEST_SIZE);
}


void generate_id(enum hash_alg alg, dumwolf *hdr, void *kernel_data,
                 void *ramdisk_data, void *second_data, void *dt_data)
{
    switch (alg) {
        case HASH_SHA1:
            generate_id_sha1(hdr, kernel_data, ramdisk_data,
                             second_data, dt_data);
            break;
        case HASH_SHA256:
            generate_id_sha256(hdr, kernel_data, ramdisk_data,
                               second_data, dt_data);
            break;
        case HASH_UNKNOWN:
        default:
            printf("Unknown hash type.\n");
    }
}



static unsigned char padding[131072] = { 0, };

int write_padding(int fd, unsigned pagesize, unsigned itemsize)
{
    unsigned pagemask = pagesize - 1;
    ssize_t count;

    if((itemsize & pagemask) == 0) {
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    if(write(fd, padding, count) != count) {
        return -1;
    } else {
        return 0;
    }
}

static char* load_file(const char* file) {
    FILE *f = fopen(file, "rt");
    if (f == NULL) {
    printf("Could not load file: '%s' (%s)\n", file, strerror(errno));
    return NULL;
    }
	char* line = malloc((BOOT_EXTRA_ARGS_SIZE + BOOT_EXTRA_ARGS_SIZE) * sizeof(char*));
	if (line == NULL) {
	printf("malloc failed on load_file() - (%s)\n", strerror(errno));
    goto oops;
    }   
	LOGDUMWOLF("Reading: '%s'\n", file);
	if (fgets(line, BOOT_EXTRA_ARGS_SIZE + BOOT_EXTRA_ARGS_SIZE, f) == NULL) {
	printf("fgets failed on: %s\n", file);
    //goto oops;
    }
	LOGDUMWOLF("Line: '%s'\n", line);
	fclose(f);
	return line;
	oops:
    fclose(f);
    if(line != 0) free(line);
    return NULL;
}

void write_file(const char* file, const char* string)
{
	FILE* f = fopen(file, "w");
    fwrite(string, strlen(string), 1, f);
    fclose(f);
}

const char *detect_hash_type(const struct dumwolf *hdr)
{
    const dword *id = hdr->id;
    if (id[0] != 0 && id[1] != 0 && id[2] != 0 && id[3] != 0 &&
        id[4] != 0 && id[5] != 0 && id[6] != 0 && id[7] != 0)
        return "sha256";
    else if (id[0] != 0 && id[1] != 0 && id[2] != 0 && id[3] != 0 &&
        id[4] != 0 && id[5] == 0 && id[6] == 0 && id[7] == 0)
        return "sha1";
    else
        return "unknown";
}

int read_padding(FILE* f, unsigned itemsize, int pagesize)
{
    byte* buf = (byte*)malloc(sizeof(byte) * pagesize);
    unsigned pagemask = pagesize - 1;
    unsigned count;

    if((itemsize & pagemask) == 0) {
        free(buf);
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    if(fread(buf, count, 1, f)){};
    free(buf);
    return count;
}

int dumwolf_extract_header(const char* path, const char* directory) {
#ifdef _DUMWOLF_LOGGING
	 time_t start, stop;
	 time(&start);
#endif
	    char tmp[PATH_MAX];
        int base = 0;      
        int total_read = 0;
        int i, seeklimit = 65536;
        FILE* f = fopen(path, "rb");
        if (!f) {
        printf("Could not open input file: '%s', (%s)\n", path, strerror(errno));
        return 1;
    }
        dumwolf header;
       for (i = 0; i <= seeklimit; i++) {
        fseek(f, i, SEEK_SET);
        if(fread(tmp, BOOT_MAGIC_SIZE, 1, f)){};
        if (memcmp(tmp, BOOT_MAGIC, BOOT_MAGIC_SIZE) == 0)
            break;
         }
    if (i > seeklimit) {
        printf("Unable to find android boot magic!\n");
        return 1;
    }
    total_read = i;
    fseek(f, i, SEEK_SET);
    if (i > 0) {
        LOGDUMWOLF("Android magic found at: %d\n", i);
    }

    if(fread(&header, sizeof(header), 1, f)){};
    base = header.kernel_addr - 0x00008000;
    LOGDUMWOLF("BOARD_KERNEL_CMDLINE %.*s%.*s\n", BOOT_ARGS_SIZE, header.cmdline, BOOT_EXTRA_ARGS_SIZE, header.extra_cmdline);
    LOGDUMWOLF("BOARD_KERNEL_BASE %08x\n", base);
    LOGDUMWOLF("BOARD_NAME %s\n", header.name);
    LOGDUMWOLF("BOARD_PAGE_SIZE %d\n", header.page_size);
    LOGDUMWOLF("BOARD_HASH_TYPE %s\n", detect_hash_type(&header));
    LOGDUMWOLF("BOARD_KERNEL_OFFSET %08x\n", header.kernel_addr - base);
    LOGDUMWOLF("BOARD_RAMDISK_OFFSET %08x\n", header.ramdisk_addr - base);
    LOGDUMWOLF("BOARD_SECOND_OFFSET %08x\n", header.second_addr - base);
    LOGDUMWOLF("BOARD_TAGS_OFFSET %08x\n", header.tags_addr - base);
    int a=0, b=0, c=0, y=0, m=0;
    if (header.os_version != 0) {
        int os_version,os_patch_level;
        os_version = header.os_version >> 11;
        os_patch_level = header.os_version&0x7ff;

        a = (os_version >> 14)&0x7f;
        b = (os_version >> 7)&0x7f;
        c = os_version&0x7f;
        
        y = (os_patch_level >> 4) + 2000;
        m = os_patch_level&0xf;
        
        if((a < 128) && (b < 128) && (c < 128) && (y >= 2000) && (y < 2128) && (m > 0) && (m <= 12)) {
        	LOGDUMWOLF("BOARD_OS_VERSION %d.%d.%d\n", a, b, c);
            LOGDUMWOLF("BOARD_OS_PATCH_LEVEL %d-%02d\n", y, m);
            } else {
            header.os_version = 0;
        }
    }
    
    if (header.dt_size != 0) {
        LOGDUMWOLF("BOARD_DT_SIZE %d\n", header.dt_size);
    }
           
    LOGDUMWOLF("cmdline...\n");
    sprintf(tmp, "%s/cmdline", directory);
    char cmdlinetmp[BOOT_ARGS_SIZE+BOOT_EXTRA_ARGS_SIZE+1];
    sprintf(cmdlinetmp, "%.*s%.*s", BOOT_ARGS_SIZE, header.cmdline, BOOT_EXTRA_ARGS_SIZE, header.extra_cmdline);
    cmdlinetmp[BOOT_ARGS_SIZE+BOOT_EXTRA_ARGS_SIZE]='\0';
    write_file(tmp, cmdlinetmp);
  
     
    LOGDUMWOLF("board...\n");
    sprintf(tmp, "%s/board", directory);
    write_file(tmp, (char *)header.name);
    char valtmp[200];
    
    
    LOGDUMWOLF("base...\n");
    sprintf(tmp, "%s/base", directory);
    sprintf(valtmp, "%08x", base);
    write_file(tmp, valtmp);
    
    
    LOGDUMWOLF("pagesize...\n");
    sprintf(tmp, "%s/pagesize", directory);
    sprintf(valtmp, "%d", header.page_size);
    write_file(tmp, valtmp);

    
    LOGDUMWOLF("kerneloff...\n");
    sprintf(tmp, "%s/kerneloff", directory);
    sprintf(valtmp, "%08x", header.kernel_addr - base);
    write_file(tmp, valtmp);

    
    LOGDUMWOLF("ramdiskoff...\n");
    sprintf(tmp, "%s/ramdiskoff", directory);
    sprintf(valtmp, "%08x", header.ramdisk_addr - base);
    write_file(tmp, valtmp);
    
    
    LOGDUMWOLF("secondoff...\n");
    sprintf(tmp, "%s/secondoff", directory);
    sprintf(valtmp, "%08x", header.second_addr - base);
    write_file(tmp, valtmp);
 
    
    LOGDUMWOLF("tagsoff...\n");  
    sprintf(tmp, "%s/tagsoff", directory);
    sprintf(valtmp, "%08x", header.tags_addr - base);
    write_file(tmp, valtmp);
    
    if (header.os_version != 0) {    
        LOGDUMWOLF("osversion...\n");
        sprintf(tmp, "%s/osversion", directory);
        sprintf(valtmp, "%d.%d.%d", a, b, c);
        write_file(tmp, valtmp);
    
        LOGDUMWOLF("oslevel...\n");
        sprintf(tmp, "%s/oslevel", directory);
        sprintf(valtmp, "%d-%02d", y, m);
        write_file(tmp, valtmp);
    }
        
    
    LOGDUMWOLF("hash...\n");
    sprintf(tmp, "%s/hash", directory);
    write_file(tmp, detect_hash_type(&header));
	
	total_read += sizeof(header);
    LOGDUMWOLF("total read: %d\n", total_read);
    total_read += read_padding(f, sizeof(header), header.page_size);
       
    sprintf(tmp, "%s/zImage", directory);
    FILE *k = fopen(tmp, "wb");
    cbyte* kernel = (cbyte*)malloc(header.kernel_size);
    if (kernel == NULL) {
    printf("malloc failed on '%s' - (%s)\n", tmp, strerror(errno));
    return 1;
    }
    
    LOGDUMWOLF("reading kernel...\n");
    if(fread(kernel, header.kernel_size, 1, f)){};
    total_read += header.kernel_size;
    fwrite(kernel, header.kernel_size, 1, k);
    free(kernel);
    fclose(k);
    
	LOGDUMWOLF("kernel size: %d bytes\n", header.kernel_size);
	total_read += read_padding(f, header.kernel_size, header.page_size);

    sprintf(tmp, "%s/ramdisk.gz", directory);
    FILE *r = fopen(tmp, "wb");
    cbyte* ramdisk = (cbyte*)malloc(header.ramdisk_size);
    if (ramdisk == NULL) {
    printf("malloc failed on '%s' - (%s)\n", tmp, strerror(errno));
    return 1;
    }
    
    LOGDUMWOLF("reading ramdisk...\n");
    if(fread(ramdisk, header.ramdisk_size, 1, f)){};
    total_read += header.ramdisk_size;
    fwrite(ramdisk, header.ramdisk_size, 1, r);
    free(ramdisk);
    fclose(r);
    
	LOGDUMWOLF("ramdisk size: %d bytes\n", header.ramdisk_size);
	read_padding(f, header.ramdisk_size, header.page_size);
	
	if (header.second_size != 0) {
        sprintf(tmp, "%s/second", directory);
        FILE *s = fopen(tmp, "wb");
        cbyte* second = (cbyte*)malloc(header.second_size);
        if (second == NULL) {
        printf("malloc failed on '%s' - (%s)\n", tmp, strerror(errno));
        return 1;
        }    
        LOGDUMWOLF("reading second...\n");
        if(fread(second, header.second_size, 1, f)){};
        total_read += header.second_size;
        fwrite(second, header.second_size, 1, s);
        free(second);
        fclose(s);
    }
    
	LOGDUMWOLF("second size: %d bytes\n", header.second_size);
	read_padding(f, header.second_size, header.page_size);
    
     if (header.dt_size != 0) {
        sprintf(tmp, "%s/dtb", directory);
        FILE *d = fopen(tmp, "wb");
        cbyte* dtb = (cbyte*)malloc(header.dt_size);
        if (dtb == NULL) {
        printf("malloc failed on '%s' - (%s)\n", tmp, strerror(errno));
        return 1;
        }
        
        LOGDUMWOLF("Reading dtb...\n");
        if(fread(dtb, header.dt_size, 1, f)){};
        total_read += header.dt_size;
        fwrite(dtb, header.dt_size, 1, d);
        free(dtb);
        fclose(d);
    }
    fclose(f);
	LOGDUMWOLF("dtb size: %d bytes\n", header.dt_size);
    LOGDUMWOLF("Total size: %d bytes\n", total_read);
#ifdef _DUMWOLF_LOGGING
    time(&stop);
    LOGDUMWOLF("Header extraction took %i second(s)\n", (int)difftime(stop, start));
#endif
    return 0;
}

int dumwolf_create_header(const char* bootimg, const char* directory) {
#ifdef _DUMWOLF_LOGGING
	 time_t start, stop;
	 time(&start);
#endif
    dumwolf hdr;
    char path[256];
    void *kernel_data = NULL;
    void *ramdisk_data = NULL;
    void *second_data = NULL;
    char *cmdline = "";
    char *board = "";
    char *val = NULL;
    int os_version = 0;
    int os_patch_level = 0;
    void *dt_data = NULL;
    dword pagesize = 2048;
    int fd;
    dword base           = 0x10000000U;
    dword kernel_offset  = 0x00008000U;
    dword ramdisk_offset = 0x01000000U;
    dword second_offset  = 0x00f00000U;
    dword tags_offset    = 0x00000100U;
    dword kernel_sz      = 0;
    dword ramdisk_sz     = 0;
    dword second_sz      = 0;
    dword dt_sz          = 0;
    size_t cmdlen;
    enum hash_alg hash_alg = HASH_SHA1;
    memset(&hdr, 0, sizeof(hdr));
    struct stat st;
    sprintf(path, "%s/cmdline", directory);
    if (!stat(path, &st)) {
    val = load_file(path);
    if (val == NULL) return 1;
    cmdline = val;
    }
    sprintf(path, "%s/base", directory);
    if (!stat(path, &st))  {
    val = load_file(path);
    if (val == NULL) return 1;
    base = strtoul(val, 0, 16);
    }
    sprintf(path, "%s/pagesize", directory);
    if (!stat(path, &st)) {
       val = load_file(path);
       if (val == NULL) return 1;
       pagesize = strtoul(val, 0, 10);
          if ((pagesize != 2048) && (pagesize != 4096)
           && (pagesize != 8192) && (pagesize != 16384)
           && (pagesize != 32768) && (pagesize != 65536)
           && (pagesize != 131072)) {
           printf("Unsupported page size %d\n", pagesize);
           return 1;
           }
          }
       sprintf(path, "%s/kerneloff", directory);
       if (!stat(path, &st)) {
       val = load_file(path);
       if (val == NULL) return 1;
       kernel_offset = strtoul(val, 0, 16);
          }
       sprintf(path, "%s/ramdiskoff", directory);
       if (!stat(path, &st)) {
       val = load_file(path);
       if (val == NULL) return 1;
       ramdisk_offset = strtoul(val, 0, 16);
          }
       sprintf(path, "%s/secondoff", directory);
       if (!stat(path, &st)) {
       val = load_file(path);
       if (val == NULL) return 1;
       second_offset = strtoul(val, 0, 16);
          }
       sprintf(path, "%s/tagsoff", directory);
       if (!stat(path, &st)) {
       val = load_file(path);
       if (val == NULL) return 1;
       tags_offset = strtoul(val, 0, 16);
          }
       sprintf(path, "%s/board", directory);
       if (!stat(path, &st)) {
       val = load_file(path);
       if (val == NULL) return 1;
       board = val;
          }
       sprintf(path, "%s/osversion", directory);
       if (!stat(path, &st)) {
      val = load_file(path);
      if (val == NULL) return 1;
      os_version = parse_os_version(val);
        }
       sprintf(path, "%s/oslevel", directory);
      if (!stat(path, &st)) {
      val = load_file(path);
      if (val == NULL) return 1;
      os_patch_level = parse_os_patch_level(val);
        }
       sprintf(path, "%s/hash", directory);
      if (!stat(path, &st)) {
     val = load_file(path);
     if (val == NULL) return 1;
     hash_alg = get_hash_algorithm(strcmp(val,"unknown") == 0  ? "sha1" : val);      
     if (hash_alg == HASH_UNKNOWN) {
     printf("Unknown hash algorithm '%s'\n", val);
     return 1;
      }     
       }
      sprintf(path, "%s/zImage", directory);
      if (stat(path, &st) != 0) {
      printf("Unable to find '%s'\n", path);
      return 1;
      }
      
    hdr.page_size = pagesize;
    hdr.kernel_addr =  base + kernel_offset;
    hdr.ramdisk_addr = base + ramdisk_offset;
    hdr.second_addr =  base + second_offset;
    hdr.tags_addr =    base + tags_offset;
    hdr.os_version = (os_version << 11) | os_patch_level;
    
    if(strlen(board) >= BOOT_NAME_SIZE) {
        printf("board name too large\n");
        return 1;
    }
    
    strcpy((char *) hdr.name, board);
    memcpy(hdr.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);
    cmdlen = strlen(cmdline);
    if(cmdlen <= BOOT_ARGS_SIZE) {
        strcpy((char *)hdr.cmdline, cmdline);
    } else if(cmdlen <= BOOT_ARGS_SIZE + BOOT_EXTRA_ARGS_SIZE) {
        /* exceeds the limits of the base command-line size, go for the extra */
        memcpy(hdr.cmdline, cmdline, BOOT_ARGS_SIZE);
        strcpy((char *)hdr.extra_cmdline, cmdline+BOOT_ARGS_SIZE);
    } else {
        printf("kernel commandline too large!\n");
        return 1;
    }
    
    kernel_data = extract_to_memory(path, &kernel_sz);
    if(kernel_data == 0) {
        printf("could not load kernel '%s' (%s)\n", path, strerror(errno));
        return 1;
      }
        hdr.kernel_size = kernel_sz;
        sprintf(path, "%s/ramdisk-new", directory);
        if (stat(path, &st) != 0)
        sprintf(path, "%s/ramdisk.gz", directory);
        ramdisk_data = extract_to_memory(path, &ramdisk_sz);
        if(ramdisk_data == 0) {
            printf("could not load ramdisk '%s' (%s)\n", path, strerror(errno));
            return 1;
        }
        hdr.ramdisk_size = ramdisk_sz;

    sprintf(path, "%s/second", directory);
    if(!stat(path, &st)) {
        second_data = extract_to_memory(path, &second_sz);
        if(second_data == 0) {
            printf("could not load secondstage '%s' (%s)\n", path, strerror(errno));
            return 1;
        }
    }
    hdr.second_size = second_sz;

    sprintf(path, "%s/dtb", directory);
    if(!stat(path, &st)) {
        dt_data = extract_to_memory(path, &dt_sz);
        if (dt_data == 0) {
            printf("could not load device tree image '%s' (%s)\n", path, strerror(errno));
            return 1;
        }
    }
    hdr.dt_size = dt_sz;

    generate_id(hash_alg, &hdr, kernel_data, ramdisk_data, second_data,
                dt_data);
                
       LOGDUMWOLF("Reading header after id generation...\n");
       LOGDUMWOLF("cmdline: %s \n", cmdline); 
       LOGDUMWOLF("board: %s \n", board);
       LOGDUMWOLF("base: %d \n", base);
       LOGDUMWOLF("pagesize: %d \n", pagesize);
       LOGDUMWOLF("ramdisk_offset: %d \n", ramdisk_offset);
       LOGDUMWOLF("second_offset: %d \n", second_offset);
       LOGDUMWOLF("tags_offset: %d \n", tags_offset);
       LOGDUMWOLF("os_version: %i \n", os_version);
       LOGDUMWOLF("os_patch_level: %i \n", os_patch_level);
       LOGDUMWOLF("hash_algorithm: %s\n", hash_alg == HASH_SHA1 ? "sha1" : "sha256");

   if (!stat(bootimg, &st))
    unlink(bootimg);
    
    fd = open(bootimg, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd < 0) {
        printf("could not create '%s'\n", bootimg);
        return 1;
    }
    
    do {

    if(write(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) break;
    if(write_padding(fd, pagesize, sizeof(hdr))) break;

    if(write(fd, kernel_data, hdr.kernel_size) != (ssize_t) hdr.kernel_size) break;
    if(write_padding(fd, pagesize, hdr.kernel_size)) break;

    if(write(fd, ramdisk_data, hdr.ramdisk_size) != (ssize_t) hdr.ramdisk_size) break;
    if(write_padding(fd, pagesize, hdr.ramdisk_size)) break;

    if(second_data) {
        if(write(fd, second_data, hdr.second_size) != (ssize_t) hdr.second_size) break;
        if(write_padding(fd, pagesize, hdr.second_size)) break;
    }

    if(dt_data) {
        if(write(fd, dt_data, hdr.dt_size) != (ssize_t) hdr.dt_size) break;
        if(write_padding(fd, pagesize, hdr.dt_size)) break;
    }
free(kernel_data);
free(ramdisk_data);
free(second_data);
free(dt_data);
free(val);
close(fd);
#ifdef _DUMWOLF_LOGGING
time(&stop);
printf("Successfully created bootable image, process took %i second(s)\n", (int)difftime(stop, start));
#endif
return 0;
} while (0);
free(kernel_data);
free(ramdisk_data);
free(second_data);
free(dt_data);
free(val);
    close(fd);
    printf("failed writing '%s': %s\n", bootimg,
    strerror(errno));
#ifdef _DUMWOLF_LOGGING
time(&stop);
printf("Unable to create bootable image, process took %i second(s)\n", (int)difftime(stop, start));
#endif
    return 1;
}