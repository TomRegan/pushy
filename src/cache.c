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

	i = 0;
	for (; i < CACHE_KEYS; ++i) {
		if (CACHE.keys [i] == NULL) {
			continue;
		}
		_rm_recurse(CACHE.keys [i]);
	}
	return 0; /* no error */
}

size_t
cache_size(void)
{
	return CACHE.size;
}
