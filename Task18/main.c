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
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("Little Penguin Task17");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPLv2");

#define TIME_OUT	msecs_to_jiffies(5000)

static unsigned int id = 0;
static wait_queue_head_t wee_wait;
static struct task_struct *thread;
static spinlock_t lock;

struct identity {
	char name[20];
	int id;
	bool busy;
	struct list_head list;
};

static LIST_HEAD(identities);

static int identity_create(char *name, int id)
{
	struct identity *new;

	new = kzalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	strcpy(new->name, name);
	new->id = id;

	spin_lock_irq(&lock);
	list_add_tail(&new->list, &identities);
	spin_unlock_irq(&lock);
	return 0;
}

static struct identity *identity_find(int id)
{
	struct identity *i;
	bool found = false;

	spin_lock_irq(&lock);
	list_for_each_entry(i, &identities, list) {
		if (i->id == id) {
			found = true;
			break;
		}
	}
	spin_unlock_irq(&lock);

	return found ? i : NULL;
}

static struct identity *identity_get(void)
{
	struct identity *i;

	if (list_empty(&identities))
		return NULL;

	spin_lock_irq(&lock);
	i = list_entry(identities.next, struct identity, list);
	spin_unlock_irq(&lock);
	list_del(&i->list);
	return i;
}

static void identity_destroy(int id)
{
	struct identity *i, *tmp;

	spin_lock_irq(&lock);
	list_for_each_entry_safe_reverse(i, tmp, &identities, list) {
		if (i->id == id) {
			list_del(&i->list);
			kfree(i);
			break;
		}
	}
	spin_unlock_irq(&lock);
}

static void identity_destroy_all(void)
{
	struct identity *i, *tmp;

	spin_lock_irq(&lock);
	list_for_each_entry_safe(i, tmp, &identities, list) {
		list_del(&i->list);
		kfree(i);
	}
	spin_unlock_irq(&lock);
}

int thread_handler(void *data)
{
	struct identity *i;

	while(!kthread_should_stop()) {
		wait_event_interruptible_timeout(wee_wait, 1, TIME_OUT);
		i = identity_get();
		msleep_interruptible(5000);
		if (i) {
			pr_info("Task18: name=%s id=%d\n", i->name, i->id);
			kfree(i);
		}
	}

	return 0;
}

static ssize_t t18_write(struct file *file,
			const char __user *ubuf,
			size_t count,
			loff_t *ppos)
{
	char buf[80];
	int err;

	if (count > 19)
		count = 19;

	err = simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, ubuf, count - 1);
	if (err < 0)
		return err;

	buf[count - 1] = '\0';
	err = identity_create(buf, id++);
	if (err)
	       return err;

	wake_up_process(thread);
	return count;

}

static const struct file_operations t18_chardev_ops = {
	.write	= t18_write,
};

static struct miscdevice t18_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.mode = S_IWUGO | S_IRUGO,
	.fops = &t18_chardev_ops,
};

static int __init t18_init(void)
{
	int err;

	init_waitqueue_head(&wee_wait);

	thread = kthread_create(thread_handler, NULL, "eudyptula");
	if (IS_ERR(thread)) {
		pr_err("Task18: Failed to create eudyptula thread!\n");
		return 1;
	}

	err = misc_register(&t18_dev);
	if (err) {
		pr_err("Task18: Failed to register misc device err(%d)\n",
		       err);
		kthread_stop(thread);
		return 1;
	}

	return 0;
}

static void __exit t18_exit(void)
{
	if (thread)
		kthread_stop(thread);
	misc_deregister(&t18_dev);
	identity_destroy_all();
}

module_init(t18_init);
module_exit(t18_exit);
