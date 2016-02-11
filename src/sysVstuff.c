/* sysVstuff.c         (c) Markus Hoffmann   */

/* Semaphoren, Shared- Memory Objekte und Message Queues */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define FALSE 0
#define TRUE (!FALSE)

#define MAX_SEND_SIZE 80
#define SEM_RESOURCE_MAX         1        /* Initial value of all semaphores */

struct mymsgbuf {
        long mtype;
        char mtext[MAX_SEND_SIZE];
};

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text);
void read_message(int qid, struct mymsgbuf *qbuf, long type);
void remove_queue(int qid);
void change_queue_mode(int qid, char *mode);
void opensem(int *sid, key_t key);
void createsem(int *sid, key_t key, int members);
void locksem(int sid, int member);
void unlocksem(int sid, int member);
void removesem(int sid);
unsigned short get_member_count(int sid);
int getval(int sid, int member);
void dispval(int sid, int member);
void changemode(int sid, char *mode);
void shm_free(int);
void change_shm_mode(int, char *);
int shm_malloc(size_t,void *,key_t);
int open_msg(key_t);


int open_msg(key_t key) {

  int msgqueue_id;
      /* Open the queue - create if necessary */
        if((msgqueue_id = msgget(key, IPC_CREAT|0660)) == -1) {
                 error(-41,"open");    /* Message error.*/ 
                 return(-1);
        }
  return(msgqueue_id);
}

int peek_message( int qid, long type )
{
         int     result, length;
         if((result = msgrcv( qid, NULL, 0, type,  IPC_NOWAIT)) == -1)
         { if(errno == E2BIG) return(TRUE); }
        return(FALSE);
}

void change_queue_mode(int qid, char *mode) {
        struct msqid_ds myqueue_ds;
	
        msgctl(qid, IPC_STAT, &myqueue_ds);            /* Get current info */
        sscanf(mode, "%ho", &myqueue_ds.msg_perm.mode);/* Convert and load the mode */
        msgctl(qid, IPC_SET, &myqueue_ds);             /* Update the mode */
}

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text) {
        /* Send a message to the queue */
        printf("Sending a message ...\n");
        qbuf->mtype = type;
        strcpy(qbuf->mtext, text);
        if((msgsnd(qid, (struct msgbuf *)qbuf,
                strlen(qbuf->mtext)+1, 0)) ==-1) {
               error(-41,"send");    /* Message konnte nicht versandt werden.*/ 
        }
}


void read_message(int qid, struct mymsgbuf *qbuf, long type)
{
        /* Read a message from the queue */
        qbuf->mtype = type;
        msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
        printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
}


void remove_queue(int qid) /* Remove the msg-queue */ {
        msgctl(qid, IPC_RMID, 0);
}


#if 0


void opensem(int *sid, key_t key)
{
        /* Open the semaphore set - do not create! */


        if((*sid = semget(key, 0, 0666)) == -1)
        {
                printf("Semaphore set does not exist!\n");
                exit(1);
        }


}


void createsem(int *sid, key_t key, int members)
{
        int cntr;
        union semun semopts;


        if(members > SEMMSL) {
                printf("Sorry, max number of semaphores in a set is %d\n",
                         SEMMSL);
                exit(1);
        }


        printf("Attempting to create new semaphore set with %d members\n",
                                 members);


        if((*sid = semget(key, members, IPC_CREAT|IPC_EXCL|0666))
                         == -1)
        {
                fprintf(stderr, "Semaphore set already exists!\n");
                exit(1);
        }


        semopts.val = SEM_RESOURCE_MAX;


        /* Initialize all members (could be done with SETALL) */
        for(cntr=0; cntr<members; cntr++)
                semctl(*sid, cntr, SETVAL, semopts);
}

