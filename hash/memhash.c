/*
 * Copyright 2003,2006,2011-2012,2015-2016 BitMover, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hash.h"
#include "memhash.h"
#include "utils/crc32c.h"

typedef struct node node;
typedef	struct memhash memhash;
struct memhash {
	hash	hdr;		/* std header for hash_* wrappers */
	int	nodes;		/* number of elements in hash */
	node	**arr;		/* array indexed by hash */
	u32	mask;		/* size of array */

	/* for nextkey .. */
	int	lastidx;
	node	*lastnode;

};

struct node {
	node	*next;		/* next element in same hash bucket */
	u32	klen;		/* key len */
	u32	dlen;		/* data len */
	char	key[0];		/* key and data stored here */
};

node	*nodes;

#define	HASH(buf, len) crc32c(0, buf, len)

/*
 * Find the offset of the data in the key array given the klen and
 * dlen.  If the data is big enough that it _might_ need to be
 * aligned, we align it to the smallest boundry we can.
 */
private inline int
DOFF(int klen, int dlen)
{
	int	mask;

	if ((sizeof(void*) == 8) && (dlen >= 8)) {
		mask = 8-1;
	} else if (dlen >= 4) {
		mask = 4-1;
	} else if (dlen >= 2) {
		mask = 2-1;
	} else {
		/* no alignment */
		return (klen);
	}
	return ((klen + mask) & ~mask);
}

private	void	memhash_split(memhash *h);

/*
 * Create a new hash structure.
 */
hash *
memhash_new(va_list ap)
{
	memhash	*ret;

	ret = new(memhash);
	ret->nodes = 0;
	ret->mask = 63;
	ret->arr = calloc(ret->mask + 1, sizeof(*ret->arr));
	return ((hash *)ret);
}

/*
 * Free hash
 */
int
memhash_free(hash *_h)
{
	memhash	*h = (memhash *)_h;
	int	i;
	node	*n, *t;

	for (i = 0; i <= h->mask; i++) {
		n = h->arr[i];
		while (n) {
			t = n;
			n = n->next;
			free(t);
		}
	}
	free(h->arr);
	return (0);
}

/*
 * Find a node in the hash and return a pointer to the pointer that
 * points at that node.  Or return a pointer to where the node should
 * be added to the hash.
 */
private inline node **
find_nodep(memhash *h, void *kptr, int klen)
{
	u32	hash = HASH(kptr, klen);
	node	*n, **nn;

	nn = &h->arr[hash & h->mask];
	while ((n = *nn) &&
	    !(klen == n->klen && !memcmp(n->key, kptr, klen))) {
		nn = &(n->next);
	}
	if (n) {
		h->hdr.kptr = n->key;
		h->hdr.klen = klen;
		h->hdr.vptr = n->key + DOFF(klen, n->dlen);
		h->hdr.vlen = n->dlen;
	}
	return (nn);
}

/*
 * find key in hash and return a pointer to the data.
 * Returns NULL if not found.
 * sets h->kv.val.{ptr,len}
 */
void *
memhash_fetch(hash *_h, void *kptr, int klen)
{
	memhash	*h = (memhash *)_h;
	node	*n, **nn;

	nn = find_nodep(h, kptr, klen);
	unless (n = *nn) {
		h->hdr.kptr = h->hdr.vptr = 0;
		h->hdr.klen = h->hdr.vlen = 0;
		errno = EINVAL;
	}
	return (h->hdr.vptr);
}

/*
 * Allocate a new code and add it to the chain at nn
 */
private inline void *
new_node(memhash *h, node **nn, void *kptr, int klen, int dlen)
{
	node	*n;
	int	doff;

	doff = DOFF(klen, dlen);
	n = malloc(sizeof(node) + doff + dlen);
	h->hdr.kptr = memcpy(n->key, kptr, klen);
	h->hdr.klen = n->klen = klen;
	h->hdr.vptr = n->key + doff;
	h->hdr.vlen = n->dlen = dlen;
	n->next = *nn;
	*nn = n;
	if (++h->nodes >= h->mask) memhash_split(h);
	return (h->hdr.vptr);
}

