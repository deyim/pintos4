#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

struct file_descriptor{
  int id; // file_Descirptor의 고유 넘버
  struct list_elem file_list_elem; // 연결되어 있는 리스트를 참조
  struct file* file; // 연결되어 있는 파일
};


tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
