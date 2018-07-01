#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>

static struct proc_dir_entry *cfi_entry;
static struct proc_dir_entry *cfi_call_entry;
static struct proc_dir_entry *cfi_attack_entry;

/*roptest_impl and roptest_bad func addr offset*/
static unsigned long cfi_func_offset;

/*set addr_offset by ecall*/
void set_func_offset(unsigned int offset)
{
	cfi_func_offset = offset;
}

static void get_func_offset(void)
{
    unsigned long roptest_impl_addr;
    unsigned long roptest_bad_addr;
    roptest_impl_addr =  kallsyms_lookup_name("roptest_impl");
    roptest_bad_addr =  kallsyms_lookup_name("roptest_bad");
    cfi_func_offset = roptest_impl_addr - roptest_bad_addr;
    pr_err("roptest get_func_offset 0x%lx\n", cfi_func_offset);
    return;
}

static noinline void roptest_bad(const char *name)
{
	pr_err("roptest_bad \"%s\"\n", name);
}

static noinline void roptest_good(const char *name)
{
	pr_err("roptest_good \"%s\"\n", name);
}

static void roptest_impl(const char *name)
{
	if (!strncmp(name, "roptest", strlen("roptest")))
		roptest_good(name);
	else
		roptest_bad(name);
}

typedef void (*openfunc) (const char *name);

struct roptest_struct {
	char name[8];
	openfunc func;
};

static struct roptest_struct roptest = {
	.name = "roptest",
	.func = roptest_impl,
};

#define RECORD_SIZE 7
static void *cfi_violations[RECORD_SIZE * 2] = {NULL};

void __ifcc_logger(void *callee)
{
	if (*((uint32_t *)callee - 1) != CONFIG_HUAWEI_CFI_TAG) {
		void *retaddr = __builtin_return_address(0);
		int i = (unsigned long long)callee % RECORD_SIZE;

		cfi_violations[(long)(unsigned)(i*2+0)] = callee;
		cfi_violations[(long)(unsigned)(i*2+1)] = retaddr;
	}
}
EXPORT_SYMBOL(__ifcc_logger);

static ssize_t testcall_read(struct file *file, char __user *buf,
				size_t size, loff_t *ppos)
{
	pr_debug("roptest: hint: roptest.func = %pK\n", roptest.func);
	roptest.func(roptest.name);
	return 0;
}

static ssize_t testattack_read(struct file *file, char __user *buf,
				size_t size, loff_t *ppos)
{
	/**
	 * Use objdump or System.map to get the function address.
	 * I use relative address rather than abs addr,
	 * because sometime there's relocation.
	 */
	roptest.func = (openfunc)((char *)roptest_impl
			- cfi_func_offset); /*lint !e611*/
	return 0;
}

static int nop_open(struct inode *inode, struct file *file) { return 0; }

static int nop_release(struct inode *inode, struct file *file) { return 0; }

static const struct file_operations testcall_fops = {
	.open = nop_open,
	.read = testcall_read,
	.release = nop_release,
};

static const struct file_operations testattack_fops = {
	.open = nop_open,
	.read = testattack_read,
	.release = nop_release,
};

static int proccfi_open(struct inode *inode, struct file *file) { return 0; }

static int proccfi_release(struct inode *inode, struct file *file) { return 0; }

static ssize_t proccfi_read(struct file *file, char __user *buf,
				size_t size, loff_t *ppos)
{
	char kbuf[RECORD_SIZE * 40] = {0};
	int len = 0;
	int i;

	if (*ppos != 0)
		return 0;
	if (size < sizeof(kbuf))
		return -ENOMEM;

	for (i = 0; i < RECORD_SIZE; i++) {
		if (cfi_violations[(long)(unsigned)(i*2+0)] == NULL)
			continue;
		len += snprintf(kbuf+len, sizeof(kbuf)-len, "%p %p\n",
				cfi_violations[i*2+0], cfi_violations[i*2+1]);
	}

	if (copy_to_user(buf, kbuf, len))
		return -ENOMEM;
	*ppos += len;
	return len;
}

static const struct file_operations proccfi_fops = {
	.open  = proccfi_open,
	.read  = proccfi_read,
	.release = proccfi_release,
};

static int __init cfihelper_init(void)
{
    get_func_offset();
	cfi_entry = proc_create("cfi", S_IRUSR | S_IRGRP | S_IROTH, NULL, &proccfi_fops);
	if (!cfi_entry) {
		pr_err("proc_create cfi fail\n");
		return -1;
	}

	cfi_call_entry = proc_create("cfitest-call", S_IRUSR | S_IRGRP | S_IROTH, NULL,
					&testcall_fops);
	if (!cfi_call_entry) {
		pr_err("proc_create cfitest-call fail\n");
		return -1;
	}

	cfi_attack_entry = proc_create("cfitest-attack", S_IRUSR | S_IRGRP | S_IROTH, NULL,
					&testattack_fops);
	if (!cfi_attack_entry) {
		pr_err("proc_create cfitest-attack fail\n");
		return -1;
	}
	return 0;
}

static void __exit cfihelper_exit(void)
{
	if (cfi_entry)
		proc_remove(cfi_entry);

	if (cfi_call_entry)
		proc_remove(cfi_entry);

	if (cfi_attack_entry)
		proc_remove(cfi_entry);

}

module_init(cfihelper_init);
module_exit(cfihelper_exit);
MODULE_LICENSE("GPL");
