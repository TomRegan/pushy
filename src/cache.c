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

	/*
	 * This will always return a value which is no larger then
	 * the cardinality of the cache.
	 */
	return cksum % CACHE_KEYS;
}

void
_rm_recurse(struct record *r)
{
	if (r == NULL) {
		return;
	}
	write_log(DEBUG, "visiting %s:%s %p\n", r->key, r->value, r);
	if (r->l != NULL) {
		_rm_recurse(r->l);
		r->l = NULL;
	}
	if (r->r != NULL) {
		_rm_recurse(r->r);
		r->r = NULL;
	}
	if (r->l == NULL && r->r == NULL) {
		write_log(DEBUG, "deallocing %s:%s %p\n", r->key, r->value, r);
		free(r);
		CACHE.size--;
	} 
}

struct record**
_rm(char * key, struct record *this, struct record **relink_ref)
{
	struct record	**tmp = NULL;

	if (this == NULL) {
		return NULL;
	}
	write_log(DEBUG, "visiting %s:%s %p\n", this->key, this->value, this);
	/* the left node is for deletion */
	if (this->l != NULL && 0 == strncmp(key, this->l->key, RECORD_KEY_LEN)) {
		tmp = _rm(key, this->l, NULL);
		(void) free(this->l);
		this->l = *tmp;
	}
	/* the right node is for deletion */
	if (this->r != NULL && 0 == strncmp(key, this->r->key, RECORD_KEY_LEN)) {
		tmp = _rm(key, this->r, NULL);
		(void) free(this->r);
		this->r = *tmp;
	}
	/* this is the node for deletion */
	if (0 == strncmp(key, this->key, RECORD_ENTRY_LEN)) {
		/* terminal node, delete */
		if (this->l == NULL && this->r == NULL) {
			return NULL;
		}
		/*
		 * So I don't have to think about this again: &(this->l) is
		 * intended to pass a reference to the left link pointer.
		 * This will be reassigned in a later frame to point at the
		 * new child node.
		 */
		if (this->r != NULL) {
			/* relink the left node */
			(void) _rm(key, this->r, &(this->l));
			return &(this->r);
		} else {
			return &(this->l);
		}
	}
	/* relink the node */
	if (relink_ref != NULL) {
		if (this->l == NULL) {
			this->l = *relink_ref;
		} else {
			(void) _rm(key, this->l, &(this->l));
		}
	}
	return NULL;
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
		write_log(DEBUG, "updating %s with value %s\n", k, v);
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
	write_log(DEBUG, "creating record %s with value %s\n", k, v);
	strncpy(CACHE.keys [hash]->key, k, RECORD_KEY_LEN);
	strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
	CACHE.keys [hash]->l = NULL;
	CACHE.keys [hash]->r = NULL;
	CACHE.size++;
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
