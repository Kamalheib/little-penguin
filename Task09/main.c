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
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("little penguin Task09");
MODULE_VERSION("9");
MODULE_LICENSE("GPL");

#define little_dbg(fmt, arg...) \
	pr_dbg("little: " fmt, ## arg)
#define little_err(fmt, arg...) \
	pr_err("little: " fmt, ## arg)
#define little_info(fmt, arg...) \
	pr_info("little: " fmt, ## arg)

static int little_id = 0xff;

static char little_foo_buf[PAGE_SIZE];
static spinlock_t foo_lock;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
	return sprintf(buf, "0x%x\n", little_id);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
		       const char *buf, size_t count)
{
	int ret, var;

	ret = kstrtoint(buf, 16, &var);
	if (ret < 0)
		return ret;

	little_id = var;

	return count;
}

static struct kobj_attribute id_attribute = __ATTR_RW(id);

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%llu\n", get_jiffies_64());
}

static struct kobj_attribute jiffies_attribute = __ATTR_RO(jiffies);

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	spin_lock_irq(&foo_lock);
	strncpy(buf, little_foo_buf, PAGE_SIZE - 1);
	spin_unlock_irq(&foo_lock);

	return PAGE_SIZE - 1;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	if (count > PAGE_SIZE)
		return -ENOSPC;

	spin_lock_irq(&foo_lock);
	strncpy(little_foo_buf, buf, PAGE_SIZE - 1);
	spin_unlock_irq(&foo_lock);

	return count;
}

static struct kobj_attribute foo_attribute = __ATTR_RW(foo);

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *root;

static int __init little_init(void)
{
	int err;

	little_info("Hello sysfs world\n");

	root = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!root)
		return -ENOMEM;

	err = sysfs_create_group(root, &attr_group);
	if (err)
		kobject_put(root);

	return err;
}

static void __exit little_exit(void)
{
	little_info("Good bye sysfs!\n");

	kobject_put(root);
}

module_init(little_init);
module_exit(little_exit);
