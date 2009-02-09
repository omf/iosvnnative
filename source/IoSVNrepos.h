#ifndef IOSVNrepos_DEFINED
#define IOSVNrepos_DEFINED 1

#include "svn_repos.h"

#include "IoObject.h"
#include "IoSeq.h"

typedef IoObject IoSVNreposObject;


typedef struct {
    svn_repos_t*    handle;   
    apr_pool_t*     pool;
} IoSVNreposData;

#define ISSVNrepos(self) \
    (IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSVNrepos_rawClone) && (DATA(self) != NULL)) && DATA(self)->handle

IoTag*      IoSVNrepos_newTag(void *state);
IoObject*   IoSVNrepos_proto(void *state);
IoObject*   IoSVNrepos_rawClone(IoSVNreposObject *self);
IoObject*   IoSVNrepos_mark(IoSVNreposObject *self);
//TODO quitar cleanup
IoObject*   IoSVNrepos_cleanUp(IoSVNreposObject* self);
void        IoSVNrepos_free(IoSVNreposObject *self);

IoObject*   IoSVNrepos_newWithHandle_(void *state, svn_repos_t* handle);
IoObject*   IoSVNrepos_newWithHandle_pool_(void *state, svn_repos_t* handle, apr_pool_t* pool);
IoObject*   IoSVNrepos_setHandle_(IoSVNreposObject* new, svn_repos_t* handle);
void        IoSVNrepos_checkHandle(IoSVNreposObject* self);

IoObject*   IoSVNrepos_create(IoSVNreposObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNrepos_delete(IoSVNreposObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNrepos_open(IoSVNreposObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNrepos_close(IoSVNreposObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNrepos_fs(IoSVNreposObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNrepos_ref(IoSVNreposObject *self, IoObject *locals, IoMessage *m);
IoObject*   IoSVNrepos_printHandle(IoSVNreposObject *self, IoObject *locals, IoMessage *m);

#endif