/*
 * Allocate a new hash entry with size for a 'dlen' data entry
 * and return a pointer to it.  Return NULL if that key already
 * exists.  dlen==0 is perfectly valid, only the existance of the
 * key is stored.
 */
void *
memhash_insert(hash *_h, void *kptr, int klen, void *dptr, int dlen)
{
	memhash	*h = (memhash *)_h;
	node	**nn;
	void	*ret;

	nn = find_nodep(h, kptr, klen);
	if (*nn) {
		/* ret 0, but h->kptr points at existing data */
		errno = EEXIST;
		return (0);
	}
	ret = new_node(h, nn, kptr, klen, dlen);
	if (dlen) {
		if (dptr) {
			memcpy(ret, dptr, dlen);
		} else {
			memset(ret, 0, dlen);
		}
	}
	return (ret);
}


void *
memhash_store(hash *_h, void *kptr, int klen, void *dptr, int dlen)
{
	memhash	*h = (memhash *)_h;
	node	*n, **nn, *nfree = 0;
	void	*ret = 0;

	nn = find_nodep(h, kptr, klen);
	if ((n = *nn) != 0) {
		if (dlen > n->dlen) {
			*nn = n->next;
			--h->nodes;
			nfree = n;
		} else {
			n->dlen = dlen;
			ret = &n->key[DOFF(klen, dlen)];
		}
	}
	unless (ret) ret = new_node(h, nn, kptr, klen, dlen);
	if (dlen) {
		if (dptr) {
			memcpy(ret, dptr, dlen);
		} else {
			memset(ret, 0, dlen);
		}
	}
	if (nfree) free(nfree);
	return (ret);
}

/*
 * Delete hash entry.
 */
int
memhash_delete(hash *_h, void *kptr, int klen)
{
	memhash	*h = (memhash *)_h;
	node	*n, **nn;

	nn = find_nodep(h, kptr, klen);
	if ((n = *nn) != 0) {
		*nn = n->next;
		free(n);
		assert(h->nodes > 0);
		--h->nodes;
		return (0);
	} else {
		errno = ENOENT;
		return (-1);
	}
}

void *
memhash_first(hash *_h)
{
	memhash	*h = (memhash *)_h;

	h->lastidx = -1;
	h->lastnode = 0;
	return (memhash_next(_h));
}

void *
memhash_next(hash *_h)
{
	memhash	*h = (memhash *)_h;
	node	*n;
	int	i;

	n = h->lastnode;
	if (n) n = n->next;
	unless (n) {
		i = h->lastidx + 1;
		while ((i <= h->mask) && !h->arr[i]) i++;
		if (i <= h->mask) {
			h->lastidx = i;
			n = h->arr[i];
		}
	}
	if ((h->lastnode = n) != 0) {
		h->hdr.kptr = n->key;
		h->hdr.klen = n->klen;
		h->hdr.vptr = &n->key[DOFF(n->klen, n->dlen)];
		h->hdr.vlen = n->dlen;
	} else {
		h->hdr.kptr = 0;
		h->hdr.klen = 0;
		h->hdr.vptr = 0;
		h->hdr.vlen = 0;
	}
	return (h->hdr.kptr);

}

/* double the size of a hash array because it has grown too large */
private void
memhash_split(memhash *h)
{
	int	newmask = ((h->mask + 1) << 1) - 1;
	int	i;
	node	*n;
	node	*t;
	node	**p;
	node	**newarr;
	u32	hash;

	newarr = calloc(newmask+1, sizeof(*newarr));
	for (i = 0; i <= h->mask; i++) {
		n = h->arr[i];
		while (n) {
			t = n;
			n = n->next;
			hash = HASH((u8 *)t->key, t->klen);
			p = &newarr[hash & newmask];
			t->next = *p;
			*p = t;
		}
	}
	free(h->arr);
	h->arr = newarr;
	h->mask = newmask;
}

int
memhash_count(hash *_h)
{
	memhash	*h = (memhash *)_h;

	return (h->nodes);
}
