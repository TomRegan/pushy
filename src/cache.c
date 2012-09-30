#include "../include/cache.h"

uint16_t
_hash(char *buf, size_t len)
{
	uint16_t	cksum;
	size_t		i;

	cksum = 0;

	for (i = 0; i < len; ++i) {
		cksum += (uint8_t) buf [i];
	}

	return cksum % CACHE_KEYS;
}

void
_rm_recurse(struct record *r)
{
	if (r == NULL) {
		return;
	}
	if (r->l != NULL) {
		_rm_recurse(r->l);
		r->l = NULL;
	}
	if (r->r != NULL) {
		_rm_recurse(r->r);
		r->r = NULL;
	}
	if (r->l == NULL && r->r == NULL) {
		log_ln(MEM_DEBUG, "freeing %s:%s %p\n", r->key, r->value, r);
		free(r);
		CACHE.size--;
	} 
}

struct record*
_rm(char * key, struct record *this, struct record **relink_ref)
{
	int		cmp;
	struct record	*tmp = this;

	if (tmp == NULL) {
		return tmp;
	}

	/*
	 * The root node should return a reference to itself. The only time
	 * this function should return another reference is when it is
	 * relinking a subtree.
	 */

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
				(void) _rm(key, this->r, &this->l);
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
			this->l = _rm(key, this->l, NULL);
		} else {
			(void) _rm(key, this->l, NULL);
		}
	} else if (relink_ref == NULL && 0 < cmp) { /* node is in right subtree */
		if (this->r != NULL && 0 == strncmp(key, this->r->key, RECORD_KEY_LEN)) {
			/* the right node is for deletion */
			this->r = _rm(key, this->r, NULL);
		} else {
			(void) _rm(key, this->r, NULL);
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
			(void) _rm(key, this->l, relink_ref);
		}
	}

	return tmp;
}

int8_t
cache_init(void)
{
	CACHE.size = 0;
	cache_clear();
	return 0; /* no error */
}

int8_t 
cache_add(char* k, char* v)
{
	uint16_t	hash;

	hash = _hash(k, strlen(k));

	if (CACHE.keys [hash] != NULL) {
		/*
		 * Updating the key as well as the value...
		 * This covers a bug that I'm not focussing on, where
		 * an update may collide with a key (based on the hash)
		 * although the keys are unique. I don't really care to
		 * fix this at the moment, so this is some sensible
		 * behaviour for the meantime.
		 */
		strncpy(CACHE.keys [hash]->key, k, RECORD_KEY_LEN);
		strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
		return 1; /* value updated */
	}

	if ((CACHE.keys [hash] = (struct record*) malloc(
					sizeof(struct record))) == NULL) {
		return -1; /* malloc failed */
	}
	strncpy(CACHE.keys [hash]->key, k, RECORD_KEY_LEN);
	strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
	CACHE.keys [hash]->l = NULL;
	CACHE.keys [hash]->r = NULL;
	CACHE.size++;
	return 0; /* no error */
}

int8_t
cache_rtrv(char *key, char rtrv_buf [], size_t len)
{
	strncpy(rtrv_buf, "{\"request\":\"/SYSTEM\",\"version\":\"0.0.1.1\"}\r\n", len);
	return 0; /* no error */
}

int8_t
cache_rm(char *k)
{
	uint16_t	hash;

	hash = _hash(k, strlen(k));
	if (CACHE.keys [hash] != NULL) {
		_rm(k, CACHE.keys [hash], NULL);
		CACHE.size--;
	} else {
		return -1; /* not found */
	}
	return 0; /* no error */
}

int8_t
cache_clear(void)
{
	size_t		i;

	for (i = 0; i < CACHE_KEYS; ++i) {
		if (CACHE.keys [i] != NULL) {
			_rm_recurse(CACHE.keys [i]);
		}
	}
	return 0; /* no error */
}

size_t
cache_size(void)
{
	return CACHE.size;
}
