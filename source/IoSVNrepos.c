#include "IoState.h"
#include "IoNumber.h"
#include "IoSVNrepos.h"
#include "IoSVNfs.h"
#include "SVN.h"
#include "svn_repos.h"
#include "svn_fs.h"

#define DATA(self) ((IoSVNreposData *)IoObject_dataPointer(self))

IoTag *IoSVNrepos_newTag(void *state)
{
    printf("IoSVNrepos_newTag\n");

	IoTag *tag = IoTag_newWithName_("SVNrepos");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSVNrepos_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSVNrepos_mark);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSVNrepos_rawClone);
    //IoTag_cleanupFunc_(tag, (IoTagCleanupFunc *)IoSVNrepos_cleanUp);
	return tag;
}

IoObject *IoSVNrepos_proto(void *state)
{
    printf("IoSVNrepos_proto\n");

	IoSVNreposObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSVNrepos_newTag(state));
    //OMF: Aqui es donde generalemnte se hace el calloc para los datos privados del proto
    //pero lo que quiero es que el calloc se haga cuando hago un CLONE no por el mero
    //hecho de importar el addon SVN, por eso el calloc no lo hacemos aqui, si no cuando
    //creamos un "objeto" SVNrepos/fs/etc.. explicitamente con clone.

	IoState_registerProtoWithFunc_(state, self, IoSVNrepos_proto);
	{
		IoMethodTable methodTable[] = {
        {"printHandle", IoSVNrepos_printHandle},
        {"ref", IoSVNrepos_ref},
        {"create", IoSVNrepos_create},
        {"delete", IoSVNrepos_delete},
        {"open", IoSVNrepos_open},
        {"close", IoSVNrepos_close},
        {"fs", IoSVNrepos_fs},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoObject *IoSVNrepos_rawClone(IoSVNreposObject *proto)
{
    printf("IoSVNrepos_rawClone\n");
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoSVNreposData)));
    printf("IoSVNrepos_proto->calloc(%p)\n", IoObject_dataPointer(self));
    DATA(self)->handle = NULL;
    DATA(self)->pool = SVN_createSubpool();
	return self;

    //PARA QUE SIEMPRE SEA EL MISMO PROTO
	//return IoState_protoWithInitFunction_(IOSTATE, IoSVNrepos_proto);
}

IoObject *IoSVNrepos_new(void *state)
{
    printf("IoSVNrepos_new\n");
	IoObject* proto = IoState_protoWithInitFunction_(state, IoSVNrepos_proto);
    return IOCLONE(proto);

    //PARA QUE SIEMPRE SEA EL MISMO PROTO
	//return IoState_protoWithInitFunction_(state, IoSVNrepos_proto);
}

IoObject *IoSVNrepos_mark(IoSVNreposObject* self)
{
    printf("IoSVNrepos_mark\n");
    return self;
}

//IoObject *IoSVNrepos_cleanUp(IoSVNreposObject* self)
//{
//    printf("IoSVNrepos_cleanUp\n");
//    return self;
//}

void IoSVNrepos_free(IoSVNreposObject *self)
{
	// free dynamically allocated data and do any cleanup
    printf("IoSVNrepos_free\n");

    if(DATA(self)) {
        if(DATA(self)->pool) SVN_destroySubpool(DATA(self)->pool);
        printf("IoSVNrepos_free->free(%p)\n", IoObject_dataPointer(self));
        free(IoObject_dataPointer(self));
    }
}

//--------------------------------------------------------------------------------------

IoObject *IoSVNrepos_newWithHandle_(void *state, svn_repos_t* handle)
{
    printf("IoSVNrepos_newWithHandle_ handle %p\n", handle);
    IoSVNreposObject *new = IoSVNrepos_new(state);
    DATA(new)->handle = handle;
    printf("IoSVNrepos_newWithHandle_ new %p\n", new);
    return new;
}

IoObject *IoSVNrepos_newWithHandle_pool_(void *state, svn_repos_t* handle, apr_pool_t* pool)
{
    IoSVNreposObject* new = IoSVNrepos_newWithHandle_(state, handle);
    SVN_destroySubpool(DATA(new)->pool);
    DATA(new)->pool = pool;
    printf("IoSVNrepos_newWithHandle_pool_ new %p, pool %p\n", new, pool);
    return new;
}

