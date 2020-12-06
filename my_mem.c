/**
 * 原始代码来自 Stack Overflow（https://stackoverflow.com/questions/41090469/linux-kernel-how-to-get-physical-address-memory-management）
 * 做了一些修改，可以处理用户传入的 pid 和 address
 */
#include  <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/page.h>

static pid_t pid;
module_param(pid, int,0644);

static unsigned long va;
module_param(va, ulong, 0644);


static void print_pgtable_macro(void) {
    printk("PAGE_OFFSET = 0x%lx\n", PAGE_OFFSET);
    printk("PGDIR_SHIFT = %d\n", PGDIR_SHIFT);
    printk("PUD_SHIFT = %d\n", PUD_SHIFT);
    printk("PMD_SHIFT = %d\n", PMD_SHIFT);
    printk("PAGE_SHIFT = %d\n", PAGE_SHIFT);

    printk("PTRS_PER_PGD = %d\n", PTRS_PER_PGD);
    printk("PTRS_PER_PUD = %d\n", PTRS_PER_PUD);
    printk("PTRS_PER_PMD = %d\n", PTRS_PER_PMD);
    printk("PTRS_PER_PTE = %d\n", PTRS_PER_PTE);

    printk("PAGE_MASK = 0x%lx\n", PAGE_MASK);
}
int my_module_init(void) {
    print_pgtable_macro();
    printk("\n");

    unsigned long pa = 0;

    pgd_t *pgd = NULL;
    pud_t *pud = NULL;
    pmd_t *pmd = NULL;
    pte_t *pte = NULL;

    struct pid *p = NULL;
    struct task_struct *task_struct = NULL;

    p = find_vpid(pid);
    if (p == NULL) {
        printk("find_vpid is null\n");
        return -1;
    }

    task_struct = pid_task(p, PIDTYPE_PID);
    if (task_struct == NULL) {
        printk("pid_task is null\n");
        return -1;
    }

    pgd = pgd_offset(task_struct->mm, va);
    printk("pgd_val = 0x%lx\n", pgd_val(*pgd));
    printk("pgd_index = %lu\n", pgd_index(va));

    if (pgd_none(*pgd)) {
        printk("Not mapped in pgd.\n");
        return -1;
    }
    pud = pud_offset(pgd, va);
    printk("pud_val = 0x%lx\n", pud_val(*pud));
    printk("pud_index = %lu\n", pud_index(va));

    if (pud_none(*pud)) {
        printk("Not mapped in pud.\n");
        return 0;
    }
    pmd = pmd_offset(pud, va);
    printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
    printk("pmd_index = %lu\n", pmd_index(va));

    if (pmd_none(*pmd)) {
        printk("Not mapped in pmd.\n");
        return 0;
    }
    pte = pte_offset_kernel(pmd, va);

    printk("pte_val = 0x%lx\n", pte_val(*pte));
    printk("pte_index = %lu\n", pte_index(va));

    if (pte_none(*pte)) {
        printk("Not mapped in pte.\n");
        return 0;
    }
    if (!pte_present(*pte)) {
        printk("pte not in RAM.\n");
        return 0;
    }
    unsigned long page_addr = 0;
    unsigned long page_offset = 0;
    page_addr = pte_val(*pte) & PAGE_MASK;
    page_addr &= 0x7fffffffffffffULL;

    page_offset = va & ~PAGE_MASK;
    pa = page_addr | page_offset;

    printk("page_addr = 0x%lx, page_offset = 0x%03lx\n", page_addr, page_offset);
    printk("virtual address 0x%lx in RAM Page is 0x%lx\n", va, pa);

    return 0;
}
void my_module_exit(void) {
    printk("module exit!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arthur.Zhang");
MODULE_DESCRIPTION("A simple virtual memory inspect");

