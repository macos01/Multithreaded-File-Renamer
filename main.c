#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#define MAX_FILES_IN_LIST 5
#define MAX_FILENAME_LENGTH 150

void *get_file_to_fix(void *arg);

int fix_name ();
int search_metadata();

void *rename_by_metadata (void *arg);
void *rename_by_name (void *arg);

pthread_mutex_t mutex_date;
pthread_mutex_t mutex_name;
pthread_cond_t queue_cond_metadata;
pthread_cond_t queue_cond_name;

char *work_directory = "/home/usuarioso/renamer";
char *input_directory = "./input";

char queue_fix_by_name[MAX_FILES_IN_LIST][MAX_FILENAME_LENGTH];
char queue_fix_by_metadata[MAX_FILES_IN_LIST][MAX_FILENAME_LENGTH];

int nqueue_cond_name = 0;
int nQueue_metadata = 0;

int main (int argc, char *argv[]){
  
  pthread_t th1, th2, th3;
  
  if(chdir(work_directory)){
    perror("Chdir main");
    exit(errno);
  }
  
  pthread_mutex_init (&mutex_date, NULL);
  pthread_mutex_init (&mutex_name, NULL);
  pthread_cond_init (&queue_cond_name, NULL);
  pthread_cond_init (&queue_cond_metadata, NULL);
  
  pthread_create (&th1, NULL, rename_by_name, NULL);
  pthread_create (&th2, NULL, rename_by_metadata, NULL);
  pthread_create (&th3, NULL, get_file_to_fix, NULL);
  
  pthread_join (th1, NULL);
  pthread_join (th2, NULL);
  pthread_join (th3, NULL);

  pthread_mutex_destroy(&mutex_date);
  pthread_mutex_destroy(&mutex_name);
  pthread_cond_destroy(&queue_cond_metadata);
  pthread_cond_destroy(&queue_cond_name);
  
  return 0;
}

void *get_file_to_fix(void *arg){
  
  DIR *dir;
  struct dirent *dp;
  
  dir = opendir(input_directory);
  if (dir == NULL) {
    perror("opendir get_file_to_fix()");
    exit(errno);
  }
  else {
    
    while ((dp = readdir(dir)) != NULL) {
      if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
	
	if (!strncmp(dp->d_name,"3",1)) {
	  pthread_mutex_lock (&mutex_name);
	  
	  fprintf(stdout,"%lu: %s => FIX NAME \n",(unsigned long)time(NULL),dp->d_name);
	  
	  strcpy(queue_fix_by_name[nqueue_cond_name],dp->d_name);
	  nqueue_cond_name++;
	  
	  pthread_cond_signal (&queue_cond_name);
	  pthread_mutex_unlock (&mutex_name);
	}
	else {
	  pthread_mutex_lock (&mutex_date);
	  
	  fprintf(stdout,"%lu: %s => RENAME BY METADATA \n",(unsigned long)time(NULL),dp->d_name);
	  
	  strcpy(queue_fix_by_metadata[nQueue_metadata],dp->d_name);
	  nQueue_metadata++;
	  
	  pthread_cond_signal (&queue_cond_metadata);
	  pthread_mutex_unlock (&mutex_date);
	}
      }
    }
    
    fprintf(stdout,"%lu: Fin asignar trabajo \n",(unsigned long)time(NULL));
    closedir(dir);
  
  }
  
  return NULL;
  
}

void *rename_by_name (void *arg) {
  
  char filename[MAX_FILENAME_LENGTH];
  
  while(1){
    
    pthread_mutex_lock (&mutex_name);
    
    while (nqueue_cond_name == 0) {
      pthread_cond_wait (&queue_cond_name, &mutex_name);
    }
    
    strcpy(filename, queue_fix_by_name[nqueue_cond_name - 1]);
    nqueue_cond_name--;
    
    pthread_mutex_unlock (&mutex_name);
    
    if(fix_name()){
      //ok
    }
    else {
      //pipe to socket
    }
  }
  
  return NULL;
}

void *rename_by_metadata (void *arg) {
  
  char filename[MAX_FILENAME_LENGTH];
  
  while(1){
    
    pthread_mutex_lock (&mutex_date);
    
    while (nQueue_metadata == 0) {
      pthread_cond_wait (&queue_cond_metadata, &mutex_date);
    }
   
    strcpy(filename, queue_fix_by_name[nQueue_metadata - 1]);
    nQueue_metadata--;
  
    pthread_mutex_unlock (&mutex_date);
    
    if(search_metadata()){
      //ok
    }
    else {
      //pipe to socket
    }
    
  }
  
  return NULL;
}

int search_metadata() {
  
  fprintf(stdout, "%lu: search_metadata 2 SEC \n",(unsigned long)time(NULL));
  sleep (2);
  fprintf(stdout, "%lu: search_metadata FIN \n", (unsigned long)time(NULL));
 
  return 0;
  
}

int fix_name() {
  
  fprintf(stdout, "%lu: search_name 2 SEC \n",(unsigned long)time(NULL));
  sleep (2);
  fprintf(stdout, "%lu: search_name FIN \n", (unsigned long)time(NULL));
 
  return 0;
}


