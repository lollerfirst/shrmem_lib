#shrmem: simple shared memory between processes

this implementation offers the following methods to manage shared memory in a simple way:

| Method | Description | 
| --------------- | --------------- |
| shrmem_init() | Initializes internal variables and arrays |
| get_shrmem(unique_id, size) | Retrieves 1 particular memory block characterized by its unique_id and its size (if another process also requests this, the memory will be shared!)|
| detach_shrmem(addr) | Removes the desired block from your address space |
| deinit_shrmem() | Removes all blocks that have been mapped to your address space and deinitilazes the library |
