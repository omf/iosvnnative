#ifndef IoSVNtxn_DEFINED
#define IoSVNtxn_DEFINED 1

#include "svn_repos.h"
#include "svn_fs.h"

#include "IoObject.h"
#include "IoSeq.h"

typedef IoObject IoSVNtxnObject;


typedef struct {
    svn_fs_txn_t*   handle;
    apr_pool_t*     pool;
} IoSVNtxnData;


#define ISSVNtxn(self) (IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSVNtxn_rawClone) && (DATA(self) != NULL))

IoTag*      IoSVNtxn_newTag(void *state);
IoObject*   IoSVNtxn_proto(void *state);
IoObject*   IoSVNtxn_rawClone(IoSVNtxnObject *self);
IoObject*   IoSVNtxn_mark(IoSVNtxnObject *self);
void        IoSVNtxn_free(IoSVNtxnObject *self);

IoObject*   IoSVNtxn_newWithHandle_(void *state, svn_fs_txn_t* handle);
IoObject*   IoSVNtxn_newWithHandle_pool_(void *state, svn_fs_txn_t* handle, apr_pool_t* pool);
IoObject*   IoSVNtxn_setHandle_(IoSVNtxnObject* self, svn_fs_txn_t* handle);
void        IoSVNtxn_checkHandle(IoSVNtxnObject* self);

IoObject*   IoSVNtxn_printHandle(IoSVNtxnObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNtxn_propList(IoSVNtxnObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNtxn_changeProp(IoSVNtxnObject *self, IoObject *locals, IoMessage *m);
//IoObject *IoSVNtxn_opentxn(IoSVNtxnObject *self, IoObject *locals, IoMessage *m);

#endif

