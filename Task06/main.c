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

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("Little Penguin Task06");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL v3");

static ssize_t t6_read(struct file *file,
		       char __user *ubuf,
		       size_t count,
		       loff_t *ppos)
{
	char buf[10];
	ssize_t len;

	len = snprintf(buf, sizeof(buf), "%d\n", MISC_DYNAMIC_MINOR);
	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t t6_write(struct file *file,
			const char __user *ubuf,
			size_t count,
			loff_t *ppos)
{
	int num;
	int err;
	char buf[80];

	err = simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, ubuf, count);
	if (err < 0)
		return err;

	sscanf(buf, "%x\n", &num);

	return num == MISC_DYNAMIC_MINOR ? count : -EINVAL;

}

static const struct file_operations t6_chardev_ops = {
	.read	= t6_read,
	.write	= t6_write,
};

static struct miscdevice t6_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &t6_chardev_ops,
};

static int __init t6_init(void)
{
	int err = 0;

	pr_info("Task06: Hello World!");
	err = misc_register(&t6_dev);
	if (err)
		pr_err("Task06: Failed to register t6 misc device err(%d)\n",
		       err);

	return err;
}

static void __exit t6_exit(void)
{
	pr_info("Task06: Good bye!");
	misc_deregister(&t6_dev);
}

module_init(t6_init);
module_exit(t6_exit);
