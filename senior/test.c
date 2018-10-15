#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <linux/delay.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h> //IT IS NOT asm/uasccess.h
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/string.h>
//#include <linux/slab.h>

extern int b_stop_scheduler;
MODULE_LICENSE("GPL");
typedef struct _TSS_32 {
   u16   prev_task_link, res0;
   u32   esp0;
   u16   ss0, res8;
   u32   esp1;
   u16   ss1, res16;
   u32   esp2;
   u16   ss2, res24;
   u32   cr3;
   u32   eip;
   u32   eflags;
   u32   eax;
   u32   ecx;
   u32   edx;
   u32   ebx;
   u32   esp;
   u32   ebp;
   u32   esi;
   u32   edi;
   u16   es, res72;
   u16   cs, res76;
   u16   ss, res80;
   u16   ds, res84;
   u16   fs, res88;
   u16   gs, res92;
   u16   ldt_set_selector, res96;
   u16   t_flag, io_map_base_address;
} TSS_32, *PTSS_32;
TSS_32 g_my_tss;
u32 c_cr3;

struct descriptor_table
{
   unsigned short limit;
   unsigned int base;
} __attribute__((packed));
unsigned int segment_base1(u16 selector)
{
   TSS_32 *print_tss;
   struct descriptor_table gdt;
   struct desc_struct *d  = NULL;
   unsigned int table_base;
   unsigned int v;
   u16 num=selector/8;
   /*if (selector == 0) 
      return 0; 
   */

   asm("sgdt %0" : "=m"(gdt));
   table_base = gdt.base;

   d = (struct desc_struct *)(table_base + (selector & ~7));
   v = d->base0 | ((unsigned long)d->base1 << 16) |
      ((unsigned long)d->base2 << 24);
   print_tss=v;
#ifdef CONFIG_X86_64 
   if (d->s == 0 && (d->type == 2 || d->type == 9 || d->type == 11))
      v |= ((unsigned long)((struct ldttss_desc64 *)d)->base3) << 32;

#endif
   printk(KERN_INFO "%d's BASE ADDRESS : %08x\n", num,v);
   /*
   printk(KERN_INFO "----------------------------------------------");
   printk(KERN_INFO "prev_task_link : %x\n",print_tss->prev_task_link);
   printk(KERN_INFO "esp0 : %x\n",print_tss->esp0);
   printk(KERN_INFO "cr3 : %x\n",print_tss->cr3);
   printk(KERN_INFO "eip : %x\n",print_tss->eip);
   printk(KERN_INFO "eflags : %x\n",print_tss->eflags);
   printk(KERN_INFO "esp : %x\n",print_tss->esp);
   printk(KERN_INFO "ebp : %x\n",print_tss->ebp);
   printk(KERN_INFO "es : %x\n",print_tss->es);
   printk(KERN_INFO "cs : %x\n",print_tss->cs);
   printk(KERN_INFO "ss : %x\n",print_tss->ss);
   printk(KERN_INFO "ds : %x\n",print_tss->ds);
   printk(KERN_INFO "fs : %x\n",print_tss->fs);
   printk(KERN_INFO "gs : %x\n",print_tss->gs);
   printk(KERN_INFO "io_map_base_address : %x\n",print_tss->io_map_base_address);
   */
   return v;
}
/*
unsigned int segment_base2(u16 selector)
{
   struct descriptor_table gdt;
   struct desc_struct *d  = NULL;
   unsigned int table_base;
   unsigned int v;
   asm("sgdt %0" : "=m"(gdt));
   table_base = gdt.base;
   d = (struct desc_struct *)(table_base + (selector & ~7));
   v = d->base0 | ((unsigned long)d->base1 << 16) |
      ((unsigned long)d->base2 << 24);
#ifdef CONFIG_X86_64 
   if (d->s == 0 && (d->type == 2 || d->type == 9 || d->type == 11))
      v |= ((unsigned long)((struct ldttss_desc64 *)d)->base3) << 32;

#endif
   printk(KERN_INFO "BASE ADDRESS : %08x\n", v);
   d->type=0x9;
   printk(KERN_INFO " type : %x\n",d->type);
   printk(KERN_INFO "----------------------------------------------");

   return 0;
}
*/
void test(void)
 {
	    int count=1;
	    struct file *fd=NULL;
	   // mm_segment_t old_fs=get_fs();
	    TSS_32 *tss_16;
	    //b_stop_scheduler=0;
	    tss_16=segment_base1(16*8);
	    tss_16->cr3=c_cr3;
	    while(1)
            {	
  	   	 __asm__ __volatile__ ("sti");
    
   		 printk("called : %d\n",count);
   		 printk ("show me the Ajaedll\n");
  		 //__asm__ __volatile__("movl %%cr3, %0":"=a"(tss_16->cr3));
  		 printk("16_tss's cr3 : %#x\n",tss_16->cr3);
  		 __asm__ __volatile__ (".byte 0xEA, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00");
  		 count++;
   	     }
}
int init_module(void)
{
   int count=1;
  // b_stop_scheduler=1;
   unsigned int my_eip=0;
   TSS_32 *my_tss;
   //memset(&g_my_tss,NULL,sizeof(TSS_32));
   printk("-------- changed before 31_tss ---------\n");
   __asm__ ("call 1f \n" "1:pop %0":"=m"(my_eip)); //eip_store
   printk(KERN_INFO "-----1. current eip_address:%#x\n",my_eip);//eip_print
   my_tss=segment_base1(31*8);
   //my_tss->esp0 = kmalloc(2,GFP_ATOMIC);
   //my_tss->esp = my_tss->esp0;
   my_tss->eip = test;
   my_tss->cs = 0x60;
   my_tss->ds = 0x7b;
   my_tss->ss = 0x68;
   my_tss->es = 0x7b;
   my_tss->fs = 0xd8;
   my_tss->io_map_base_address=(unsigned short)sizeof(TSS_32);
   my_tss->ss0 = 0x68;
   my_tss->gs = 0xe0;
   my_tss->prev_task_link=0x80;

   mdelay(4000);
   /*
   printk("---------7b_desc---------\n");
   segment_base2(120);
   printk("---------d8_desc---------\n");
   segment_base2(216);
   */
   //printk("-------- changed after 31_tss ---------\n");
   //segment_base1(31*8);
   //printk("-------- 16_tss ---------\n");
   //segment_base1(16*8);
     while(1)
   {
   __asm__ __volatile__("movl %%cr3, %0":"=r"(c_cr3));
   printk("current_cr3 : %#x\n",c_cr3);
   printk(KERN_INFO "------------ tss ------------------\n");
   printk(KERN_INFO "init_module()!!!!!! called\n");

   __asm__ __volatile__ ("cli\n"".byte 0xEA, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00");
   printk(KERN_INFO " %d call is success!!\n",count);
   count++;
   }
   return 0;
}
void cleanup_module(void)
{
   printk(KERN_INFO "cleanup_module() called\n");
}

