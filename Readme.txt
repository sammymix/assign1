README.txt

# Storage Manager Assignment

## Overview

This project implements a simple storage manager capable of reading blocks from a file on disk into memory and writing blocks from memory to a file on disk. The storage manager handles pages (blocks) of fixed size (PAGE SIZE).
- The first 4KB of a file is reserved as its header, with the first 4 bytes storing the total page count.
## Files

- `storage_mgr.h`: Header file defining the storage manager interface (The file header is read-only and cannot be modified).
- `storage_mgr.c`: Implementation of the storage manager interface.
- `dberror.h`: Header file defining error codes.
- `dberror.c`: Implementation of error handling functions.
- `test_assign1_1.c`: Test cases for the storage manager.
- `test_helper.h`: Helper macros for testing.
- `Makefile`: Makefile to compile the project.

## Building and Running

1. To build the project, run `make` in the terminal.
2. To clean the build directory, run `make clean`.
3. To run the tests, execute the generated binary `./test_assign1`.

## Implementation Details

- The storage manager supports creating, opening, closing, and destroying page files.
- When reading or writing pages, update the current position and adjust the total page count as needed.
- It includes methods to read and write pages from/to a file.
- Error handling is implemented using predefined error codes.
