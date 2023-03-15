#include "spinlock.h"
#include "sleeplock.h"

#define SLEEPLOCKS_CONTAINER_SIZE 10

#define CONTAINER_OK_REQUEST            0
#define CONTAINER_NO_EMPTY_LOCK_ERROR  -1
#define CONTAINER_NOT_BUSY_ERROR       -2
#define CONTAINER_ALREADY_LOCKED_ERROR -3
#define CONTAINER_NOT_LOCKED_ERROR     -4

struct sleeplocks_container_t {
	struct spinlock locker;

	char   busy_locks     [SLEEPLOCKS_CONTAINER_SIZE];
	struct sleeplock locks[SLEEPLOCKS_CONTAINER_SIZE];
};
