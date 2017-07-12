/*
 * Copyright (c) 2017 Kamal Heib.  All rights reserved.
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
#include <linux/wait.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("Little Penguin Task17");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL v3");

#define TIME_OUT	msecs_to_jiffies(10000)

static wait_queue_head_t wee_wait;
static struct task_struct *thread;

int thread_handler(void *data)
{
	while(!kthread_should_stop()) {
		wait_event_interruptible_timeout(wee_wait, 0, TIME_OUT);
	}

	return 0;
}

static ssize_t t17_write(struct file *file,
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

static const struct file_operations t17_chardev_ops = {
	.write	= t17_write,
};

static struct miscdevice t17_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.mode = S_IWUGO | S_IRUGO,
	.fops = &t17_chardev_ops,
};

static int __init t17_init(void)
{
	int err;

	init_waitqueue_head(&wee_wait);

	thread = kthread_create(thread_handler, NULL, "eudyptula");
	if (IS_ERR(thread)) {
		pr_err("Task17: Failed to create eudyptula thread!\n");
		return 1;
	}

	err = misc_register(&t17_dev);
	if (err) {
		pr_err("Task17: Failed to register misc device err(%d)\n",
		       err);
		kthread_stop(thread);
		return 1;
	}

	wake_up_process(thread);

	return 0;
}

static void __exit t17_exit(void)
{
	if (thread)
		kthread_stop(thread);
	misc_deregister(&t17_dev);
}

module_init(t17_init);
module_exit(t17_exit);