IoObject *IoSVNrepos_setHandle_(IoSVNreposObject* new, svn_repos_t* handle)
{
    printf("IoSVNrepos_setHandle_ handle %p\n", handle);
    DATA(new)->handle = handle;
    return new;
}

void IoSVNrepos_checkHandle(IoSVNreposObject* self)
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

//crea repo 
IoObject *IoSVNrepos_create(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNrepos(self)) {
        printf("IoSVNrepos_create INVALID OBJECT\n");
    }
    else {
        IoSeq *path = IoMessage_locals_seqArgAt_(m, locals, 0);

        IoSVNrepos_checkHandle(self);

        svn_repos_t* rh = NULL;
        svn_error_t* err = svn_repos_create(&rh, CSTRING(path), NULL, NULL, NULL, NULL, DATA(self)->pool);
        if(!err) {
            ret = IoSVNrepos_setHandle_(self, rh);
        }
        else {
            printf("IoSVNrepos_create error: %s\n", err->message);
        }
    }
    return ret;
}

//delete does not take a handle
IoObject *IoSVNrepos_delete(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    IoSeq *path = IoMessage_locals_seqArgAt_(m, locals, 0);

    apr_pool_t* p = (apr_pool_t*)SVN_createSubpool();

    svn_error_t* err = svn_repos_delete(CSTRING(path), p);
    if(err) {
        printf("IoSVNrepos_delte error: %s\n", err->message);
    }

    SVN_destroySubpool(p);

    return self;
}

//open an existing repo
IoObject *IoSVNrepos_open(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNrepos(self)) {
        printf("IoSVNrepos_open INVALID OBJECT\n");
    }
    else {
        IoSeq *path = IoMessage_locals_seqArgAt_(m, locals, 0);

        IoSVNrepos_checkHandle(self);

        svn_repos_t* rh = NULL;
        svn_error_t* err = svn_repos_open(&rh, CSTRING(path), DATA(self)->pool);
        if(!err) {
            ret = IoSVNrepos_setHandle_(self, rh);
        }
        else {
            printf("IoSVNrepos_open error: %s\n", err->message);
        }
    }
    return ret;
}

IoObject *IoSVNrepos_close(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNrepos(self)) {
        printf("IoSVNrepos_close INVALID OBJECT\n");
    }
    else {
        IoSVNrepos_checkHandle(self);
        ret = self;
    }

    return ret;
}

IoObject *IoSVNrepos_fs(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    //IoSVNfsObject* fs = (IoSVNfsObject *)IoMessage_argAt(m, locals, 0);
    //List *list = IoMessage_rawArgList(m);
    //IoObject* repos_handle_object = (IoObject*)List_at_(list, 0);
    IoObject *ret = IONIL(self);

    if(!ISSVNrepos(self)) {
        printf("IoSVNrepos_open INVALID OBJECT\n");
    }
    else {
        svn_fs_t* fh = svn_repos_fs(DATA(self)->handle);
        printf("IoSVNrepos_fs fs_handle %p\n", fh);
        if(fh) {
            ret = IoSVNfs_newWithHandle_(IOSTATE, fh);
        }
        else {
            printf("IoSVNrepos_fs error\n");
        }
    }
    return ret;
}

IoObject *IoSVNrepos_ref(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    IoObject *ret = IONIL(self);

    if(!ISSVNrepos(self)) {
        printf("IoSVNrepos_open INVALID OBJECT\n");
    }
    else {
        ret = IONUMBER((intptr_t)&(DATA(self)->handle));
    }
    
    return ret;
}

IoObject *IoSVNrepos_printHandle(IoSVNreposObject *self, IoObject *locals, IoMessage *m)
{
    if(!ISSVNrepos(self)) return IONIL(self);

    printf("IoSVNrepos handle: %p\n", DATA(self)->handle);
    printf("IoSVNrepos pool: %p\n", DATA(self)->pool);
    return self;
}

