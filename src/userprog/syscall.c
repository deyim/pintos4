#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/input.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "threads/init.h"
#include "filesys/file.h"


static void syscall_handler (struct intr_frame *);
void exit(int status);
int read(int fd,void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void halt(void);
pid_t exec(const char *cmd_line);
int wait(pid_t pid);

////////// PROJECT 2 ////////////
bool create (const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize( int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

struct file_descriptor*
get_file_descriptor(struct thread *current, int fd);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
	int fd;
	unsigned size;
	char *buffer;
int *args = f->esp;
int returnVal;
switch(*(int*)(f->esp)){

	//printf("f->esp : %d\n", *(int*)(f->esp));

	case SYS_HALT:
		halt(); 
		break;
	case SYS_EXIT:
		f->eax=*(int*)(f->esp+4);
		exit((int)args[1]);
		break;
	case SYS_EXEC:
		returnVal = exec((const char*)(args[1]));
		f->eax = returnVal;
		break;
	case SYS_WAIT:
		returnVal = wait((pid_t)args[1]);
		f->eax = returnVal;
		break;
	case SYS_READ:
		fd = *(int*)(f->esp+4);
		returnVal = read(fd,(void*)args[2], (unsigned)args[3]);
		f->eax = returnVal;
		break;
	case SYS_WRITE:
		fd = *(int*)(f->esp+4);
		returnVal = write(fd, (void*)args[2], (unsigned)args[3]);
		f->eax = returnVal;
		break;
	case SYS_SUM:
		fd=*(int*)(f->esp+4);
		returnVal = summationFour((int)args[1],(int)args[2],(int)args[3],(int)args[4]);
		f->eax = returnVal;
		break;
	case SYS_FIBO:
		fd = *(int*)(f->esp+4);
		returnVal = fibo((int)args[1]);
		f->eax= returnVal;
		break;
	//////////PORJECT2////////////
	case SYS_CREATE :
		returnVal = create((const char*)args[1],(unsigned)args[2]);
		f->eax = returnVal;
		break;

	case SYS_REMOVE:
		returnVal = remove((const char*)args[1]);
		f->eax = returnVal;
		break;

	case SYS_OPEN:
		returnVal = open((const char*)args[1]);
		f->eax = returnVal;
		break;

	case SYS_CLOSE:
		fd = *(int*)(f->esp+4);
		close(fd);
		break;
	case SYS_FILESIZE:
		fd = *(int*)(f->esp+4);
		returnVal = filesize(fd);
		f->eax= returnVal;
		break;
	case SYS_SEEK:
		fd = *(int*)(f->esp+4);
		seek(fd,(unsigned)args[2]);
		break;
	case SYS_TELL:
		fd = *(int*)(f->esp+4);
		returnVal = tell(fd);
		f->eax = returnVal;
		break;
	default:
		exit(-1);
		break;


}


}


////////// PROJECT 2 ////////////

int filesize( int fd){
	struct thread *current=thread_current();
	struct file_descriptor* now_file_desc;
	int return_size;

	now_file_desc = get_file_descriptor(current,fd);
	if(now_file_desc != NULL){
		return_size = file_length(now_file_desc->file);
		return return_size;
	}
	else{
		return -1;
	}
}
int read(int fd, void *buffer, unsigned size){
	int i=0;
	uint8_t tmp;
	if(fd < 0) exit(-1);
	if(buffer > PHYS_BASE)
		exit(-1);

	//if(fd != 0) exit(-1); //stdin
	/////////////////////////////////////////
	if(fd > 2){
		//input getc file read
		struct thread *current = thread_current();
		struct file_descriptor* now_file_desc;

		now_file_desc = get_file_descriptor(current,fd);
		if(now_file_desc != NULL){
			int file_size = file_read(now_file_desc->file,buffer,size);
			return file_size;
		}
		else{
			return -1;
		}
	}
	else if(fd == 0){ 
		while(i<size)
		{
			tmp = input_getc();
			*(char*)(buffer+i) = tmp;
			i++;
		}
		return size;
	}
	else{
		return -1;
	}

}
int write(int fd, const void *buffer, unsigned size){
	struct thread *curThread = thread_current();
	//int status = 0;
	//if(fd < 0 || fd >= 128) exit(-1);
	if(fd<1) return -1;

	if((buffer >= PHYS_BASE || !pagedir_get_page(curThread->pagedir,buffer) || buffer == NULL)){
		exit(-1);
	}
	 if(fd == 1){
		putbuf((char*)buffer, size);
		return size;
	}
	else if(fd >2){
		////////////////////////////////////
		// file write
		struct thread *current = thread_current();
		struct file_descriptor* now_file_desc;

		now_file_desc = get_file_descriptor(current,fd);
		if(now_file_desc != NULL){
			int file_size = file_write(now_file_desc->file,buffer,size);
			return file_size;
		}
		else{
			return -1;
		}

	}

}


void close(int fd){
	struct thread *current=thread_current();
	struct file_descriptor* now_file_desc;

	now_file_desc = get_file_descriptor(current,fd);
	if(now_file_desc != NULL){
		file_close(now_file_desc->file);
		//list_remove(list_entry(now_file_desc->file_list_elem,struct file_descriptor,list_elem));
		list_remove(&(now_file_desc->file_list_elem));
		palloc_free_page(now_file_desc);
	}
}


bool create (const char *file, unsigned initial_size){
	bool return_status;

	if(file != NULL){
		return_status = filesys_create(file,initial_size);

	}
	else{
		exit(-1);
		return_status = 0;		
	}
	return return_status;
}

bool remove(const char *file){
	bool return_status;

	if(file != NULL){
		return_status = filesys_remove(file);
	}
	else{
		return_status = -1; //original : 0		
	}
	return return_status;
}
int open(const char *file){
	struct list *my_list = &thread_current()->file_descriptor_list;
	struct file_descriptor* now_file_desc;

	if(file == NULL){
		exit(-1);
	}


	now_file_desc = palloc_get_page(0);
	if(now_file_desc == NULL){
		return -1;
	}

	now_file_desc->file = filesys_open(file);
	if(now_file_desc->file == NULL){
		palloc_free_page(now_file_desc);
		return -1;
	}
	
	file_deny_write(now_file_desc->file);

	if(!list_empty(my_list)){
		now_file_desc->id = list_entry(list_back(my_list),struct file_descriptor,file_list_elem)->id +1;
		list_push_back(my_list,&(now_file_desc->file_list_elem));
	}
	else{
		now_file_desc->id = 3;
		list_push_back(my_list,&(now_file_desc->file_list_elem));
	}

	return now_file_desc->id;
}
void seek(int fd, unsigned position){
	struct thread *current=thread_current();
	struct file_descriptor* now_file_desc;

	now_file_desc = get_file_descriptor(current,fd);
	if(now_file_desc != NULL){
		file_seek(now_file_desc->file, position);
	}
	else{
		return;
	}
}
unsigned tell(int fd){
	struct thread *current=thread_current();
	struct file_descriptor* now_file_desc;
	unsigned retVal;

	now_file_desc = get_file_descriptor(current,fd);
	if(now_file_desc != NULL){
		retVal = file_tell(now_file_desc->file);
		return retVal;
	}
	else{
		return -1;
	}

}





void
halt (void){
    shutdown_power_off();
}

void exit(int status){
	struct thread *threadCur = thread_current();
	char *parsed, *token;
	char * tname = thread_name();
	struct thread * Parent;
	Parent = threadCur->Parent;
	
	if(status <-1)
		status = -1;
	
	if(threadCur){
		if(Parent!=NULL){		
			list_remove(&(threadCur->child_elem));
			threadCur->Parent->Exit_status = status;
		}
		token = strtok_r(tname, " ", &parsed);
		printf("%s: exit(%d)\n",token,status);

		if(Parent->status==THREAD_BLOCKED) thread_unblock(Parent);

	}
	thread_exit();

	return;
}

pid_t exec(const char * cmd_line){
	struct file *f ;
	char *parsed, *file_name, tmp[100];

	int returnVal;
	strlcpy(tmp, cmd_line,strlen(cmd_line)+1);
	file_name = strtok_r(tmp, " ", &parsed);
	//printf("%s\n", file_name);	
	f = filesys_open(file_name);

	if(f!= NULL){
		file_close(f);
		returnVal = process_execute(cmd_line);
		return returnVal;
	}
	else{
		return -1;
	}


}

int wait(pid_t pid){
	return process_wait(pid); //child_id 
}

int summationFour(int a, int b, int c, int d){
	int sum = 0;
	sum = a + b + c + d;
	return sum;
}

int fibo(int input){
	int i=0;
	int a1=0, a2=1, a3;
	if(input==0) return 0;
	else if(input==1) return 1;
	else if(input==2) return 1;
	while(i < input){
		a3 = a1 + a2;
		a1 = a2;
		a2 = a3;
		i++;
	}
	return a1;
}


struct file_descriptor*
get_file_descriptor(struct thread *current, int fd){
	struct list_elem* temp; // 루프 돌기 용

	if(list_empty(&current->file_descriptor_list)){
			return NULL;
		}

	for(temp = list_begin(&current->file_descriptor_list);;temp=list_next(temp)){
		struct file_descriptor* now_file_desc;
		now_file_desc = list_entry(temp, struct file_descriptor, file_list_elem);
		if(now_file_desc->id == fd){
			return now_file_desc;
		}
		if(temp == list_end(&current->file_descriptor_list)){
			return NULL;
		}
	}
	return NULL;
}