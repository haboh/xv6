#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sleeplocks_container.h"


void check_lock(int lock_bucket) {  
	if (make_lock(lock_bucket) != CONTAINER_OK_REQUEST) {
		printf("Synchronization error!");
		exit(3);
	}	
}

void check_unlock(int lock_bucket) {
	if (make_unlock(lock_bucket) != CONTAINER_OK_REQUEST) {
		printf("Synchronization error!");
		exit(3);
	}
}

void safe_print(int lock_bucket, char const* str) {
	check_lock(lock_bucket);
	printf(str);
	check_unlock(lock_bucket);	
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Not enough command line argumnets!");
		exit(2);
	}	

	int p_child_to_parent[2];
	int p_parent_to_child[2];
	
	if (pipe(p_child_to_parent) != 0) {
		printf("Error in creating pipe!");
		exit(1);
	}
	if (pipe(p_parent_to_child) != 0) {
		printf("Error in creating pipe!");
		exit(1);
	}
	
	int lock_bucket = get_free();
	if (lock_bucket == CONTAINER_NO_EMPTY_LOCK_ERROR) {
		printf("Unable to create lock bucket!");
		exit(3);
	}

	int pid = fork();
	if (pid == 0) {
		if (close(p_parent_to_child[1]) != 0) {
			safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}
		if (close(p_child_to_parent[0]) != 0) {
			safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}
		
		char symb;
		int read_error;
		while ((read_error = read(p_parent_to_child[0], &symb, 1)) > 0) {		
			check_lock(lock_bucket);
			printf("%d: received <%c>\n", getpid(), symb);
			check_unlock(lock_bucket);
			if (write(p_child_to_parent[1], &symb, 1) != 1) {
				safe_print(lock_bucket, "Error in writing!");
				exit(1);
			}
		}

		if (read_error != 0) {
			safe_print(lock_bucket, "Error in reading!");
			exit(1);
		}
		
		if (close(p_parent_to_child[0]) != 0) {
			safe_print(lock_bucket, "Erorr in closing pipe!");
			exit(1);
		}
		if (close(p_child_to_parent[1]) != 0) {
			safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}
	} else if (pid > 0) {
		if (close(p_parent_to_child[0]) != 0) {
		       	safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}
		if (close(p_child_to_parent[1]) != 0) {
			safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}
		
		for (char *symb=argv[1]; *symb; symb++) {
			if (write(p_parent_to_child[1], symb, 1) != 1) {
				safe_print(lock_bucket, "Error while writing!");
				exit(1);
			}
		}
		if (close(p_parent_to_child[1]) != 0) {
			safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}

		char symb;
		int read_error;
		while ((read_error = read(p_child_to_parent[0], &symb, 1)) > 0) {
			check_lock(lock_bucket);
			printf("%d: received <%c>\n", getpid(), symb);
			check_unlock(lock_bucket);
		}

		if (read_error != 0) {
			safe_print(lock_bucket, "Error while reading");
			exit(1);
		}
		
		if (close(p_child_to_parent[0]) != 0) {
			safe_print(lock_bucket, "Error in closing pipe!");
			exit(1);
		}
		wait(0);
	} else {
	        safe_print(lock_bucket, "Error in creating new process!");	
		exit(0);
	}

	make_free(lock_bucket);
	exit(0);
}
