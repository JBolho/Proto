#include <linux/fs.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/rtc.h>
#include <linux/statfs.h>
#include <linux/kernel.h>
#include <linux/bug.h>

#ifdef HW_SAVE_CFI_LOG
#define CFI_HISTORY_PATH "/log/cfi/cfi_history.log"
#define CFI_LOG_DIR "/log/cfi"
#define CFI_FILE_LIMIT 0664
#define CFI_DIR_LIMIT 0775
static char* get_current_asctime(void)
{
    struct timeval tv = {0};
    unsigned long long rtc_time;
    struct rtc_time tm;
    static char asctime[32];

    /*1.get rtc timer*/
    do_gettimeofday(&tv);
    rtc_time = (unsigned long long)tv.tv_sec;

    /*2.convert rtc to asctime*/
    memset((void *)asctime, 0, sizeof(asctime));
    memset((void *)&tm, 0, sizeof(struct rtc_time));
    rtc_time_to_tm(rtc_time, &tm);
    snprintf(asctime, sizeof(asctime) - 1, "%04d%02d%02d%02d%02d%02d\n",
         tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
         tm.tm_hour, tm.tm_min, tm.tm_sec);

     return asctime;
}

static int cfi_save_history_log(void)
{
    int ret = -1;
    int fd = -1;
    int flags = 0;
    mm_segment_t old_fs;
    long bytes_write = 0L;
    void *buf;
    unsigned int len;

    pr_err("%s start\n",__func__);
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    buf = (void *)get_current_asctime();
    len = strlen((char *)buf);

    /* 0. mkdir for cfi */
    fd = sys_access(CFI_LOG_DIR, 0);
    if (0 != fd) {
        fd = sys_mkdir(CFI_LOG_DIR, CFI_DIR_LIMIT);
        if (fd < 0) {
            pr_err("create dir [%s] failed! [ret = %d]\n", CFI_LOG_DIR, fd);
            set_fs(old_fs);
            return fd;
        }
    }

    /* 1. open file for writing */
    flags = O_CREAT | O_WRONLY |  O_APPEND;
    fd = sys_open(CFI_HISTORY_PATH, flags, CFI_FILE_LIMIT);
    if (fd < 0) {
        pr_err("Open file [%s] failed!fd: %d\n", CFI_HISTORY_PATH, fd);
        goto __out;
    }

    /* 2. write data to file */
    bytes_write = sys_write(fd, buf, len);
    if ((long)len != bytes_write) {
        pr_err("write file [%s] failed!bytes_write: %ld, it shoule be: %ld\n",
            CFI_HISTORY_PATH, (long)bytes_write, (long)len);
        goto __out;
    }

    /* 3. write successfully, modify the value of ret */
    ret = 0;

__out:
    if (fd >= 0) {
        sys_sync();
        sys_close(fd);
    }

    set_fs(old_fs);
    return ret;
}
#endif

void __cfi_report(void)
{
	pr_err("Function indirect call exception detected by CFI.\n");
#ifdef HW_SAVE_CFI_LOG
	cfi_save_history_log();
#endif
	BUG();
}
EXPORT_SYMBOL(__cfi_report);
