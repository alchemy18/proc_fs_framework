
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/proc_fs.h>
#include<linux/slab.h>
#include<linux/seq_file.h>
#include<linux/sched.h>

struct _mydrv_struct {
   struct list_head list; 	
   char info[12];        
                          
};

static LIST_HEAD(mydrv_list);
static struct proc_dir_entry *entry = NULL ; 



//new code for assignment 
static void *
mydrv_seq_start(struct seq_file *seq, loff_t *pos)
{
  struct task_struct *p; 
  loff_t off = 0;

  for_each_process(p) {
    if (*pos == off++) {
                        printk("in start : success %d\n",*pos);
                        return p;
    }
  }
  printk("in seq_start : over\n");
  return NULL;
}
static void *
mydrv_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
//struct task_struct *task=v;
struct list_head *n=((struct task_struct *)v)->tasks.next; 
++*pos;   
printk("in seq_next :%d\n",*pos);
return (n!=NULL)?
list_entry(n, struct task_struct, tasks):NULL;
}


/* show() method */
static int
mydrv_seq_show(struct seq_file *seq, void *v)
{
   int ret;
   const struct task_struct *p =v;

   char *info_str = kmalloc(256,GFP_KERNEL);
   /* Interpret the iterator, 'v' */
   printk("in seq_show \n");
   
   //sprintf(info_str, "pid is %d ppid is %d\n", p->pid, p->ppid);
   // printk("address of ppid is %x\n",p->parent); 
   sprintf(info_str, "pid is %d ppid is %d\n", p->pid,p->parent->pid);
  // sprintf(info_str, "pid is %d \n", p->pid);

   ret = seq_printf(seq,info_str);
   printk(KERN_INFO "the return value of seq_printf is %d\n", ret); 

   kfree(info_str); 
   return 0;
}


/* stop() method */
static void
mydrv_seq_stop(struct seq_file *seq, void *v)
{

   printk("in seq_stop:\n");
}


/* Define iterator operations */
static struct seq_operations mydrv_seq_ops = {
   .start = mydrv_seq_start,
   .next   = mydrv_seq_next,
   .stop   = mydrv_seq_stop,
   .show   = mydrv_seq_show,
};



static int
mydrv_seq_open(struct inode *inode, struct file *file)
{
   printk("we are in mydrv_seq_open\n");   //1

    

   return seq_open(file, &mydrv_seq_ops);
}


static struct file_operations mydrv_proc_fops = {
   .owner    = THIS_MODULE,    //this macro will provide the ptr to our module object
   .open     = mydrv_seq_open, /* User supplied */  //passing addresses of functions 
                                                    //to function pointers
   .read     = seq_read,       /* Built-in helper function */
   .llseek   = seq_lseek,       /* Built-in helper function */
   .release  = seq_release,    /* Built-in helper funciton */
};


static int __init
mydrv_init(void)
{

  int i;
  struct _mydrv_struct *mydrv_new;
  entry = proc_create("test", S_IRUSR, NULL,&mydrv_proc_fops);//a file is created 
  
    for (i=0;i<10;i++) {
       mydrv_new = kmalloc(sizeof(struct _mydrv_struct), GFP_KERNEL);
    //check errors
    sprintf(mydrv_new->info, "Node No: %d\n", i);
    list_add_tail(&mydrv_new->list, &mydrv_list);
  }

  printk("we are in init function of the module\n");  //2
  return 0;
}

static void mydrv_exit(void)
{
  //incomplete
  struct _mydrv_struct *p,*n;
  list_for_each_entry_safe(p,n, &mydrv_list, list) 
      kfree(p);

   remove_proc_entry("readme", NULL);
   printk("mydrv_exit just executed\n");    //3

}

module_init(mydrv_init);
module_exit(mydrv_exit);


//add other macros as needed 


