//metadoc copyright Your Name Here 2008

#ifndef aprhash_iomap_DEFINED
#define aprhash_iomap_DEFINED 1

#include "IoMap.h"
#include "apr_hash.h"

IoMap* IoMap_newWithAprhash_(void *state, apr_hash_t* ht, apr_pool_t* pool);

#endif

