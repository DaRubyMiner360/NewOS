I've finally managed to make good progress on an operating system.

This one has:

- Kernel mapped to higher half of memory

- Working paging

- A filesystem (Custom format for now)

- System calls

- Ability to load programs


And will be able to enter user mode soon too.

Bochs can't find a sound device when running on replit so you need to enter `cont` to make it ignore it ~~*, and then `c` `cont` or `continue` to start running the OS (Bochs pauses it automatically when it starts)*~~

# Filesystem

To load your own files you can use the fsgen utility in the fsgen directory and then replace fs.img with the generated image.

`./generate FILE SIZE FILE2 SIZE ...`

SIZE is in *bytes*.