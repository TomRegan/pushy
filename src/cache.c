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

int8_t 
cache_add(char* k, char* v)
{
	uint16_t	hash;

	hash = _hash(k, strlen(k));

	if (CACHE.keys [hash] != NULL) {
		strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
		return 1; /* value updated */
	}

	if ((CACHE.keys [hash] = (struct record*) malloc(
					sizeof(struct record))) == NULL) {
		return -1; /* malloc failed */
	}
	CACHE.keys [hash]->size = strnlen(v, RECORD_ENTRY_LEN);
	strncpy(CACHE.keys [hash]->key, k, RECORD_KEY_LEN);
	strncpy(CACHE.keys [hash]->value, v, RECORD_ENTRY_LEN);
	CACHE.keys [hash]->l = NULL;
	CACHE.keys [hash]->r = NULL;
	return 0; /* no error */
}
