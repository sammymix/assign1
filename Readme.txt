The first 4KB of a file is reserved as its header, with the first 4 bytes storing the total page count.

The file header is read-only and cannot be modified.

When reading or writing pages, update the current position and adjust the total page count as needed.
