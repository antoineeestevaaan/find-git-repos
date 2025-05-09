#define NOB_IMPLEMENTATION
#include "nob.h/nob.h"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  Nob_Cmd cmd = {0};

  nob_mkdir_if_not_exists("build/");

  nob_cmd_append(&cmd, "gcc", "-O3", "-o", "build/main", "main.c");
  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;

  return 0;
}
