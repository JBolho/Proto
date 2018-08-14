#include "acm.h"

/* data received from framework */
static struct acm_data_node_for_fwk_t {
	char pkgname[ACM_PKGNAME_MAX_LEN];
}temp_fwk_node;
DEFINE_MUTEX(fwk_data_mutex);

/* data received from mediaprovider */
static struct acm_data_node_for_mp_t {
	char pkgname[ACM_PKGNAME_MAX_LEN];
	char pic_path[ACM_PIC_PATH];
	int flag;
}temp_mp_node;
DEFINE_MUTEX(mp_data_mutex);

/* Hash table for white list */
acm_static acm_hash_table_t acm_hash;
/* List for framework */
acm_static acm_list_t acm_fwk_list;
/* List for DMD */
acm_static acm_list_t acm_dmd_list;

static dev_t acm_devno;
static struct cdev *acm_cdev;
static struct class *acm_class;
static struct device *acm_device;
static struct kset *acm_kset;
static struct timer_list acm_timer;

static struct task_struct *acm_main_task;

acm_static unsigned long dmd_upload_time_out;

static char uevent_pkgname[UEVENT_KEY_STR_MAX_LEN + ACM_PKGNAME_MAX_LEN];
static char uevent_pic_path[UEVENT_KEY_STR_MAX_LEN + ACM_PIC_PATH];
static char *uevent_val[UEVENT_LAST_VAL_INDEX + 1];

/* The flag of acm state after acm_init.
 * 0 is successful or none-zero errorno failed.
 * It should be initialized as none-zero. */
static long acm_init_state = -ENOTTY;

static struct control_dir {
	char full_path[36];     /* path with prefix */
	char path[32];          /* path */
	int nr_slashes;         /* slashes in path with prefix */
} dir[] = {
	{PATH_CAMERA_FULL,         PATH_CAMERA,          4},
	{PATH_DCIM_FULL,           PATH_DCIM,            3},
	{PATH_SCREENSHOTS_FULL,    PATH_SCREENSHOTS,     4},
	{PATH_PICTURES_FULL,       PATH_PICTURES,        3},
	{PATH_WEIXIN_FULL,         PATH_WEIXIN,          5},
	{PATH_MICROMSG_FULL,       PATH_MICROMSG,        4},
	{PATH_QQ_IMAGES_FULL,      PATH_QQ_IMAGES,       4},
	{PATH_TENCENT_FULL,        PATH_TENCENT,         3},
	{"0", "0", 0},
};

EXPORT_FOR_ACM_TEST(acm_hash);
EXPORT_FOR_ACM_TEST(acm_fwk_list);
EXPORT_FOR_ACM_TEST(acm_dmd_list);
EXPORT_FOR_ACM_TEST(dmd_upload_time_out);

acm_static int valid_len(char *str,unsigned long len) {
	unsigned long str_len;

	str_len = strlen(str);
	if (str_len == 0 || str_len > len) {
		return -EINVAL;
	}

	return ACM_SUCCESS;
}

 int acm_hash_init(void) {
	int i;
	struct hlist_head *head = NULL;

	head = (struct hlist_head*)kzalloc(sizeof(struct hlist_head) * ACM_HASH_TABLE_SIZE, GFP_KERNEL);
	if(!(head)) {
		printk("ACM:Cannot allocate memory for acm hash table.err = %d\n",-ENOMEM);
		return -ENOMEM;
	}
	for (i = 0; i < ACM_HASH_TABLE_SIZE; i++) {
		INIT_HLIST_HEAD(&(head[i]));
	}

	acm_hash.head = head;
	mutex_init(&acm_hash.mutex);
	return ACM_SUCCESS;
}

acm_static acm_hash_node_t *acm_hash_search(struct hlist_head *hash,char *keystring) {
	struct hlist_head *phead = NULL;
	acm_hash_node_t *pnode = NULL;

	phead = &hash[ELFHash(keystring)];
	mutex_lock(&acm_hash.mutex);
	hlist_for_each_entry(pnode,phead, hnode) {
		if (pnode->pkgname) {
			if (!strcmp(pnode->pkgname,keystring)) {
				break;
			}
		}
	}
	mutex_unlock(&acm_hash.mutex);
	return pnode;
}

