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

#include <linux/slab.h>
#include <linux/list.h>
#include <linux/module.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("little penguin Task12");
MODULE_VERSION("12");
MODULE_LICENSE("GPL v2");

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

	list_add_tail(&new->list, &identities);
	return 0;
}

struct identity *identity_find(int id)
{
	struct identity *i;

	list_for_each_entry(i, &identities, list) {
		if (i->id == id)
			return i;
	}

	return NULL;
}

void identity_destroy(int id)
{
	struct identity *i, *tmp;

	list_for_each_entry_safe_reverse(i, tmp, &identities, list) {
		if (i->id == id) {
			list_del(&i->list);
			kfree(i);
			return;
		}
	}

	pr_err("Can not destroy id=%d, id not found!\n", id);
}

void identity_destroy_all(void)
{
	struct identity *i, *tmp;

	list_for_each_entry_safe(i, tmp, &identities, list) {
		list_del(&i->list);
		kfree(i);
	}
}

static int __init t12_init(void)
{
	struct identity *temp;
	int err;

	err = identity_create("Alice", 1);
	if (err)
		goto destroy;

	err = identity_create("Bob", 2);
	if (err)
		goto destroy;

	err = identity_create("Dave", 3);
	if (err)
		goto destroy;

	err = identity_create("Gena", 10);
	if (err)
		goto destroy;

	temp = identity_find(3);
	pr_debug("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_debug("id 42 not found\n");

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return 0;

destroy:
	identity_destroy_all();
	return err;
}

static void __exit t12_exit(void)
{
	identity_destroy_all();
}

module_init(t12_init);
module_exit(t12_exit);
