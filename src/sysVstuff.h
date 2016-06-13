/* Semaphoren, Shared- Memory Objekte und Message Queues */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#if defined WINDOWS || defined ATARI || defined __APPLE__
  #define key_t int
#endif

void shm_free(int shmid);
int shm_detatch(const void *);
int shm_attach(int shmid);
int shm_malloc(size_t segsize, key_t key);