/* ELFHash Function */
acm_static unsigned int ELFHash(char *str) {
	unsigned int hash = 0;
	unsigned int x = 0;
	unsigned ret = 0;

	while (*str) {
		hash = (hash << 4) + (*str++);
		if ((x = hash & 0xF0000000L) != 0) {
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	ret = (hash & 0x7FFFFFFF) % ACM_HASH_TABLE_SIZE;
	return ret;
}

static void acm_list_init (acm_list_t *list) {
	INIT_LIST_HEAD(&list->head);
	list->nr_nodes = 0;
	mutex_init(&list->mutex);
}


acm_static void acm_fwk_add(struct list_head *head,acm_list_node_t *list_node) {
	mutex_lock(&acm_fwk_list.mutex);
	list_add_tail(&list_node->lnode,head);
	mutex_unlock(&acm_fwk_list.mutex);
}

/* Called only for adding nodes to dmd list */
acm_static enum CHECK_STATE should_add(struct list_head *head, char *pkgname, char *pic_path) {
	enum CHECK_STATE ret = DEFAULT_STATE;
	acm_list_node_t *tail_node;

	if (list_empty(&acm_dmd_list.head)) {
		return LIST_EMPTY;
	}

	tail_node = (acm_list_node_t *)list_entry(head->prev,acm_list_node_t,lnode);
	if (strcmp(tail_node->pkgname,pkgname)) {
		ret = PKGNAME_DIFF;
	} else {
		if (strcmp(tail_node->pic_path,pic_path)) {
			ret = PIC_PATH_DIFF;
		} else {
			ret = SAME_PKG_AND_PATH;
		}
	}

	return ret;
}

acm_static void acm_dmd_add(struct list_head *head,acm_list_node_t *list_node) {
	acm_list_node_t *tail_node;

	mutex_lock(&acm_dmd_list.mutex);
	if (acm_dmd_list.nr_nodes > ACM_DMD_MAX_NODES - 1) {
		printk("ACM:List was too long!Data will be dropped!Pkgname=%s\n",
			list_node->pkgname);
		mutex_unlock(&acm_dmd_list.mutex);
		return;
	}
	if (should_add(head,list_node->pkgname,list_node->pic_path)) {
		list_add_tail(&list_node->lnode,head);
		acm_dmd_list.nr_nodes++;
		mutex_unlock(&acm_dmd_list.mutex);
	} else {
		tail_node = (acm_list_node_t *)list_entry(head->prev,acm_list_node_t,lnode);
		//tail_node must NOT be NULL here
		if (tail_node) {
			tail_node->pic_nr++;
		} else {
			printk("ACM:Unknown error!tail_node=%p",tail_node);
		}
		mutex_unlock(&acm_dmd_list.mutex);
		kfree(list_node);
	}
}

static int do_cmd_add(unsigned long args) {
	int err = 0;
	acm_hash_node_t *temp_hash_node = NULL;
	struct hlist_head * phead = NULL;
	struct hlist_head *hash = acm_hash.head;

	temp_hash_node = (acm_hash_node_t *)kzalloc(sizeof(acm_hash_node_t),GFP_KERNEL);
	if (!temp_hash_node) {
		printk("Failed to allocate memory for acm hash node!\n");
		return -ENOMEM;
	}
	INIT_HLIST_NODE(&temp_hash_node->hnode);

	if (copy_from_user(temp_hash_node->pkgname,(char *)args,ACM_PKGNAME_MAX_LEN)) {
		err = -EFAULT;
		goto do_cmd_add_fail;
	}

	if (valid_len(temp_hash_node->pkgname,ACM_PKGNAME_MAX_LEN - 1)) {
		err = -EINVAL;
		goto do_cmd_add_fail;
	}
	temp_hash_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';

	phead = &hash[ELFHash(temp_hash_node->pkgname)];
	mutex_lock(&acm_hash.mutex);
	hlist_add_head(&temp_hash_node->hnode, phead);
	mutex_unlock(&acm_hash.mutex);
	return err;

do_cmd_add_fail:
	kfree(temp_hash_node);
	return err;
}

static int do_cmd_delete(unsigned long args) {
	int err = 0;
	acm_hash_node_t *temp_hash_node = NULL;

	mutex_lock(&fwk_data_mutex);
	memset(&temp_fwk_node,0,sizeof(struct acm_data_node_for_fwk_t));
	if (copy_from_user(temp_fwk_node.pkgname,(char *)args,ACM_PKGNAME_MAX_LEN)) {
		err = -EFAULT;
		goto do_cmd_delete_fail;
	}
	if (valid_len(temp_fwk_node.pkgname,ACM_PKGNAME_MAX_LEN -1 )) {
		err = -EINVAL;
		goto do_cmd_delete_fail;
	}
	temp_fwk_node.pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';

	temp_hash_node = acm_hash_search(acm_hash.head,temp_fwk_node.pkgname);
	if(!temp_hash_node) {
		printk("ACM:Data not found!Pkgname is %s\n",temp_fwk_node.pkgname);
		err = -ENODATA;
		goto do_cmd_delete_fail;
	}
	mutex_unlock(&fwk_data_mutex);

	printk("ACM:Delete pkgname:%s\n",temp_hash_node->pkgname);
	mutex_lock(&acm_hash.mutex);
	hlist_del(&(temp_hash_node->hnode));
	mutex_unlock(&acm_hash.mutex);
	kfree(temp_hash_node);
	return err;

do_cmd_delete_fail:
	mutex_unlock(&fwk_data_mutex);
	return err;
}

static void remove_file_name(char *path)
{
	int len = VALID_STR_SIZE(strlen(path), ACM_PIC_PATH - 1) - 1;

	while (len >= 0) {
		if (unlikely(path[len] == '/')) {
			path[len] = '\0';
			break;
		} else {
			path[len] = '\0';
		}
		len--;
	}
}

static void unify_path_prefix(char *p1, char *p2)
{
	if (!strncmp(p2, PICPATH_PREFIX_STR, PICPATH_PREFIX_STR_SIZE)) {
		p2 = (char *)((unsigned long)p2 + PICPATH_PREFIX_STR_SIZE);
		snprintf(p1, ACM_PIC_PATH - 1, "/media%s", p2);
	} else {
		memcpy(p1, p2, ACM_PIC_PATH - 1);
	}
	p1[ACM_PIC_PATH - 1] = '\0';
}

static int do_cmd_search(unsigned long args) {
	int err = 0;
	acm_hash_node_t *temp_hash_node = NULL;

	mutex_lock(&mp_data_mutex);
	memset(&temp_mp_node,0,sizeof(struct acm_data_node_for_mp_t));
	if (copy_from_user(&temp_mp_node,(struct acm_data_node_for_mp_t *)args,
		sizeof(struct acm_data_node_for_mp_t))) {
		err = -EFAULT;
		goto do_cmd_search_ret;
	}
	if (valid_len(temp_mp_node.pkgname,ACM_PKGNAME_MAX_LEN - 1)) {
		err = -EINVAL;
		goto do_cmd_search_ret;
	}
	if (valid_len(temp_mp_node.pic_path,ACM_PIC_PATH - 1)) {
		err = -EINVAL;
		goto do_cmd_search_ret;
	}
	temp_mp_node.pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	temp_mp_node.pic_path[ACM_PIC_PATH - 1] = '\0';

	temp_hash_node = acm_hash_search(acm_hash.head,temp_mp_node.pkgname);
	(temp_hash_node)?(temp_mp_node.flag = 0):(temp_mp_node.flag = -1);

	if (copy_to_user((struct acm_data_node_for_mp_t *)args,&temp_mp_node,
		sizeof(struct acm_data_node_for_mp_t))) {
		err = -EFAULT;
		goto do_cmd_search_ret;
	}
	printk("ACM:Search result:pkgname=%s flag=%d\n",temp_mp_node.pkgname,
		temp_mp_node.flag);

#ifdef CONFIG_ACM_DSM
	acm_list_node_t *new_dmd_node = NULL;

	new_dmd_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t), GFP_KERNEL);
	if (!new_dmd_node) {
		printk("Failed to allocate memory for new_dmd_node!\n");
		err = -ENOMEM;
		goto do_cmd_search_ret;
	}

	memcpy(new_dmd_node->pkgname, temp_mp_node.pkgname,
		ACM_PKGNAME_MAX_LEN - 1);
	new_dmd_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	new_dmd_node->pic_nr = PIC_NR_INIT;

	remove_file_name(temp_mp_node.pic_path);
	memcpy(new_dmd_node->pic_path, temp_mp_node.pic_path, ACM_PIC_PATH - 1);
	unify_path_prefix(new_dmd_node->pic_path, temp_mp_node.pic_path);
	acm_dmd_add(&acm_dmd_list.head, new_dmd_node);
	if (likely(acm_main_task))
		wake_up_process(acm_main_task);
#endif

do_cmd_search_ret:
	mutex_unlock(&mp_data_mutex);
	return err;
}

