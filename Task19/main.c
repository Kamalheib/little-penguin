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

#include <linux/ip.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <uapi/linux/netfilter_ipv4.h>

MODULE_AUTHOR("Kamal Heib <kamalheib1@gmail.com>");
MODULE_DESCRIPTION("Little Penguin Task19");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPLv2");

static struct ts_config *conf;
#define MY_ID "682c83e55b77"

unsigned int t19_nf_input(void *priv, struct sk_buff *skb,
			  const struct nf_hook_state *st)
{
	int ret;
	struct iphdr *iph;

	if (skb) {
		iph = ip_hdr(skb);
		if (iph->protocol == IPPROTO_TCP) {
			ret = skb_find_text(skb, 0, skb->len, conf);
			if (ret != UINT_MAX)
				pr_debug("Found%s\n", MY_ID);
		}
	}

	return NF_ACCEPT;
}

static const struct nf_hook_ops t19_nfops = {
	.hook = t19_nf_input,
	.hooknum	= NF_INET_PRE_ROUTING,
	.pf		= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};

static int __init t19_init(void)
{
	pr_info("t19: Hello World\n");

	conf = textsearch_prepare("kmp", MY_ID, strlen(MY_ID),
			GFP_KERNEL, TS_AUTOLOAD);
	if (IS_ERR(conf)) {
		pr_err("ts19: Failed to prepare text search\n");
		return 1;
	}

	nf_register_net_hook(&init_net, &t19_nfops);
	return 0;
}

static void __exit t19_exit(void)
{
	pr_info("t19: Good bye\n");
	textsearch_destroy(conf);
	nf_unregister_net_hook(&init_net, &t19_nfops);
}

module_init(t19_init);
module_exit(t19_exit);
