/*
 * Copyright (c) 2016 Kamal Heib.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 3, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/module.h>
#include <linux/debugfs.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("little penguin Task8");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

#define t8_dbg(fmt, arg...) \
	pr_dbg("Task08: " fmt, ## arg)
#define t8_err(fmt, arg...) \
	pr_err("Task08: " fmt, ## arg)
#define t8_info(fmt, arg...) \
	pr_info("Task08: " fmt, ## arg)

static int t8_id = 0xff;

struct t8_debugfs {
	struct dentry *root;
	struct dentry *id;
};

static struct t8_debugfs dbg;

static int t8_create_root_dir(void)
{
	t8_info("Create root debugfs dir\n");
	dbg.root = debugfs_create_dir("eudyptula", NULL);
	return dbg.root ? 0 : -ENOMEM;
}

static void t8_destroy_root_dir(void)
{
	t8_info("Destroy root debugfs dir\n");
	debugfs_remove(dbg.root);
}

static ssize_t t8_read_id(struct file *file,
			  char __user *ubuf,
			  size_t count,
			  loff_t *ppos)
{
	char buf[10];
	ssize_t len;

	len = snprintf(buf, sizeof(buf), "0x%x\n", t8_id);
	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t t8_write_id(struct file *file,
			   const char __user *ubuf,
			   size_t count,
			   loff_t *ppos)
{
	int err;
	char buf[80];

	err = simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, ubuf, count);
	if (err < 0)
		return err;

	return sscanf(buf, "%x\n", &t8_id);
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = t8_write_id,
	.read = t8_read_id,
};

static int t8_create_id_file(void)
{
	t8_info("Create id debugfs file\n");
	dbg.id = debugfs_create_file("id", S_IWUGO | S_IRUGO, dbg.root,
				     NULL, &id_fops);
	return dbg.id ? 0 : -ENOMEM;
}

static void t8_destroy_id_file(void)
{
	t8_info("Destroy id debugfs file\n");
	debugfs_remove(dbg.id);
}

static int t8_create_debugfs(void)
{
	int err;

	t8_info("Create debugfs files\n");
	err = t8_create_root_dir();
	if (err) {
		t8_err("Failed to create \"eudyptula\" debgfs dir\n");
		return err;
	}

	err = t8_create_id_file();
	if (err) {
		t8_err("Failed to create \"id\" debugfs file\n");
		goto free_root;
	}

	return 0;

free_root:
	t8_destroy_root_dir();

	return err;
}

static void t8_destroy_debugfs(void)
{
	t8_destroy_id_file();
	t8_destroy_root_dir();
}

static int __init t8_init(void)
{
	t8_info("Loading...\n");
	return t8_create_debugfs();
}

static void __exit t8_exit(void)
{
	t8_info("Unloading...\n");
	t8_destroy_debugfs();
}

module_init(t8_init);
module_exit(t8_exit);