static long acm_ioctl(struct file *filp, unsigned int cmd, unsigned long args) {
	int err=0;

	if (acm_init_state) {
		printk("ACM:ACM module wasn't ready!\n");
		return -ENOTTY;
	}

	if(_IOC_TYPE(cmd) != ACM_MAGIC) {
		return -EINVAL;
	}

	if(_IOC_NR(cmd) > ACM_CMD_MAXNR) {
		return -EINVAL;
	}

	if(_IOC_DIR(cmd) & _IOC_READ) {
		err = !access_ok(VERIFY_WRITE,(void *)args,_IOC_SIZE(cmd));
	} else if (_IOC_DIR(cmd) & _IOC_WRITE) {
		err = !access_ok(VERIFY_READ,(void *)args,_IOC_SIZE(cmd));
	}
	if (err) {
		return -EINVAL;
	}

	switch(cmd) {
		case ACM_ADD:
			err = do_cmd_add(args);
			break;
		case ACM_DEL:
			err = do_cmd_delete(args);
			break;
		case ACM_SEARCH:
			err = do_cmd_search(args);
			break;
		default:
			printk("ACM:Unknown command!\n");
			return -EINVAL;
	}

	return err;
}

void do_get_path_error(acm_list_node_t *node,struct dentry *dentry) {
	struct dentry *d[ERR_PATH_MAX_DENTRIES];
	char *path;
	int i;

	for(i=0; i<ERR_PATH_MAX_DENTRIES; i++) {
		d[i] = dentry;
	}

	/* Find the root dentry of the current file system.The d[i] saves the
		top ERR_PATH_MAX_DENTRIES-1 dentries to the root dentry. */
	while(!IS_ROOT(dentry)) {
		for (i=0; i<ERR_PATH_MAX_DENTRIES-1; i++) {
			d[i] = d[i+1];
		}
		dentry = d[ERR_PATH_MAX_DENTRIES - 1] = dentry->d_parent;
	}

	dentry = d[ERR_PATH_LAST_DENTRY];
	dentry = dget(dentry);
	path = dentry_path_raw(dentry, node->pic_path, ACM_DMD_PIC_PATH);
	if (IS_ERR(path)) {
		dput(dentry);
		printk("ACM:Unknown error!err=%lu\n",PTR_ERR(path));
		return;
	}
	dput(dentry);
	memcpy(node->pic_path, path, node->pic_path + ACM_DMD_PIC_PATH - path);
	node->pic_path[ACM_DMD_PIC_PATH - 1] = '\0';
}

