#include "svn_fs.h"

#include "aprhash_iomap.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSVNfs.h"
#include "IoSVNtxn.h"
#include "SVN.h"

#define DATA(self) ((IoSVNfsData *)IoObject_dataPointer(self))

IoTag *IoSVNfs_newTag(void *state)
{
    printf("IoSVNfs_newTag\n");

	IoTag *tag = IoTag_newWithName_("SVNfs");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSVNfs_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSVNfs_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSVNfs_rawClone);
	return tag;
}

IoObject *IoSVNfs_proto(void *state)
{
    printf("IoSVNfs_proto\n");

	IoSVNfsObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSVNfs_newTag(state));
	IoState_registerProtoWithFunc_(state, self, IoSVNfs_proto);
	{
	IoMethodTable methodTable[] = {
        //Opening and creating filesystems
        {"open", IoSVNfs_open},
        {"close", IoSVNfs_close},
        {"type", IoSVNfs_type},
        //Filesystem revisions
        {"youngestRev", IoSVNfs_youngestRev},
        {"revisionProp", IoSVNfs_revisionProp},
        {"revisionPropList", IoSVNfs_revisionPropList},
        {"revisionChangeProp", IoSVNfs_revisionChangeProp},
        //svn_fs_deltify_revision {"revisionDeltify", IoSVNFs_revisionDeltify),
        //Filesystem Transactions
        {"openTxn", IoSVNfs_openTxn},
        {"printHandle", IoSVNfs_printHandle},
        {NULL, NULL},
        };
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoObject *IoSVNfs_rawClone(IoSVNfsObject *proto)
{
    printf("IoSVNfs_rawClone\n");

	IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoSVNfsData)));
    DATA(self)->handle = NULL;
    DATA(self)->pool = SVN_createSubpool();
    printf("IoSVNfs_proto->calloc(%p)\n", DATA(self));
	return self;

    //PARA QUE SIEMPRE SEA EL MISMO PROTO
	//return IoState_protoWithInitFunction_(IOSTATE, IoSVNfs_proto);
}

IoObject *IoSVNfs_new(void *state)
{
    printf("IoSVNfs_new\n");
	IoObject* proto = IoState_protoWithInitFunction_(state, IoSVNfs_proto);
    return IOCLONE(proto);

    //PARA QUE SIEMPRE SEA EL MISMO PROTO
	//return IoState_protoWithInitFunction_(state, IoSVNfs_proto);
}

IoObject *IoSVNfs_mark(IoSVNfsObject* self)
{
    printf("IoSVNfs_mark\n");
    return self;
}

void IoSVNfs_free(IoSVNfsObject *self)
{
    printf("IoSVNfs_free\n");

    if(DATA(self)) {
        printf("IoSVNfs_free->free(%p)\n", IoObject_dataPointer(self));
        if(DATA(self)->pool) SVN_destroySubpool(DATA(self)->pool);
        free(IoObject_dataPointer(self));
    }
}

//--------------------------------------------------------------------------------------

IoObject *IoSVNfs_newWithHandle_(void *state, svn_fs_t* handle)
{
    printf("IoSVNfs_newWithHandle_ handle %p\n", handle);
    IoSVNfsObject *new = IoSVNfs_new(state);
    DATA(new)->handle = handle;
    printf("IoSVNfs_newWithHandle new %p\n", new);
    return new;
}

IoObject *IoSVNfs_newWithHandle_pool_(void *state, svn_fs_t* handle, apr_pool_t* pool)
{
    IoSVNfsObject* new = IoSVNfs_newWithHandle_(state, handle);
    SVN_destroySubpool(DATA(new)->pool);
    DATA(new)->pool = pool;
    printf("IoSVNfs_newWithHandle_pool_ new %p, pool %p\n", new, pool);
    return new;
}

IoObject *IoSVNfs_setHandle_(IoSVNfsObject* self, svn_fs_t* handle)
{
    printf("IoSVNfs_setHandle_ handle %p\n", handle);
    DATA(self)->handle = handle;
    return self;
}

void IoSVNfs_checkHandle(IoSVNfsObject* self)
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
//Opening and creating filesystems     
//--------------------------------------------------------------------------------------

IoObject *IoSVNfs_open(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
/*    IoObject *ret = IONIL(self);
    
    if(!ISSVNfs(self)) {
        printf("IoSVNfs_open INVALID OBJECT\n");
    }
    else {
        const char *path = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 0));
    
        IoSVNfs_checkHandle(self);

        svn_error_t* err;
        err = svn_io_file_lock2(path, TRUE, FALSE, DATA(self)->pool);
        if(!err) {
            svn_fs_t* fs = NULL;
            err = svn_fs_open(&fs, path, NULL, DATA(self)->pool);
            if(!err) {
                return IoSVNfs_setHandle_(self, fs);
            }
        }
        printf("IoSVNfs_open error: %s\n", err->message);
    }
    return ret;*/
}
        
