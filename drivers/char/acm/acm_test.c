#include "acm.h"

static struct dentry *acm_debugfs_root;

extern acm_hash_table_t acm_hash;
extern acm_list_t acm_fwk_list;
extern acm_list_t acm_dmd_list;
extern unsigned long dmd_upload_time_out;

void acm_hash_cleanup(struct hlist_head *hash) {
	int i;
	struct hlist_head *phead = NULL;
	acm_hash_node_t *pnode = NULL;

	for (i = 0; i < ACM_HASH_TABLE_SIZE; i++) {
	phead = &hash[i];
	mutex_lock(&acm_hash.mutex);
	while (!hlist_empty(phead)) {
		pnode = hlist_entry(phead->first, acm_hash_node_t, hnode);
		hlist_del(&pnode->hnode);
		kfree(pnode);
	}
	mutex_unlock(&acm_hash.mutex);
}
}

bool acm_hash_empty(struct hlist_head *hash) {
	struct hlist_head *phead = NULL;
	bool ret = true;
	int i;

	for (i=0; i < ACM_HASH_TABLE_SIZE; i++) {
		phead = &hash[i];
		if (!hlist_empty(phead)) {
			ret = false;
			break;
		}
	}

	return ret;
}

void acm_list_cleanup (acm_list_t *list) {
	acm_list_node_t *temp_list_node;
	struct list_head *head,*pos;

	mutex_lock(&list->mutex);
	head = &list->head;
	while(!list_empty(head)) {
		pos = head->next;
		temp_list_node = (acm_list_node_t *)list_entry(pos,acm_list_node_t,lnode);
		list_del(pos);
		kfree(temp_list_node);
	}
	list->nr_nodes = 0;
	mutex_unlock(&list->mutex);

	return;
}
static int acm_hash_table_open(struct inode *inode,struct file *filp) {
	int i,err = 0;
	struct hlist_head *phead = NULL;
	struct hlist_head *hash = acm_hash.head;
	acm_hash_node_t *pnode = NULL;

	filp->private_data = inode -> i_private;

	for (i=0; i<ACM_HASH_TABLE_SIZE; i++) {
		printk("===ACM:acm_hash[%d]===\n",i);
		phead = &(hash[i]);
		hlist_for_each_entry(pnode,phead, hnode) {
			if (pnode && pnode->pkgname) {
			printk("=======ACM:PKGNAME=%s\n",pnode->pkgname);
			}
		}
	}
	return err;
}

static int acm_fwk_list_open(struct inode *inode,struct file *filp) {
	int err = 0;
	acm_list_node_t *pnode = NULL;

	filp->private_data = inode -> i_private;

	printk("===ACM:acm_fwk_list===\n");
	list_for_each_entry(pnode,&acm_fwk_list.head,lnode) {
		if (pnode) {
			printk("=======ACM:pkgname=%s \t pic_path=%s \t pic_nr=%lu\n",pnode->pkgname,pnode->pic_path,pnode->pic_nr);
		}
	}

	return err;
}

static int acm_dmd_list_open(struct inode *inode,struct file *filp) {
	int err = 0;
	acm_list_node_t *pnode = NULL;

	filp->private_data = inode -> i_private;

	printk("===ACM:acm_dmd_list===\n");
	list_for_each_entry(pnode,&acm_dmd_list.head,lnode) {
		if (pnode) {
			printk("=======ACM:pkgname=%s \t pic_path=%s \t pic_nr=%lu\n",pnode->pkgname,pnode->pic_path,pnode->pic_nr);
		}
	}
	printk("===ACM:acm_dmd_list.nrnodes:%lu===\n",acm_dmd_list.nr_nodes);

	return err;
}

