#ifndef __ACM_H__
#define __ACM_H__

#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/hash.h>
#include <asm-generic/unistd.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/fs_struct.h>
#include <uapi/linux/limits.h>
#include <uapi/asm-generic/errno.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/acm_f2fs.h>

#define ACM_DEV_NAME "acm"
#define ACM_DEV_BASE_MINOR (0)
#define ACM_DEV_COUNT (1)

#define ACM_MAGIC 'a'
#define ACM_ADD _IOW(ACM_MAGIC,0,struct acm_data_node_for_fwk_t)
#define ACM_DEL _IOW(ACM_MAGIC,1,struct acm_data_node_for_fwk_t)
#define ACM_SEARCH _IOR(ACM_MAGIC,2,struct acm_data_node_for_mp_t)
#define ACM_CMD_MAXNR (2)

#define ACM_HASH_TABLE_SIZE (512)
#define ACM_PKGNAME_MAX_LEN (100)
#define ACM_PIC_PATH (1024)
#define PIC_NR_INIT (1)

#define ACM_DMD_BUFLEN (1024)
#define SPACE_LEN (1)
#define BUF_END_LEN (1)
#define ACM_PKGNAME_DMD_BUFLEN (ACM_PKGNAME_MAX_LEN + SPACE_LEN)
#define ACM_PIC_NR_DMD_BUFLEN (sizeof(unsigned long) + SPACE_LEN)
#define ACM_DMD_PIC_PATH (ACM_DMD_BUFLEN - ACM_PKGNAME_DMD_BUFLEN - ACM_PIC_NR_DMD_BUFLEN - BUF_END_LEN)
#define PICPATH_PREFIX_STR "/storage/emulated"
#define PICPATH_PREFIX_STR_SIZE (strlen(PICPATH_PREFIX_STR))
#define VALID_STR_SIZE(len, maxlen) (((len) > (maxlen)) ? (maxlen) : (len))
#define ACM_DELETE_INFO (928005000)

#define ERR_PATH_MAX_DENTRIES (6)
#define ERR_PATH_LAST_DENTRY (0)

#define ACM_DMD_UPLOAD_NR_THRESHOLD (10)
#define ACM_DMD_MAX_NODES (2048)
#define ACM_TICK (1000/HZ)
#define ACM_DMD_UPLOAD_THRESHOLD (60*60*2 * HZ)
#define ACM_DSM_THRESHOLD (HZ/20)
#define ACM_DSM_THRESHOLD_MS (ACM_DSM_THRESHOLD * ACM_TICK)

#define UEVENT_KEY_STR_MAX_LEN (10)
#define ACM_MAX_TRIES (3)

#define PATH_PREFIX "/media/0"
#define PATH_CAMERA "/DCIM/Camera"
#define PATH_CAMERA_FULL (PATH_PREFIX PATH_CAMERA)
#define PATH_DCIM "/DCIM"
#define PATH_DCIM_FULL (PATH_PREFIX PATH_DCIM)
#define PATH_SCREENSHOTS "/Pictures/Screenshots"
#define PATH_SCREENSHOTS_FULL (PATH_PREFIX PATH_SCREENSHOTS)
#define PATH_PICTURES "/Pictures"
#define PATH_PICTURES_FULL (PATH_PREFIX PATH_PICTURES)
#define PATH_WEIXIN "/tencent/MicroMsg/WeiXin"
#define PATH_WEIXIN_FULL (PATH_PREFIX PATH_WEIXIN)
#define PATH_MICROMSG "/tencent/MicroMsg"
#define PATH_MICROMSG_FULL (PATH_PREFIX PATH_MICROMSG)
#define PATH_QQ_IMAGES "/tencent/QQ_Images"
#define PATH_QQ_IMAGES_FULL (PATH_PREFIX PATH_QQ_IMAGES)
#define PATH_TENCENT "/tencent"
#define PATH_TENCENT_FULL (PATH_PREFIX PATH_TENCENT)
#define PATH_UNKNOWN "unknown_path"

#define UID_BOUNDARY 10000
#define ACM_SUCCESS (0)

#ifdef CONFIG_ACM_TEST
#include <linux/debugfs.h>
#define acm_static
#define EXPORT_FOR_ACM_TEST(name) EXPORT_SYMBOL(name)
#else
#define acm_static static
#define EXPORT_FOR_ACM_TEST(name)
#endif

#ifdef CONFIG_HUAWEI_DSM
#define ACM_DSM
#endif

#ifdef ACM_DSM
#include <dsm/dsm_pub.h>
extern struct dsm_client *f2fs_dclient;
#endif

enum UEVENT_VAL_INDEX {
	UEVENT_PKGNAME_INDEX = 0,
	UEVENT_PIC_PATH_INDEX,
	UEVENT_LAST_VAL_INDEX
};

enum CHECK_STATE {
	DEFAULT_STATE =0,
	LIST_EMPTY = 1,
	PKGNAME_DIFF = 1,
	PIC_PATH_DIFF = 1,
	SAME_PKG_AND_PATH = 0
};

/* white list node */
typedef struct acm_hash_node {
	struct hlist_node hnode;
	char pkgname[ACM_PKGNAME_MAX_LEN];
}acm_hash_node_t;

typedef struct acm_hash_table {
	struct hlist_head *head;
	struct mutex mutex;
}acm_hash_table_t;

/* data node for framework and DMD */
typedef struct acm_list_node {
	struct list_head lnode;
	char pkgname[ACM_PKGNAME_MAX_LEN];
	char pic_path[ACM_PIC_PATH];
	unsigned long pic_nr;
}acm_list_node_t;

typedef struct acm_list {
	struct list_head head;
	unsigned long nr_nodes;
	struct mutex mutex;
}acm_list_t;

acm_static acm_hash_node_t *acm_hash_search(struct hlist_head *hash,char *keystring);
acm_static unsigned int ELFHash(char *str);

acm_static void acm_fwk_add(struct list_head *head,acm_list_node_t *list_node);
acm_static void acm_dmd_add(struct list_head *head,acm_list_node_t *list_node);
acm_static void acm_fwk_upload(void);
acm_static void acm_dmd_upload(void);
acm_static enum CHECK_STATE should_add(struct list_head *head, char *pkgname, char *pic_path);
acm_static int valid_len(char *str,unsigned long len);

#endif /* __ACM_H__ */