IoObject *IoSVNfs_close(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNfs(self)) {
        printf("IoSVNfs_close INVALID OBJECT\n");
    }
    else {
        IoSVNfs_checkHandle(self);
        ret = self;
    }

    return ret;
}

IoObject *IoSVNfs_type(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    const char* type;
    char* path = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 0));
    apr_pool_t* p = SVN_createSubpool();
    svn_error_t *err = svn_fs_type(&type, path, p);
    if(!err) {
        ret = IOSYMBOL(type);
    }
    else {
        printf("%s\n", err->message);
    }
    SVN_destroySubpool(p);

    return ret;
}

//--------------------------------------------------------------------------------------
//Filesystem revisions
//--------------------------------------------------------------------------------------
IoObject *IoSVNfs_youngestRev(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);
    
    if(!ISSVNfs(self)) {
        printf("IoSVNfs_youngestRev INVALID OBJECT\n");
    }
    else {
        svn_revnum_t rev;
        svn_error_t *err = svn_fs_youngest_rev(&rev, DATA(self)->handle, DATA(self)->pool);
        if(!err) {
            ret = IONUMBER(rev);
        }
        else {
            printf("IoSVNfs_youngestRev error: %s\n", err->message);
        }
    }
    
    return ret;
}

IoObject *IoSVNfs_revisionProp(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);
    
    if(!ISSVNfs(self)) {
        printf("IoSVNfs_youngestRev INVALID OBJECT\n");
    }
    else {
        svn_revnum_t revnum = (svn_revnum_t)IoNumber_asLong(IoMessage_locals_valueArgAt_(m, locals, 0));
        char *propname = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 1));
        
        svn_string_t *str = NULL;
        apr_pool_t *p = SVN_createSubpool_(DATA(self)->pool);
        svn_error_t *err = svn_fs_revision_prop(&str, DATA(self)->handle, revnum, propname, p);
        if(!err) {
            ret = IOSYMBOL(str->data);
        }
        else {
            printf("%s\n", err->message);
        }
        
        SVN_destroySubpool(p);
    }
    
    return ret;
}

IoObject *IoSVNfs_revisionPropList(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);
    
    if(!ISSVNfs(self)) {
        printf("IoSVNfs_youngestRev INVALID OBJECT\n");
    }
    else {
        svn_revnum_t revnum = (svn_revnum_t)IoNumber_asLong(IoMessage_locals_valueArgAt_(m, locals, 0));
        
        apr_hash_t *ht;
        apr_pool_t *p = SVN_createSubpool_(DATA(self)->pool);
        svn_error_t *err = svn_fs_revision_proplist(&ht, DATA(self)->handle, revnum, p);
        if(!err) {
            ret = IoMap_newWithAprhash_(IOSTATE, ht, p);
        }
        else {
            printf("%s\n", err->message);
        }
        SVN_destroySubpool(p);
    }
    
    return ret;
}

IoObject *IoSVNfs_revisionChangeProp(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);
    
    if(!ISSVNfs(self)) {
        printf("IoSVNfs_youngestRev INVALID OBJECT\n");
    }
    else {
        svn_revnum_t revnum = (svn_revnum_t)IoNumber_asLong(IoMessage_locals_valueArgAt_(m, locals, 0));
        char *propname = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 1));
        char *propvalue = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 2));
        
        apr_pool_t *p = SVN_createSubpool_(DATA(self)->pool);
        svn_string_t *ss = svn_string_create(propvalue, p);
        svn_error_t *err = svn_fs_change_rev_prop(DATA(self)->handle, revnum, propname, ss, p);
        if(!err) {
            ret = self;
        }
        else {
            printf("%s\n", err->message);
        }
        SVN_destroySubpool(p);
    }
    
    return ret;
}

//--------------------------------------------------------------------------------------
//Filesystem Transactions
//--------------------------------------------------------------------------------------
IoObject *IoSVNfs_openTxn(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNfs(self)) {
        printf("IoSVNfs_opentxn INVALID OBJECT\n");
    }
    else {
        IoSeq *name = IoMessage_locals_seqArgAt_(m, locals, 0);
    
        svn_fs_txn_t *txn;
        apr_pool_t *pool = (apr_pool_t *)SVN_createSubpool_(DATA(self)->pool);
        svn_error_t *err = svn_fs_open_txn(&txn, DATA(self)->handle, CSTRING(name), pool);
        if(!err) {
            ret = IoSVNtxn_newWithHandle_pool_(IOSTATE, txn, pool);
        }
        else {
            printf("%s\n", err->message);
            SVN_destroySubpool(pool);
        }
    }

    return ret;
}

IoObject *IoSVNfs_printHandle(IoSVNfsObject *self, IoObject *locals, IoMessage *m)
{
    if(!ISSVNfs(self)) return IONIL(self);

    printf("IoSVNfs handle: %p\n", DATA(self)->handle);
    printf("IoSVNfs pool: %p\n", DATA(self)->pool);
    return self;
}


