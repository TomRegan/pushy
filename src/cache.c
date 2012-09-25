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
	write_log(TEST_DEBUG, "visiting %s:%s %p\n", r->key, r->value, r);
	if (r->l != NULL) {
		_rm_recurse(r->l);
		r->l = NULL;
	}
	if (r->r != NULL) {
		_rm_recurse(r->r);
		r->r = NULL;
	}
	if (r->l == NULL && r->r == NULL) {
		write_log(TEST_DEBUG, "deallocing %s:%s %p\n", r->key, r->value, r);
		free(r);
		CACHE.size--;
	} 
}

struct record*
_rm(char * key, struct record *this, struct record **relink_ref)
{
	struct record	*tmp = NULL;

	if (this == NULL) {
		return NULL;
	}
	write_log(TEST_DEBUG, "_rm: visiting %s:%s %p\n", this->key, this->value, this);
	/* the left node is for deletion */
	if (this->l != NULL && 0 == strncmp(key, this->l->key, RECORD_KEY_LEN)) {
		write_log(TEST_DEBUG, "_rm: left node of %s is for deletion %s:%s %p\n", this->key, this->l->key, this->l->value, this->l);
		tmp = _rm(key, this->l, NULL);
		write_log(TEST_DEBUG, "_rm: freeing %s:%s %p\n", this->l->key, this->l->value, this->l);
		(void) free(this->l);
		this->l = tmp;
		write_log(TEST_DEBUG, "_rm: left node of %s is now %s:%s %p\n", this->key, this->l->key, this->l->value, this->l);
	}
	/* the right node is for deletion */
	if (this->r != NULL && 0 == strncmp(key, this->r->key, RECORD_KEY_LEN)) {
		write_log(TEST_DEBUG, "_rm: right node of %s is for deletion %s:%s %p\n", this->key, this->r->key, this->r->value, this->r);
		tmp = _rm(key, this->r, NULL);
		write_log(TEST_DEBUG, "_rm: freeing %s:%s %p\n", this->r->key, this->r->value, this->r);
		(void) free(this->r);
		this->r = tmp;
		write_log(TEST_DEBUG, "_rm: right node of %s is now %s:%s %p\n", this->key, this->r->key, this->r->value, this->r);
	}
	/* this is the node for deletion */
	if (0 == strncmp(key, this->key, RECORD_ENTRY_LEN)) {
		/* terminal node, delete */
		if (this->l == NULL && this->r == NULL) {
			write_log(TEST_DEBUG, "_rm: freeing %s:%s %p\n", this->key, this->value, this);
			(void) free(this);
			return NULL;
		}

		if (this->r != NULL) {
			/* relink the left node */
			if (this->l != NULL) {
				/*
				 * If the left link of the node to be deleted
				 * is not null, the left link of the left
				 * terminal node of the new right subtree will
				 * need to link to its node.
				 */
				write_log(TEST_DEBUG, "_rm: relinking from %s:%s %p\n", this->key, this->value, this);
				(void) _rm(key, this->r, &this->l);
			}
			return this->r;
		} else {
			write_log(TEST_DEBUG, "_rm: relinking: returning left node %s:%s %p\n", this->l->key, this->l->value, this->l);
			return this->l;
		}
		return NULL;
	}
	/* relink the node */
	if (relink_ref != NULL) {
		write_log(TEST_DEBUG, "_rm: thinking about relinking in %s:%s %p\n", this->key, this->value, this);
		if (this->l == NULL) {
			write_log(TEST_DEBUG, "_rm: setting left node of %s to %s\n", this->key, (*relink_ref)->key);
			this->l = *relink_ref;
		} else {
			write_log(TEST_DEBUG, "_rm: won't relink %s to %s\n", this->key, (*relink_ref)->key);
			(void) _rm(key, this->l, relink_ref);
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
		write_log(TEST_DEBUG, "updating %s with value %s\n", k, v);
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
	write_log(TEST_DEBUG, "creating record %s with value %s\n", k, v);
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
