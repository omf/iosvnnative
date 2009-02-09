
#include "svn_string.h"
#include "aprhash_iomap.h"

IoMap* IoMap_newWithAprhash_(void *state, apr_hash_t* ht, apr_pool_t* pool)
{
    const void *key;
    int key_len;
    apr_hash_index_t *hi;
    svn_string_t *prop;

    IoObject* self = IoMap_new(state);
    for (hi = apr_hash_first(pool, ht); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, &key, &key_len, (void*)&prop);
        //IoMap_rawAtPut(map, IOSEQ((char *)key, (size_t)key_len), IOSEQ(prop->data, (size_t)prop->len));
        IoMap_rawAtPut(self, IOSYMBOL((char *)key), IOSYMBOL(prop->data));
    }
    return self;
}
