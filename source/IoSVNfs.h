//metadoc copyright Your Name Here 2008

#ifndef IOSVNfs_DEFINED
#define IOSVNfs_DEFINED 1

#include "svn_fs.h"

#include "IoObject.h"
#include "IoSeq.h"

typedef IoObject IoSVNfsObject;


typedef struct {
    svn_fs_t*   handle;   
    apr_pool_t* pool;
} IoSVNfsData;


#define ISSVNfs(self) (IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSVNfs_rawClone) && (DATA(self) != NULL))

IoTag*      IoSVNfs_newTag(void *state);
IoObject*   IoSVNfs_proto(void *state);
IoObject*   IoSVNfs_rawClone(IoSVNfsObject *self);
IoObject*   IoSVNfs_mark(IoSVNfsObject *self);
void        IoSVNfs_free(IoSVNfsObject *self);

//----------------------------------------------------------------------------

IoObject*   IoSVNfs_newWithHandle_(void *state, svn_fs_t* handle);
IoObject*   IoSVNfs_newWithHandle_pool_(void *state, svn_fs_t* handle, apr_pool_t* pool);
IoObject*   IoSVNfs_setHandle_(IoSVNfsObject* self, svn_fs_t* handle);
void        IoSVNfs_checkHandle(IoSVNfsObject* self);

//----------------------------------------------------------------------------

IoObject*   IoSVNfs_open(IoSVNfsObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNfs_close(IoSVNfsObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNfs_type(IoSVNfsObject *self, IoObject *locals, IoMessage *m);

IoObject*   IoSVNfs_youngestRev(IoSVNfsObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNfs_revisionProp(IoSVNfsObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNfs_revisionPropList(IoSVNfsObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNfs_revisionChangeProp(IoSVNfsObject *self, IoObject *locals, IoMessage *m);

IoObject*   IoSVNfs_openTxn(IoSVNfsObject *self, IoObject *locals, IoMessage *m);

IoObject*   IoSVNfs_printHandle(IoSVNfsObject *self, IoObject *locals, IoMessage *m);

#endif