static int acm_test_valid_len(struct inode *inode,struct file *filp) {
	char test_null[5] = {'\0'};
	char test_normal[5] = {"123"};
	char test_full[5] = {'\0'};
	int i,len;

	filp->private_data = inode -> i_private;
	printk("ACM:===Testing valid_len()===\n");
	//Length is 0,return -EINVAL
	if (!valid_len(test_null,5)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	printk("ACM:VAL len=%u\n",strlen(test_normal));
	//3 > 2,return -EINVAL
	if (!valid_len(test_normal,2)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	//3 = 3,return ACM_SUCCESS
	if (valid_len(test_normal,3)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	//3 < 4,return ACM_SUCCESS
	if (valid_len(test_normal,4)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//String with no '\0' at the end
	for(i=0; i<5; i++) {
		test_full[i] = 'a';
	}
	len = strlen(test_full);
	printk("ACM:VAL len=%u\n",len);
	if ( len == 5) {
		//5 > 4,return -EINVAL
		if (!valid_len(test_full,4)) {
			printk("ACM:%d Error!",__LINE__);
			return -EINVAL;
		}
		//5 = 5,return ACM_SUCCESS
		if (valid_len(test_full,5)) {
			printk("ACM:%d Error!",__LINE__);
			return -EINVAL;
		}
		//5 < 6,return ACM_SUCCESS
		if (valid_len(test_full,6)) {
			printk("ACM:%d Error!",__LINE__);
			return -EINVAL;
		}
	} else {
		//x > 5,return -EINVAL
		if (!valid_len(test_full,5)) {
			printk("ACM:%d Error!",__LINE__);
			return -EINVAL;
		}
	}

	printk("ACM:===Testing valid_len() PASSED!===\n");
	return ACM_SUCCESS;

}

static int acm_test_should_add(struct inode *inode,struct file *filp) {
	enum CHECK_STATE ret = DEFAULT_STATE;
	acm_list_node_t *list_node;

	filp->private_data = inode -> i_private;

	//Clean dmd list first.
	acm_list_cleanup(&acm_dmd_list);
	if (!list_empty(&acm_dmd_list.head) || acm_dmd_list.nr_nodes != 0) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}

	printk("ACM:===Testing should_add()===\n");
	//Empty list,valid pkgname and pic_path,return true
	printk("ACM:VAL dmd_list_empty:%d\n",list_empty(&acm_dmd_list.head));
	ret = should_add(&acm_dmd_list.head,"hello","hello");
	if (!ret) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Add a node to dmd list
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"hello");
	list_node->pic_nr = 1;
	mutex_lock(&acm_dmd_list.mutex);
	list_add_tail(&list_node->lnode,&acm_dmd_list.head);
	acm_dmd_list.nr_nodes++;
	mutex_unlock(&acm_dmd_list.mutex);

	//pkgname==pkgname,pic_path==pic_path,return false
	printk("ACM:VAL dmd_list_empty:%d\n",list_empty(&acm_dmd_list.head));
	ret = should_add(&acm_dmd_list.head,"hello","hello");
	if (ret) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//pkgname!=pkgname.pic_path!=pic_path,return true
	ret = should_add(&acm_dmd_list.head,"h","h");
	if (!ret) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//pkgname!=pkgname,pic_path=pic_path,return true
	ret = should_add(&acm_dmd_list.head,"h","hello");
	if (!ret) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//pkgname==pkgname,pic_path!=pic_path,return true
	ret = should_add(&acm_dmd_list.head,"hello","h");
	if (!ret) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	printk("ACM:===Testing should_add() PASSED!!===\n");

	acm_list_cleanup(&acm_dmd_list);
	return ACM_SUCCESS;
}

static int acm_test_acm_dmd_add(struct inode *inode,struct file *filp) {
	acm_list_node_t *list_node;
	acm_list_node_t *tail_node;

	filp->private_data = inode -> i_private;

	//Clean dmd list first
	acm_list_cleanup(&acm_dmd_list);
	if (!list_empty(&acm_dmd_list.head) || acm_dmd_list.nr_nodes != 0) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}

	printk("ACM:===Testing acm_dmd_add()===\n");
	//Add a node to dmd list
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"hello");
	list_node->pic_nr = 1;
	acm_dmd_add(&acm_dmd_list.head,list_node);
	tail_node = (acm_list_node_t *)list_entry(acm_dmd_list.head.prev,acm_list_node_t,lnode);
	if (!tail_node) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	printk("ACM:VAL %s %s %s %s %d %d %lu\n",tail_node->pkgname,list_node->pkgname,tail_node->pic_path,list_node->pic_path,
		strcmp(tail_node->pkgname,list_node->pkgname),strcmp(tail_node->pic_path,list_node->pic_path),tail_node->pic_nr);
	if (strcmp(tail_node->pkgname,list_node->pkgname) || strcmp(tail_node->pic_path,list_node->pic_path) || tail_node->pic_nr != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (acm_dmd_list.nr_nodes != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Add an identical node
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"hello");
	list_node->pic_nr = 1;
	acm_dmd_add(&acm_dmd_list.head,list_node);
	tail_node = (acm_list_node_t *)list_entry(acm_dmd_list.head.prev,acm_list_node_t,lnode);
	if (strcmp(tail_node->pkgname,list_node->pkgname) || strcmp(tail_node->pic_path,list_node->pic_path) || tail_node->pic_nr != 2) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (acm_dmd_list.nr_nodes != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Add a different node
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}

	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"h");
	list_node->pic_nr = 1;
	acm_dmd_add(&acm_dmd_list.head,list_node);
	tail_node = (acm_list_node_t *)list_entry(acm_dmd_list.head.prev,acm_list_node_t,lnode);
	printk("ACM:%s %s %d\n",tail_node->pkgname,list_node->pkgname,strcmp(tail_node->pkgname,list_node->pkgname));
	printk("ACM:%s %s %d\n",tail_node->pic_path,list_node->pic_path,strcmp(tail_node->pic_path,list_node->pic_path));
	printk("ACM:%lu\n",tail_node->pic_nr);
	if (strcmp(tail_node->pkgname,list_node->pkgname) || strcmp(tail_node->pic_path,list_node->pic_path) || tail_node->pic_nr != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (acm_dmd_list.nr_nodes != 2) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	printk("ACM:===Testing acm_dmd__add() PASSED!!===\n");
	acm_list_cleanup(&acm_dmd_list);
	return ACM_SUCCESS;
}

static int acm_test_acm_fwk_upload(struct inode *inode,struct file *filp) {
	acm_list_node_t *list_node;
	acm_list_node_t *tail_node;

	filp->private_data = inode -> i_private;

	acm_list_cleanup(&acm_fwk_list);
	if (!list_empty(&acm_fwk_list.head) || acm_fwk_list.nr_nodes != 0) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}

	printk("ACM:===Testing acm_fwk_upload()===\n");
	//Add a node to fwk list,then do upload,List should be empty
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"h");
	list_node->pic_nr = 1;
	acm_fwk_add(&acm_fwk_list.head,list_node);
	tail_node = (acm_list_node_t *)list_entry(acm_fwk_list.head.prev,acm_list_node_t,lnode);
	if (strcmp(tail_node->pkgname,list_node->pkgname) || strcmp(tail_node->pic_path,list_node->pic_path) || tail_node->pic_nr != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	acm_fwk_upload();
	if (!list_empty(&acm_fwk_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Add 2 nodes to fwk list,then do upload.List should be empty
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"h");
	list_node->pic_nr = 1;
	acm_fwk_add(&acm_fwk_list.head,list_node);
	tail_node = (acm_list_node_t *)list_entry(acm_fwk_list.head.prev,acm_list_node_t,lnode);
	if (strcmp(tail_node->pkgname,list_node->pkgname) || strcmp(tail_node->pic_path,list_node->pic_path) || tail_node->pic_nr != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"h2");
	list_node->pic_nr = 1;
	acm_fwk_add(&acm_fwk_list.head,list_node);
	tail_node = (acm_list_node_t *)list_entry(acm_fwk_list.head.prev,acm_list_node_t,lnode);
	if (strcmp(tail_node->pkgname,list_node->pkgname) || strcmp(tail_node->pic_path,list_node->pic_path) || tail_node->pic_nr != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	acm_fwk_upload();
	if (!list_empty(&acm_fwk_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	printk("ACM:===Testing acm_fwk_upload() PASSED!===\n");
	return ACM_SUCCESS;
}

static int acm_test_acm_dmd_upload(struct inode *inode,struct file *filp) {
	acm_list_node_t *list_node;

	filp->private_data = inode -> i_private;

	acm_list_cleanup(&acm_dmd_list);
	if (!list_empty(&acm_dmd_list.head) || acm_dmd_list.nr_nodes != 0) {
		printk("ACM:Faile to execute test!\n");
		return -EINVAL;
	}

	printk("ACM:===Testing acm_dmd_upload()===\n");
	//Add a node to dmd list,then do upload,list should not be empty
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"hello");
	list_node->pic_nr = 1;
	acm_dmd_add(&acm_dmd_list.head,list_node);
	acm_dmd_upload();
	if (list_empty(&acm_dmd_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (acm_dmd_list.nr_nodes != 1) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	//Set nr_nodes to threshold
	dmd_upload_time_out = jiffies + HZ * 10;		//Set time_after() false
	acm_dmd_list.nr_nodes = ACM_DMD_UPLOAD_NR_THRESHOLD;
	acm_dmd_upload();
	if (list_empty(&acm_dmd_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	//Set nr_nodes to threshold + 1
	dmd_upload_time_out = jiffies + HZ * 10;		//Set time_after() false
	acm_dmd_list.nr_nodes = ACM_DMD_UPLOAD_NR_THRESHOLD + 1;
	acm_dmd_upload();
	if (!list_empty(&acm_dmd_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	printk("ACM:VAL dmd_upload_time_out=%lu jiffies=%lu jiffies_extern=%lu\n",dmd_upload_time_out,jiffies,(jiffies + ACM_DMD_UPLOAD_THRESHOLD));
	if (dmd_upload_time_out < (jiffies + ACM_DMD_UPLOAD_THRESHOLD - 1)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Set time_after() true
	acm_dmd_list.nr_nodes = 0;
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"h");
	list_node->pic_nr = 1;
	acm_dmd_add(&acm_dmd_list.head,list_node);
	dmd_upload_time_out = 1;
	printk("ACM:VAL jiffies=%lu dmd_upload_time_out=%lu time_after()=%d\n",jiffies,dmd_upload_time_out,time_after(jiffies,dmd_upload_time_out));
	acm_dmd_upload();
	if (!list_empty(&acm_dmd_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	printk("ACM:VAL dmd_upload_time_out=%lu jiffies=%lu jiffies_extern=%lu\n",dmd_upload_time_out,jiffies,(jiffies + ACM_DMD_UPLOAD_THRESHOLD));
	if (dmd_upload_time_out < (jiffies + ACM_DMD_UPLOAD_THRESHOLD - 1)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Set nr_nodes to threshold + 1 and time_after() true
	list_node = (acm_list_node_t *)kzalloc(sizeof(acm_list_node_t),GFP_KERNEL);
	if (!list_node) {
		printk("ACM:Faile to execute test!\n");
		return -ENOMEM;
	}
	strcpy(list_node->pkgname,"hello");
	strcpy(list_node->pic_path,"hello");
	list_node->pic_nr = 1;
	acm_dmd_add(&acm_dmd_list.head,list_node);
	dmd_upload_time_out = 1;
	acm_dmd_list.nr_nodes = ACM_DMD_UPLOAD_NR_THRESHOLD + 1;
	acm_dmd_upload();
	if (!list_empty(&acm_dmd_list.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	printk("ACM:VAL dmd_upload_time_out=%lu jiffies=%lu jiffies_extern=%lu\n",dmd_upload_time_out,jiffies,(jiffies + ACM_DMD_UPLOAD_THRESHOLD));
	if (dmd_upload_time_out < (jiffies + ACM_DMD_UPLOAD_THRESHOLD - 1)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	printk("ACM:===Testing acm_dmd_upload() PASSED!===\n");
	acm_list_cleanup(&acm_dmd_list);
	return ACM_SUCCESS;

}

static int acm_test_acm_hash_search(struct inode *inode,struct file *filp) {
	acm_hash_node_t *temp_hash_node;
	struct hlist_head *phead;
	struct hlist_head *hash = acm_hash.head;
	char teststr1[ACM_PKGNAME_MAX_LEN] = {"com.acm.testacm50"};
	char teststr2[ACM_PKGNAME_MAX_LEN] = {"com.acm.testacm51"};
	char teststr3[ACM_PKGNAME_MAX_LEN] = {"com.acm.testacm52"};

	filp->private_data = inode -> i_private;

	//Clean hash table,then add 3nodes
	acm_hash_cleanup(acm_hash.head);
	if (!acm_hash_empty(acm_hash.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -ENODATA;
	}
	temp_hash_node = (acm_hash_node_t *)kzalloc(sizeof(acm_hash_node_t),GFP_KERNEL);
	if (!temp_hash_node) {
		printk("Failed to execute test!\n");
		return -ENOMEM;
	}
	INIT_HLIST_NODE(&temp_hash_node->hnode);
	strcpy(temp_hash_node->pkgname,teststr1);
	temp_hash_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	phead = &hash[ELFHash(temp_hash_node->pkgname)];
	mutex_lock(&acm_hash.mutex);
	hlist_add_head(&temp_hash_node->hnode, phead);
	mutex_unlock(&acm_hash.mutex);

	temp_hash_node = (acm_hash_node_t *)kzalloc(sizeof(acm_hash_node_t),GFP_KERNEL);
	if (!temp_hash_node) {
		printk("Failed to execute test!\n");
		return -ENOMEM;
	}
	INIT_HLIST_NODE(&temp_hash_node->hnode);
	strcpy(temp_hash_node->pkgname,teststr2);
	temp_hash_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	phead = &hash[ELFHash(temp_hash_node->pkgname)];
	mutex_lock(&acm_hash.mutex);
	hlist_add_head(&temp_hash_node->hnode, phead);
	mutex_unlock(&acm_hash.mutex);

	temp_hash_node = (acm_hash_node_t *)kzalloc(sizeof(acm_hash_node_t),GFP_KERNEL);
	if (!temp_hash_node) {
		printk("Failed to execute test!\n");
		return -ENOMEM;
	}
	INIT_HLIST_NODE(&temp_hash_node->hnode);
	strcpy(temp_hash_node->pkgname,teststr3);
	temp_hash_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	phead = &hash[ELFHash(temp_hash_node->pkgname)];
	mutex_lock(&acm_hash.mutex);
	hlist_add_head(&temp_hash_node->hnode, phead);
	mutex_unlock(&acm_hash.mutex);

	printk("ACM:===Testing acm_hash_search()===\n");

	//Search for teststr1
	temp_hash_node = acm_hash_search(acm_hash.head,teststr1);
	if (!temp_hash_node) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (strcmp(temp_hash_node->pkgname,teststr1)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	//Search for teststr2
	temp_hash_node = acm_hash_search(acm_hash.head,teststr2);
	if (!temp_hash_node) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (strcmp(temp_hash_node->pkgname,teststr2)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	//Search for teststr3
	temp_hash_node = acm_hash_search(acm_hash.head,teststr3);
	if (!temp_hash_node) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (strcmp(temp_hash_node->pkgname,teststr3)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	printk("ACM:===Testing acm_hash_search() PASSED!===\n");
	return ACM_SUCCESS;
}

static int acm_test_ioctl(struct inode *inode,struct file *filp) {
	acm_hash_node_t *temp_hash_node;
	struct hlist_head *phead;
	struct hlist_head *hash = acm_hash.head;
	char teststr[ACM_PKGNAME_MAX_LEN] = {"com.acm.testacm50"};

	filp->private_data = inode -> i_private;

	printk("ACM:===Testing acm_ioctl()===\n");
	//Test ACM_ADD
	acm_hash_cleanup(acm_hash.head);
	printk("ACM:VAL acm_hash_empty=%d\n",acm_hash_empty(acm_hash.head));
	if (!acm_hash_empty(acm_hash.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -ENODATA;
	}
	temp_hash_node = (acm_hash_node_t *)kzalloc(sizeof(acm_hash_node_t),GFP_KERNEL);
	if (!temp_hash_node) {
		printk("Failed to execute test!\n");
		return -ENOMEM;
	}
	INIT_HLIST_NODE(&temp_hash_node->hnode);
	strcpy(temp_hash_node->pkgname,teststr);
	temp_hash_node->pkgname[ACM_PKGNAME_MAX_LEN - 1] = '\0';
	phead = &hash[ELFHash(temp_hash_node->pkgname)];
	mutex_lock(&acm_hash.mutex);
	hlist_add_head(&temp_hash_node->hnode, phead);
	phead = &hash[0];
	temp_hash_node = hlist_entry(phead->first, acm_hash_node_t, hnode);
	mutex_unlock(&acm_hash.mutex);
	if (strcmp(temp_hash_node->pkgname,teststr)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Test ACM_SEARCH
	temp_hash_node = acm_hash_search(acm_hash.head,teststr);
	if (!temp_hash_node) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}
	if (strcmp(temp_hash_node->pkgname,teststr)) {
		printk("ACM:%d Error!",__LINE__);
		return -EINVAL;
	}

	//Test ACM_DEL
	temp_hash_node = acm_hash_search(acm_hash.head,teststr);
	if(!temp_hash_node) {
		printk("ACM:%d Error!",__LINE__);
		return -ENODATA;
	}
	hlist_del(&temp_hash_node->hnode);
	kfree(temp_hash_node);
	printk("ACM:VAL acm_hash_empty=%d\n",acm_hash_empty(acm_hash.head));
	if (!acm_hash_empty(acm_hash.head)) {
		printk("ACM:%d Error!",__LINE__);
		return -ENODATA;
	}

	printk("ACM:===Testing acm_ioctl() PASSED!===\n");
	return ACM_SUCCESS;
}

int acm_enable = 0;
EXPORT_SYMBOL(acm_enable);
static int acm_enable_open(struct inode *inode, struct file *filp) {

	filp->private_data = inode->i_private;

	return 0;
}

static int acm_enable_read(struct file *filp, char __user *buff,
	size_t count, loff_t *offp)
{
	char kbuf[4] = {'\0'};

	snprintf(kbuf, sizeof(kbuf), "%d", acm_enable);
	printk("kbuf = %s acm_enable = %d\n", kbuf, acm_enable);
	if (copy_to_user(buff, kbuf, 4)) {
		return -EFAULT;
	}

	return 0;
}

static ssize_t acm_enable_write(struct file *filp, const char __user *buff,
	size_t count, loff_t *offp)
{
	int i = 0;
	char kbuf[4] = {'\0'};

	if (copy_from_user(kbuf, buff, 4)) {
		return -EFAULT;
	}
	if (!strncmp(kbuf, "1", 1)) {
		acm_enable = 1;
	} else {
		acm_enable = 0;
	}
	printk("acm_enable = %d\n", acm_enable);
	return count;
}

ssize_t acm_test_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
	return 0;
}

ssize_t acm_test_write(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
	return 0;
}

struct file_operations acm_hash_table_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_hash_table_open,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_fwk_list_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_fwk_list_open,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_dmd_list_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_dmd_list_open,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_should_add_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_should_add,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_dmd_add_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_acm_dmd_add,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_acm_dmd_upload_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_acm_dmd_upload,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_acm_fwk_upload_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_acm_fwk_upload,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_acm_hash_search_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_acm_hash_search,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_ioctl_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_ioctl,
	.read = acm_test_read,
	.write = acm_test_write,
};

struct file_operations acm_test_valid_len_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_test_valid_len,
	.read = acm_test_read,
	.write = acm_test_write,
};

static const struct file_operations acm_enable_file_ops = {
	.owner = THIS_MODULE,
	.open = acm_enable_open,
	.read = acm_enable_read,
	.write = acm_enable_write,
};

static int __init acm_debugfs_init(void) {
	static struct dentry *acm_debugfs_file_hash_table;
	static struct dentry *acm_debugfs_file_list_for_fwk;
	static struct dentry *acm_debugfs_file_list_for_dmd;
	static struct dentry *acm_debugfs_file_test_should_add;
	static struct dentry *acm_debugfs_file_test_dmd_add;
	static struct dentry *acm_debugfs_file_test_acm_fwk_upload;
	static struct dentry *acm_debugfs_file_test_acm_dmd_upload;
	static struct dentry *acm_debugfs_file_test_acm_hash_search;
	static struct dentry *acm_debugfs_file_test_ioctl;
	static struct dentry *acm_debugfs_file_test_valid_len;
	static struct dentry *acm_debugfs_file_enable;

	printk(KERN_EMERG "ACM:Initializing debugfs.\n");
	acm_debugfs_root = debugfs_create_dir("acm_debug",NULL);
	if(!acm_debugfs_root) {
		printk(KERN_EMERG "ACM:Failed to create acm_debug directory!\n");
		return -EINVAL;
	}

	acm_debugfs_file_hash_table = debugfs_create_file("acm_hash_table",0644,acm_debugfs_root,NULL,&acm_hash_table_file_ops);
	if(!acm_debugfs_file_hash_table) {
		printk(KERN_EMERG "ACM:Failed to create file acm_hash_table!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_list_for_fwk = debugfs_create_file("acm_fwk_list",0644,acm_debugfs_root,NULL,&acm_fwk_list_file_ops);
	if(!acm_debugfs_file_list_for_fwk) {
		printk(KERN_EMERG "ACM:Failed to create file acm_fwk_list!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_list_for_dmd = debugfs_create_file("acm_dmd_list",0644,acm_debugfs_root,NULL,&acm_dmd_list_file_ops);
	if(!acm_debugfs_file_list_for_dmd) {
		printk(KERN_EMERG "ACM:Failed to create file acm_dmd_list!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_should_add = debugfs_create_file("acm_test_should_add",0644,acm_debugfs_root,NULL,&acm_test_should_add_file_ops);
	if(!acm_debugfs_file_test_should_add) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_should_add!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_dmd_add = debugfs_create_file("acm_test_dmd_add",0644,acm_debugfs_root,NULL,&acm_test_dmd_add_file_ops);
	if(!acm_debugfs_file_test_dmd_add) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_dmd_add!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_acm_dmd_upload = debugfs_create_file("acm_test_acm_dmd_upload",0644,acm_debugfs_root,NULL,&acm_test_acm_dmd_upload_file_ops);
	if(!acm_debugfs_file_test_acm_dmd_upload) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_acm_dmd_upload!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_acm_fwk_upload = debugfs_create_file("acm_test_acm_fwk_upload",0644,acm_debugfs_root,NULL,&acm_test_acm_fwk_upload_file_ops);
	if(!acm_debugfs_file_test_acm_fwk_upload) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_acm_fwk_upload!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_acm_hash_search = debugfs_create_file("acm_test_acm_hash_search",0644,acm_debugfs_root,NULL,&acm_test_acm_hash_search_file_ops);
	if(!acm_debugfs_file_test_acm_hash_search) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_acm_hash_search!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_ioctl = debugfs_create_file("acm_test_ioctl",0644,acm_debugfs_root,NULL,&acm_test_ioctl_file_ops);
	if(!acm_debugfs_file_test_ioctl) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_ioctl!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_test_valid_len = debugfs_create_file("acm_test_valid_len",0644,acm_debugfs_root,NULL,&acm_test_valid_len_file_ops);
	if(!acm_debugfs_file_test_valid_len) {
		printk(KERN_EMERG "ACM:Failed to create file acm_test_valid_len!\n");
		goto free_debugfs;
	}

	acm_debugfs_file_enable = debugfs_create_file("acm_enable",
		0644, acm_debugfs_root, NULL, &acm_enable_file_ops);
	if (!acm_debugfs_file_enable) {
		printk("Failed to create file acm_enable!\n");
		goto free_debugfs;
	}

	return ACM_SUCCESS;

free_debugfs:
	debugfs_remove_recursive(acm_debugfs_root);
	return -EINVAL;
}

static void __exit acm_debugfs_exit(void) {
	debugfs_remove_recursive(acm_debugfs_root);
}

MODULE_LICENSE("GPL");
module_init(acm_debugfs_init);
module_exit(acm_debugfs_exit);
