#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHM_KEY 1234
#define SEM_KEY 5678
#define MAX_BUFFER_SIZE 1024

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

void P(int semid) {
    struct sembuf p_op;
    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = SEM_UNDO;
    semop(semid, &p_op, 1);
}

void V(int semid) {
    struct sembuf v_op;
    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = SEM_UNDO;
    semop(semid, &v_op, 1);
}

int main() {
    int shmid, semid;
    key_t key_shm = SHM_KEY, key_sem = SEM_KEY;
    char *shared_memory;
    FILE *source_file, *destination_file;
    struct stat stat_buf;

    // Create or get shared memory
    if ((shmid = shmget(key_shm, MAX_BUFFER_SIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    if ((shared_memory = shmat(shmid, NULL, 0)) == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    // Create or get semaphore
    if ((semid = semget(key_sem, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget");
        exit(1);
    }

    // Initialize semaphore value
    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(semid, 0, SETVAL, sem_arg) == -1) {
        perror("semctl");
        exit(1);
    }

    // Open source file
    if ((source_file = fopen("test.txt", "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    // Open destination file
    if ((destination_file = fopen("cptest.txt", "w")) == NULL) {
        perror("fopen");
        fclose(source_file);
        exit(1);
    }

    while (!feof(source_file)) {
        P(semid);  // Wait for semaphore

        // Read from source file to shared memory
        size_t read_size = fread(shared_memory, 1, MAX_BUFFER_SIZE, source_file);

        // Write from shared memory to destination file
        fwrite(shared_memory, 1, read_size, destination_file);

        V(semid);  // Release semaphore
    }

    // Clean up
    shmdt(shared_memory);
    fclose(source_file);
    fclose(destination_file);

    return 0;
}

