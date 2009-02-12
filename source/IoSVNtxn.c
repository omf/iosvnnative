#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoSVNtxn.h"
#include "SVN.h"
#include "aprhash_iomap.h"
#include "svn_fs.h"
#include "svn_repos.h"

#define DATA(self) ((IoSVNtxnData *)IoObject_dataPointer(self))

IoTag *IoSVNtxn_newTag(void *state)
{
    printf("IoSVNtxn_newTag\n");

	IoTag *tag = IoTag_newWithName_("SVNtxn");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSVNtxn_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSVNtxn_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSVNtxn_rawClone);
	return tag;
}

IoObject *IoSVNtxn_proto(void *state)
{
    printf("IoSVNtxn_proto\n");

    IoSVNtxnObject *self = IoObject_new(state);
    IoObject_tag_(self, IoSVNtxn_newTag(state));
    IoState_registerProtoWithFunc_(state, self, IoSVNtxn_proto);
    {
        IoMethodTable methodTable[] = {
            {"printHandle", IoSVNtxn_printHandle},
            {"propList", IoSVNtxn_propList},
            {"changeProp", IoSVNtxn_changeProp},
            {NULL, NULL},
        };
        IoObject_addMethodTable_(self, methodTable);
    }

    return self;
}

IoObject *IoSVNtxn_rawClone(IoSVNtxnObject *proto)
{
    printf("IoSVNtxn_rawClone\n");

    IoObject *self = IoObject_rawClonePrimitive(proto);
    // This is where any object-specific data would be copied
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoSVNtxnData)));
    DATA(self)->handle = NULL;
    DATA(self)->pool = SVN_createSubpool();
    printf("IoSVNtxn_proto->calloc(%p)\n", DATA(self));
    return self;

    //PARA QUE SIEMPRE SEA EL MISMO PROTO
    //return IoState_protoWithInitFunction_(IOSTATE, IoSVNtxn_proto);
}

IoObject *IoSVNtxn_new(void *state)
{
    printf("IoSVNtxn_new\n");
    IoObject* proto = IoState_protoWithInitFunction_(state, IoSVNtxn_proto);
    return IOCLONE(proto);

    //PARA QUE SIEMPRE SEA EL MISMO PROTO
    //return IoState_protoWithInitFunction_(state, IoSVNtxn_proto);
}

IoObject *IoSVNtxn_mark(IoSVNtxnObject* self)
{
    printf("IoSVNtxn_mark\n");
    return self;
}

void IoSVNtxn_free(IoSVNtxnObject *self)
{
    // free dynamically allocated data and do any cleanup
    printf("IoSVNtxn_free\n");

    if(DATA(self)) {
        printf("IoSVNtxn_free->free(%p)\n", IoObject_dataPointer(self));
        if(DATA(self)->pool) SVN_destroySubpool(DATA(self)->pool);
        free(IoObject_dataPointer(self));
    }
}

//--------------------------------------------------------------------------------------

IoObject *IoSVNtxn_newWithHandle_(void *state, svn_fs_txn_t* handle)
{
    printf("IoSVNtxn_newWithHandle_ handle %p\n", handle);
    IoSVNtxnObject *new = IoSVNtxn_new(state);
    DATA(new)->handle = handle;
    printf("IoSVNtxn_newWithHandle new %p\n", new);
    return new;
}

IoObject *IoSVNtxn_newWithHandle_pool_(void *state, svn_fs_txn_t* handle, apr_pool_t* pool)
{
    IoSVNtxnObject* new;
    new = IoSVNtxn_newWithHandle_(state, handle);
    SVN_destroySubpool(DATA(new)->pool);
    DATA(new)->pool = pool;
    printf("IoSVNtxn_newWithHandle_pool_ new %p, pool %p\n", new, pool);
    return new;
}

IoObject *IoSVNtxn_setHandle_(IoSVNtxnObject* self, svn_fs_txn_t* handle)
{
    printf("IoSVNtxn_setHandle_ handle %p\n", handle);
    DATA(self)->handle = handle;
    return self;
}

void IoSVNtxn_checkHandle(IoSVNtxnObject* self)
{
    //if there is a handle there is a pool
    if(DATA(self)->handle) {
        //destroy previous handle
        SVN_clearSubpool(DATA(self)->pool);
        //DATA(self)->pool = SVN_createSubpool();
        DATA(self)->handle = NULL;
    }
}


//--------------------------------------------------------------------------------------

IoObject *IoSVNtxn_printHandle(IoSVNtxnObject *self, IoObject *locals, IoMessage *m)
{
    if(!ISSVNtxn(self)) return IONIL(self);

    printf("IoSVNtxn handle: %p\n", DATA(self)->handle);
    printf("IoSVNtxn pool: %p\n", DATA(self)->pool);
    return self;
}

IoObject *IoSVNtxn_propList(IoSVNtxnObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNtxn(self)) printf("IoSVNtxn_propList INVALID OBJECT\n");
    else {
        apr_hash_t* ht = NULL;
        apr_pool_t* p = (apr_pool_t*)SVN_createSubpool_(DATA(self)->pool);

        svn_error_t *err = svn_fs_txn_proplist(&ht, DATA(self)->handle, p);
        if(!err) {
            ret = IoMap_newWithAprhash_(IOSTATE, ht, p);
        }
        else {
            printf("IoSVNtxn_propList error: %s\n", err->message);
        }

        SVN_destroySubpool(p);
    }

    return ret;
}

IoObject *IoSVNtxn_changeProp(IoSVNtxnObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNtxn(self)) {
        printf("IoSVNtxn_changeProp INVALID OBJECT\n");
    }
    else {
        char* prop = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 0));
        char* val = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 1));
        apr_pool_t* pool = NULL;
        printf("IoSVNtxn_changeProp %s\n%s\n", prop, val);

        pool = (apr_pool_t*)SVN_createSubpool_(DATA(self)->pool);
        svn_string_t *ss = svn_string_create(val, pool);
        svn_error_t *err = svn_fs_change_txn_prop(DATA(self)->handle, prop, ss, pool);
        if(!err) {
            ret = self;
        }
        else {
            printf("IoSVNtxn_changeProp error: %s\n", err->message);
        }
        
        SVN_destroySubpool(pool);
    }

    return ret;
}