int acm_search(char *pkgname,struct dentry *dentry,uid_t uid) {
	int err = 0;
	char *path = NULL;
	struct dentry *d_parent;
	acm_hash_node_t *temp_hash_node = NULL;
	acm_list_node_t *new_dmd_node = NULL;
	acm_list_node_t *new_fwk_node = NULL;

	if (!acm_main_task) {
		/* acm_main_task may be killed at running time.In this case,return 0
			to make acm invisible to file system */
		printk("ACM:acm_main_task wasn't ready!\n");
		return ACM_SUCCESS;
	}

	if (acm_init_state) {
		printk("ACM:ACM module wasn't ready!acm_init_state = %ld\n",acm_init_state);
		goto acm_search_fail;
	}

	if (!pkgname) {
		printk("ACM:Pakage name was NULL!\n");
		goto acm_search_fail;
	}

	if (valid_len(pkgname,ACM_PKGNAME_MAX_LEN -1)) {
		printk("ACM:The length of pakage name was invalid!\n");
		goto acm_search_fail;
	}
	pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';

#ifdef ACM_DSM
	new_dmd_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_NOFS);
	if (!new_dmd_node) {
		printk("ACM:Failed to allocate memory for new_dmd_node!\n");
		goto acm_search_fail;
	}
	memcpy(new_dmd_node->pkgname,pkgname,ACM_PKGNAME_MAX_LEN - 1);
	new_dmd_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	new_dmd_node->pic_nr = PIC_NR_INIT;

	d_parent = dget_parent(dentry);
	path = dentry_path_raw(d_parent, new_dmd_node->pic_path, ACM_DMD_PIC_PATH);
	if (IS_ERR(path)) {
		printk("ACM:Get dmd path error! dentry=%p error=%ld\n",d_parent,PTR_ERR(path));
		do_get_path_error(new_dmd_node,d_parent);
	} else {
		memcpy(new_dmd_node->pic_path, path, new_dmd_node->pic_path + ACM_DMD_PIC_PATH - path);
		new_dmd_node->pic_path[ACM_DMD_PIC_PATH - 1] = '\0';
	}
	dput(d_parent);
	acm_dmd_add(&acm_dmd_list.head,new_dmd_node);
