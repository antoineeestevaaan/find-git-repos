# Find all Git repositories in a directory.

## Building
`find-git-repos` is written in C, and its build system as well !

You only need a C compiler to both build and run the build system itself:
- bootstrap
```bash
gcc -o nob nob.c -lssl -lcrypto
```
- build
```
./nob
```

## Run
```bash
./build/main /path/to/directory
```
