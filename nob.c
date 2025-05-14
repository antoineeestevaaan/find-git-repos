#define NOB_IMPLEMENTATION
#include "nob.h"

#include <openssl/sha.h>
#include <stdio.h>

#define BUILD_DIR "build/"
#define MAIN (BUILD_DIR "main")
#define EXE_TEMPLATE                                                           \
  (BUILD_DIR "find-git-repos-%s") // "%s" will be replaced with the hash of the
                                  // executable
#define EXE_LENGTH                                                             \
  5 +                          /* build directory                      */      \
      1 +                      /* path separator                       */      \
      15 +                     /* executable name                      */      \
      SHA256_DIGEST_LENGTH * 2 /* hash length, two characters per byte */

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF_PLUS(argc, argv, NULL, "-lssl", "-lcrypto");

  Nob_Cmd cmd = {0};
  Nob_String_Builder sb = {0};

  nob_mkdir_if_not_exists(BUILD_DIR);

  // compile
  nob_cmd_append(&cmd, "gcc", "-O3", "-o", MAIN, "main.c");
  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;

  // hash binary
  nob_read_entire_file(MAIN, &sb);
  unsigned char *md_buf = malloc(SHA256_DIGEST_LENGTH);
  unsigned char *digest =
      SHA256((const unsigned char *)sb.items, sb.count, md_buf);
  sb.count = 0;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    nob_sb_appendf(&sb, "%02x", digest[i]);
  }

  // copy
  char *exe = malloc(EXE_LENGTH);
  sprintf(exe, EXE_TEMPLATE, sb.items);
  nob_copy_file(MAIN, exe);

  return 0;
}
