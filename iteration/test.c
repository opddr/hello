#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <asm/pgtable.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <linux/preempt.h>


#define store_gdt(ptr) asm volatile( "sgdt %0":"=m" (*ptr) )
#define load_gdt(ptr) asm volatile( "lgdt %0":"=m" (*ptr) )



struct _TSS_32
{
	unsigned short prev_task_link, res0;
	unsigned esp0;
	unsigned short ss0, res8;
	unsigned esp1;
	unsigned short ss1,res16;
	unsigned esp2;
	unsigned short ss2, res24;
	unsigned cr3;
	unsigned eip;
	unsigned eflags;
	unsigned eax;
	unsigned ecx;
	unsigned edx;
	unsigned ebx;
	unsigned esp;
	unsigned ebp;
	unsigned esi;
	unsigned edi;
	unsigned short es,res72;
	unsigned short cs,res76;
	unsigned short ss,res80;
	unsigned short ds,res84;
	unsigned short fs,res88;
	unsigned short gs,res92;
	unsigned short ldt_set_selector, res96;
	unsigned short t_flag, io_map_base_address;
};



struct GDTINFO
{
   unsigned short limit1;           // The lower 16 bits of the limit.
   unsigned short base1;            // The lower 16 bits of the base.
   unsigned char  base2;         // The next 8 bits of the base.
   unsigned char  access;              // Access flags, determine what ring 
   unsigned char  limit2:4,granularity:4;
   unsigned char  base3;
}__attribute__((packed));



struct _TSS_32 *pTss,oldTSS,*pTss16;
unsigned backedupEIP=0;
unsigned c_cr3=0;

static void SwitchedRoutine(void)
{	
	int i=0;
	struct desc_ptr gdtptr;
	struct desc_struct *GDT;
	unsigned Base=0;
	
	store_gdt(&gdtptr);
	GDT = (struct desc_struct *)gdtptr.address;	
	Base = GDT[16].base0 | GDT[16].base1<<16 | GDT[16].base2<<24;
	pTss16 = (struct TSS_32 *)Base;
	pTss16->cr3 = c_cr3;


	
	

	for(i=0;1;i++)
		printk("switched\n");	
	



	//asm __volatile__("sti");
	asm __volatile__ (".byte 0xEA, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00");
	

	
}




static int hello_init(void)
{
	//preempt_disable();
	while(1)
		printk("iteration.....\n");
//	preempt_enable();
	return 0;
}

static void hello_exit(void)
{

	printk("end module\n=========================================================\n");

}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");