#endif

	if (uid <= UID_BOUNDARY) {
		goto acm_search_ret;
	}

	temp_hash_node = acm_hash_search(acm_hash.head,pkgname);
	if(!temp_hash_node) {
		/* Data not found */
		new_fwk_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_NOFS);
		if (!new_fwk_node) {
			printk("ACM:Failed to allocate memory for new_fwk_node!\n");
			goto acm_search_fail;
		}
		memcpy(new_fwk_node->pkgname,pkgname,ACM_PKGNAME_MAX_LEN - 1);
		new_fwk_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
		new_fwk_node->pic_nr = PIC_NR_INIT;

		dentry = dget(dentry);
		path = dentry_path_raw(dentry, new_fwk_node->pic_path, ACM_PIC_PATH);
		if (IS_ERR(path)) {
			printk("ACM:Get fwk path error! dentry=%p error=%ld\n",dentry,PTR_ERR(path));
			dput(dentry);
			kfree(new_fwk_node);

			//Pic path is too long to upload to framework.Just delete the pic.
			goto acm_search_fail;
		}
		dput(dentry);
		memcpy(new_fwk_node->pic_path, path, new_fwk_node->pic_path + ACM_PIC_PATH - path);
		new_fwk_node->pic_path[ACM_PIC_PATH - 1] = '\0';
		acm_fwk_add(&acm_fwk_list.head,new_fwk_node);

		/* ENODATA means "pakage name is not in the white list" here. */
		err = -ENODATA;
	} else {
		printk("ACM:Pakage name is in the white list!\n");
	}

acm_search_ret:
	wake_up_process(acm_main_task);
	return err;

acm_search_fail:
	/* Any unexpected failure of acm_search should return 0
		to make acm invisible to file system */
	wake_up_process(acm_main_task);
	return ACM_SUCCESS;
}

acm_static void acm_fwk_upload(void) {
	int err = 0,tries;
	acm_list_node_t *temp_fwk_node;
	struct list_head *pos;

	while (1) {

		mutex_lock(&acm_fwk_list.mutex);
		if (list_empty(&acm_fwk_list.head)) {
			mutex_unlock(&acm_fwk_list.mutex);
			break;
		}
		mutex_unlock(&acm_fwk_list.mutex);

		pos = acm_fwk_list.head.next;
		temp_fwk_node = (acm_list_node_t *)list_entry(pos,acm_list_node_t,lnode);

		memset(uevent_pkgname,0,ACM_PKGNAME_MAX_LEN);
		memset(uevent_pic_path,0,ACM_PIC_PATH);
		snprintf(uevent_pkgname, sizeof(uevent_pkgname), "PKGNAME=%s", temp_fwk_node->pkgname);
		snprintf(uevent_pic_path, sizeof(uevent_pic_path), "PIC_PATH=%s", temp_fwk_node->pic_path);

		uevent_val[UEVENT_PKGNAME_INDEX] = uevent_pkgname;
		uevent_val[UEVENT_PIC_PATH_INDEX] = uevent_pic_path;
		uevent_val[UEVENT_LAST_VAL_INDEX] = NULL;
		tries = ACM_MAX_TRIES;
		printk("ACM:%s %s\n", __func__, uevent_pkgname);
		while ( tries-- ) {
			err = kobject_uevent_env(&(acm_cdev->kobj),KOBJ_CHANGE,uevent_val);
			if (!err) {
				break;
			}
		}

		if ( err || !tries ) {
			printk("ACM:Failed to send uevents!%s %s err = %d\n",
				uevent_pkgname,uevent_pic_path,err);
		}

		mutex_lock(&acm_fwk_list.mutex);
		list_del(pos);
		mutex_unlock(&acm_fwk_list.mutex);
		kfree(temp_fwk_node);
	}
}