void locksem(int sid, int member)
{
        struct sembuf sem_lock={ 0, -1, IPC_NOWAIT};


        if( member<0 || member>(get_member_count(sid)-1))
        {
                 fprintf(stderr, "semaphore member %d out of range\n", member);
                 return;
        }


        /* Attempt to lock the semaphore set */
        if(!getval(sid, member))
        {
                 fprintf(stderr, "Semaphore resources exhausted (no lock)!\n");
                 exit(1);
        }


        sem_lock.sem_num = member;


        if((semop(sid, &sem_lock, 1)) == -1)
        {
                 fprintf(stderr, "Lock failed\n");
                 exit(1);
        }
        else
                 printf("Semaphore resources decremented by one (locked)\n");


        dispval(sid, member);
}
void unlocksem(int sid, int member)
{
        struct sembuf sem_unlock={ member, 1, IPC_NOWAIT};
        int semval;


        if( member<0 || member>(get_member_count(sid)-1))
        {
                 fprintf(stderr, "semaphore member %d out of range\n", member);
                 return;
        }


        /* Is the semaphore set locked? */
        semval = getval(sid, member);
        if(semval == SEM_RESOURCE_MAX) {
                 fprintf(stderr, "Semaphore not locked!\n");
                 exit(1);
        }
        sem_unlock.sem_num = member;


        /* Attempt to lock the semaphore set */
        if((semop(sid, &sem_unlock, 1)) == -1)
        {
                fprintf(stderr, "Unlock failed\n");
                exit(1);
        }
        else
                printf("Semaphore resources incremented by one (unlocked)\n");


        dispval(sid, member);
}


void removesem(int sid)
{
        semctl(sid, 0, IPC_RMID, 0);
        printf("Semaphore removed\n");
}


unsigned short get_member_count(int sid)
{
        union semun semopts;
        struct semid_ds mysemds;


        semopts.buf = &mysemds;


        /* Return number of members in the semaphore set */
        return(semopts.buf->sem_nsems);
}
int getval(int sid, int member) {
        int semval;
	
        semval = semctl(sid, member, GETVAL, 0);
        return(semval);
}


void changemode(int sid, char *mode)
{
        int rc;
        union semun semopts;
        struct semid_ds mysemds;


        /* Get current values for internal data structure */



        semopts.buf = &mysemds;


        rc = semctl(sid, 0, IPC_STAT, semopts);


        if (rc == -1) {
                 perror("semctl");
                 exit(1);
        }


        printf("Old permissions were %o\n", semopts.buf->sem_perm.mode);


        /* Change the permissions on the semaphore */
        sscanf(mode, "%ho", &semopts.buf->sem_perm.mode);


        /* Update the internal data structure */
        semctl(sid, 0, IPC_SET, semopts);
}

#endif

void dispval(int sid, int member) {
        int semval;
        semval = semctl(sid, member, GETVAL, 0);
        printf("semval for member %d is %d\n", member, semval);
}


int shm_malloc(size_t segsize,void *segptr, key_t key) {
  int   shmid,cntr,i;

        /* Open the shared memory segment - create if necessary */
        if((shmid = shmget(key, segsize, IPC_CREAT|IPC_EXCL|0666)) == -1)  {
 
                 /* Segment probably already exists - try as a client */
                 if((shmid = shmget(key, segsize, 0)) == -1) {
                    error(-42,"create");    /* shm_malloc error.*/ 
                         return(-1);
                 }
        }
        
        /* Attach (map) the shared memory segment into the current process */
  if((segptr = shmat(shmid, 0, 0)) == -1) {
    error(-42,"map");    /* shm_malloc error.*/ 
    return(-1);    
  }
  return(shmid);
}



void shm_free(int shmid)
{
        shmctl(shmid, IPC_RMID, 0);
       /* printf("Shared memory segment marked for deletion\n");*/
}


void change_shm_mode(int shmid, char *mode) {
        struct shmid_ds myshmds;

        /* Get current values for internal data structure */
        shmctl(shmid, IPC_STAT, &myshmds);
        /* Display old permissions */
      /*  printf("Old permissions were: %o\n", myshmds.shm_perm.mode); */
        /* Convert and load the mode */
        sscanf(mode, "%o", &myshmds.shm_perm.mode);
        
        shmctl(shmid, IPC_SET, &myshmds);  /* Update the mode */
/*        printf("New permissions are : %o\n", myshmds.shm_perm.mode);*/
}

