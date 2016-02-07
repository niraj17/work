#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Niraj Kumar - niraj17@gmail.com");

static int nextfs_fill_sb(struct super_block *sb, void *data, int silent)
{
	/*
	struct aufs_super_block *asb = aufs_super_block_read(sb);
	struct inode *root;

	if (!asb)
		return -EINVAL;

	sb->s_magic = asb->asb_magic;
	sb->s_fs_info = asb;
	sb->s_op = &aufs_super_ops;

	if (sb_set_blocksize(sb, asb->asb_block_size) == 0) {
		pr_err("device does not support block size %lu\n",
			(unsigned long)asb->asb_block_size);
		return -EINVAL;
	}

	root = aufs_inode_get(sb, asb->asb_root_inode);
	if (IS_ERR(root))
		return PTR_ERR(root);

	sb->s_root = d_make_root(root);
	if (!sb->s_root) {
		pr_err("aufs cannot create root\n");
		return -ENOMEM;
	}
	*/

	return 0;
}

static struct dentry *nextfs_mount(struct file_system_type *type, int flags,
			char const *dev, void *data)
{
	struct dentry *entry = mount_bdev(type, flags, dev, data, nextfs_fill_sb);

	if (IS_ERR(entry))
		pr_err("nextfs  mounting failed\n");
	else
		pr_debug("nextfs mounted\n");
	return entry;
}

static struct file_system_type nextfs_type = {
	.owner = THIS_MODULE,
	.name = "NextFS",
	.mount = nextfs_mount,
	.kill_sb = kill_block_super,
	.fs_flags = FS_REQUIRES_DEV
};


static int nextfs_init(void)
{
	int ret = 0;

	printk(KERN_ALERT "Hello, NextFS\n");

	ret = register_filesystem(&nextfs_type);
	if (ret != 0) {
		pr_err("cannot register filesystem\n");
		return ret;
	}	
	return 0;
}
static void nextfs_exit(void)
{
	int ret = 0;

	printk(KERN_ALERT "Goodbye, NextFS\n");
	ret = unregister_filesystem(&nextfs_type);
}
module_init(nextfs_init);
module_exit(nextfs_exit);