void remove_user_dir(acm_list_node_t *node)
{
	int i = 0;
	char *path = node->pic_path;

	for (i = 0; dir[i].full_path[0] != '0'; i++) {
		if (!strncmp(path, dir[i].full_path, strlen(dir[i].full_path))) {
			memset(path, 0, sizeof(node->pic_path));
			memcpy(path, dir[i].path, sizeof(dir[i].path));
			break;
		}
	}

	if (dir[i].full_path[0] == '0') {
		memset(path, 0, sizeof(node->pic_path));
		memcpy(path, PATH_UNKNOWN, sizeof(PATH_UNKNOWN));
	}
}

static void do_dmd_upload(void) {
	acm_list_node_t *temp_dmd_node;
	struct list_head *pos;
	unsigned long dsm_time_out;

	dsm_time_out = jiffies + ACM_DSM_THRESHOLD;
	while (!list_empty(&acm_dmd_list.head) && time_before(jiffies,dsm_time_out)) {
		pos = acm_dmd_list.head.next;
		temp_dmd_node = (acm_list_node_t *)list_entry(pos,acm_list_node_t,lnode);

		if (f2fs_dclient && !dsm_client_ocuppy(f2fs_dclient)) {
			remove_user_dir(temp_dmd_node);
			dsm_client_record(f2fs_dclient,"%s %d %s\n",temp_dmd_node->pkgname,
				(int)temp_dmd_node->pic_nr, temp_dmd_node->pic_path);
			dsm_client_notify(f2fs_dclient, ACM_DELETE_INFO);
			printk("ACM:%s acm_dmd_list.nr_nodes:%d pkgname:%s pic_nr:%d pic_path:%s\n",
				__func__,(int)acm_dmd_list.nr_nodes,temp_dmd_node->pkgname, (int)temp_dmd_node->pic_nr,
				temp_dmd_node->pic_path);
			mutex_lock(&acm_dmd_list.mutex);
			list_del(pos);
			acm_dmd_list.nr_nodes--;
			mutex_unlock(&acm_dmd_list.mutex);
			kfree(temp_dmd_node);
			dsm_time_out = jiffies + ACM_DSM_THRESHOLD;
		}
	}
}

acm_static void acm_dmd_upload(void) {
#ifdef ACM_DSM
	if (!f2fs_dclient) {
		printk("ACM:Failed to upload data to dmd.f2fs_dclient was not ready!\n");
		return;
	}

	if (acm_dmd_list.nr_nodes > ACM_DMD_UPLOAD_NR_THRESHOLD
		|| (time_after(jiffies,dmd_upload_time_out))) {
		do_dmd_upload();

		if (!list_empty(&acm_dmd_list.head)) {
			printk("ACM:f2fs_client is busy for %dms!\n",ACM_DSM_THRESHOLD_MS);
		} else {
			dmd_upload_time_out = jiffies + ACM_DMD_UPLOAD_THRESHOLD - 1;
		}
	}
#endif
}

static int acm_main_loop(void *data) {

	while (!kthread_should_stop()) {

		set_current_state(TASK_INTERRUPTIBLE);

		acm_fwk_upload();
		acm_dmd_upload();

		schedule();
	}
	return ACM_SUCCESS;
}

static struct file_operations acm_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = acm_ioctl,
};

void acm_timer_func(unsigned long data) {
	mod_timer(&acm_timer,jiffies + ACM_DMD_UPLOAD_THRESHOLD);
	if (acm_main_task) {
		wake_up_process(acm_main_task);
	}
}

static int acm_main_task_init(void) {
	long err = 0;

	dmd_upload_time_out = jiffies + ACM_DMD_UPLOAD_THRESHOLD - 1;

	//Create the main task to asynchronously upload data to framework or DMD
	acm_main_task = kthread_run(acm_main_loop,NULL,"acm_main_loop");
	if (IS_ERR(acm_main_task)) {
		err = PTR_ERR(acm_main_task);
		printk("ACM:Failed to create acm main task!err = %ld\n",err);
		return err;
	}

	init_timer(&acm_timer);
	acm_timer.function = acm_timer_func;
	acm_timer.expires = jiffies + ACM_DMD_UPLOAD_THRESHOLD;
	add_timer(&acm_timer);

	return err;
}

