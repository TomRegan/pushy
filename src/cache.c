/**
 * Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

/**
 * @file  cache.c
 * @brief  in-memory data cache
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#include "../include/cache.h"

uint16_t
_cksum(char *buf, size_t len)
{
	uint16_t	cksum;
	size_t		i;

	cksum = 0;

	for (i = 0; i < len; ++i) {
		cksum += (uint8_t) buf [i];
	}

	return cksum % CACHE_KEYS;
}

uint8_t
_tree_rtrv_recurse(struct record *r, char *key, char rtrv_buf [], size_t len)
{
    int cmp;

    if (r == NULL) {
        return -1;
    }

    cmp = strncmp(r->key, key, len);
    if (cmp == 0) {
        strncpy(rtrv_buf, r->value, len);
        return 0;
    } else if (cmp < 0) {
        return _tree_rtrv_recurse(r->l, key, rtrv_buf, len);
    } else {
        return _tree_rtrv_recurse(r->r, key, rtrv_buf, len);
    }
    return -1;
}

void
_tree_rm_resurse(struct record *r)
{
	if (r == NULL) {
		return;
	}
	if (r->l != NULL) {
		_tree_rm_resurse(r->l);
		r->l = NULL;
	}
	if (r->r != NULL) {
		_tree_rm_resurse(r->r);
		r->r = NULL;
	}
	if (r->l == NULL && r->r == NULL) {
		log_ln(MEM_DEBUG, "freeing %s:%s %p\n", r->key, r->value, r);
		free(r);
		CACHE.size--;
	} 
}

struct record*
_tree_rm(char *key, struct record *this, struct record **relink_ref)
{
	int		cmp;
	struct record	*tmp = this;

	if (tmp == NULL) {
		return tmp;
	}

	cmp = strncmp(key, this->key, RECORD_ENTRY_LEN);

	if (0 == cmp) { /* this is the node for deletion */
		if (this->r != NULL) {
			if (this->l != NULL) {
				/*
				 * If the left link of the node to be deleted
				 * is not null, the left link of the left
				 * terminal node of the new right subtree will
				 * need to link to its node.
				 */
				(void) _tree_rm(key, this->r, &this->l);
				/*
				 * At this point the terminal left node of the
				 * right subtree is connceted to the left sub-
				 * tree of this node.
				 */
				this->l = NULL;
			}
			tmp = this->r;
			this->r = NULL;
		} else {
			tmp = this->l;
			this->l = NULL;
		}

		/* terminal node, delete */
		if (this->l == NULL && this->r == NULL) {
			log_ln(MEM_DEBUG, "freeing %s:%s %p\n", this->key, this->value, this);
			free(this);
		}

	} else if (relink_ref == NULL && 0 > cmp) { /* node is in left subtree */
		if (this->l != NULL && 0 == strncmp(key, this->l->key, RECORD_KEY_LEN)) {
			/* the left node is for deletion */
			this->l = _tree_rm(key, this->l, NULL);
		} else {
			(void) _tree_rm(key, this->l, NULL);
		}
	} else if (relink_ref == NULL && 0 < cmp) { /* node is in right subtree */
		if (this->r != NULL && 0 == strncmp(key, this->r->key, RECORD_KEY_LEN)) {
			/* the right node is for deletion */
			this->r = _tree_rm(key, this->r, NULL);
		} else {
			(void) _tree_rm(key, this->r, NULL);
		}
	}

	/* relink the node */
	if (relink_ref != NULL) {
		if (this->l == NULL) {
			/*
			 * This is the terminal node of some left subtree,
			 * and it should be linked to the old left subtree of
			 * the node to be deleted.
			 */
			this->l = *relink_ref;
		} else {
			(void) _tree_rm(key, this->l, relink_ref);
		}
	}

	return tmp;
}

int8_t
cache_init(void)
{
	sem_unlink("/pushy");
	if (SEM_FAILED == (CACHE_LOCK = sem_open("/pushy", O_CREAT, 0660, 1))) {
		printf("error opening semphore: %i\n", errno);
		return -1; /* error creating semaphor */
	}

    cache_clear();
    CACHE.size = 0;


	return 0; /* no error */
}

int8_t 
cache_add(char* k, char* v)
{
    uint16_t    hash;

    hash = _cksum(k, strlen(k));

    if (CACHE_LOCK == NULL) {
        return -1; /* concurrency error */
    }

	if (CACHE.keys [hash] != NULL) {
		/*
		 * Updating the key as well as the value...
		 * This covers a bug that I'm not focussing on, where
		 * an update may collide with a key (based on the hash)
		 * although the keys are unique. I don't really care to
		 * fix this at the moment, so this is some sensible
		 * behaviour for the meantime.
		 */
		sem_wait(CACHE_LOCK);
		strncpy(CACHE.keys [hash]->key, k, RECORD_KEY_LEN);
		strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
		sem_post(CACHE_LOCK);
		return 1; /* value updated */
	}

	if ((CACHE.keys [hash] = (struct record*) malloc(
					sizeof(struct record))) == NULL) {
		return -1; /* malloc failed */
	}
	sem_wait(CACHE_LOCK);
	strncpy(CACHE.keys [hash]->key, k, RECORD_KEY_LEN);
	strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
	CACHE.keys [hash]->l = NULL;
	CACHE.keys [hash]->r = NULL;
	CACHE.size++;
	sem_post(CACHE_LOCK);
	return 0; /* no error */
}

int8_t
cache_rtrv(char *key, char rtrv_buf [], size_t len)
{
    uint16_t        cksum;
    uint8_t         ret_val;

    cksum = _cksum(key, strlen(key));

    if (CACHE_LOCK == NULL) {
        return -1; /* concurrency error */
    }
    sem_wait(CACHE_LOCK);

    ret_val = _tree_rtrv_recurse(CACHE.keys [cksum], key, rtrv_buf, len);
    sem_post(CACHE_LOCK);

    return ret_val;
}

int8_t
cache_rm(char *k)
{
	uint16_t	cksum;

	cksum = _cksum(k, strlen(k));

	if (CACHE_LOCK == NULL) {
		return -1; /* concurrency error */
	}
	sem_wait(CACHE_LOCK);

	if (CACHE.keys [cksum] != NULL) {
		_tree_rm(k, CACHE.keys [cksum], NULL);
		CACHE.size--;
	} else {
		sem_post(CACHE_LOCK);
		return -1; /* not found */
	}
	sem_post(CACHE_LOCK);
	return 0; /* no error */
}

int8_t
cache_clear(void)
{
	size_t		i;

	if (CACHE_LOCK == NULL) {
		return -1; /* concurrency error */
	}
	sem_wait(CACHE_LOCK);

	for (i = 0; i < CACHE_KEYS; ++i) {
		if (CACHE.keys [i] != NULL) {
			_tree_rm_resurse(CACHE.keys [i]);
		}
	}

	sem_post(CACHE_LOCK);

	return 0; /* no error */
}

size_t
cache_size(void)
{
	return CACHE.size;
}