static int acm_chr_dev_init(void) {

	long err=0;

	//Dynamiclly allocate a device number
	err = alloc_chrdev_region(&acm_devno,ACM_DEV_BASE_MINOR,ACM_DEV_COUNT,ACM_DEV_NAME);
	if (err) {
		printk("ACM:chr_dev_init:Device number allocation failed!err = %ld\n",err);
		return err;
	}

	//Initialize and add a cdev data structure to kernel
	acm_cdev = cdev_alloc();
	if (!acm_cdev) {
		err = -ENOMEM;
		printk("ACM:Failed to allocate memory for cdev data structure!err = %ld\n",err);
		goto free_devno;
	}
	acm_cdev->owner = THIS_MODULE;
	acm_cdev->ops = &acm_fops;
	err = cdev_add(acm_cdev,acm_devno,ACM_DEV_COUNT);
	if (err) {
		printk("ACM:Failed to register cdev to kernel!err = %ld\n",err);
		goto free_cdev;
	}

	//Dynamiclly create a device file
	acm_class = class_create(THIS_MODULE,ACM_DEV_NAME);
	if (IS_ERR(acm_class)) {
		err = PTR_ERR(acm_class);
		printk("ACM:Failed to create a class!err = %ld\n",err);
		goto free_cdev;
	}
	acm_device = device_create(acm_class,NULL,acm_devno,NULL,ACM_DEV_NAME);
	if (IS_ERR(acm_device)) {
		err = PTR_ERR(acm_device);
		printk("ACM:Failed to create a class!err = %ld\n",err);
		goto free_class;
	}

	//Initialize uevent stuff
	acm_kset = kset_create_and_add(ACM_DEV_NAME,NULL,kernel_kobj);
	if (!acm_kset) {
		err = PTR_ERR(acm_kset);
		printk("ACM:Failed to create kset!err = %ld\n",err);
		goto free_device;
	}
	acm_cdev->kobj.kset = acm_kset;
	acm_cdev->kobj.uevent_suppress = 0;
	err = kobject_add(&(acm_cdev->kobj),&(acm_kset->kobj),"acm_cdev_kobj");
	if (err) {
		kobject_put(&(acm_cdev->kobj));
		printk("ACM:Failed to add kobject to kernel!err = %ld\n",err);
		goto free_kset;
	}

	printk("ACM:Initialize acm character device succeed!\n");
	return err;

free_kset:
	kset_unregister(acm_kset);
free_device:
	device_destroy(acm_class,acm_devno);
free_class:
	class_destroy(acm_class);
free_cdev:
	cdev_del(acm_cdev);
free_devno:
	unregister_chrdev_region(acm_devno,ACM_DEV_COUNT);

	printk("ACM:Failed to initialize acm character device!err = %ld\n",err);
	return err;
}

static int __init acm_init(void) {

	long err = 0;

	//Initialize hash table
	err = acm_hash_init();
	if (err) {
		printk("ACM:Failed to initialize hash table!err = %ld\n",err);
		goto fail_hash;
	}

	acm_list_init(&acm_fwk_list);
	acm_list_init(&acm_dmd_list);

	err = acm_main_task_init();
	if (err) {
		printk("ACM:Failed to initialize main task!err = %ld\n",err);
		goto fail_task;
	}

	//Initialize acm character device
	err = acm_chr_dev_init();
	if (err) {
		printk("ACM:Failed to initialize acm character device!err = %ld\n",err);
		goto fail_task;
	}

	printk("ACM:Initialize ACM moudule succeed!\n");

	acm_init_state = err;
	return err;

fail_task:
	kfree(acm_hash.head);
fail_hash:
	acm_init_state = err;
	return err;
}

static void __exit acm_exit(void) {

	device_destroy(acm_class,acm_devno);
	class_destroy(acm_class);
	cdev_del(acm_cdev);
	unregister_chrdev_region(acm_devno,ACM_DEV_COUNT);
	kset_unregister(acm_kset);

	printk("ACM:Exited from ACM module.\n");
}

MODULE_LICENSE("GPL");
module_init(acm_init);
module_exit(acm_exit);
