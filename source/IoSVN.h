#ifndef IOSVN_DEFINED
#define IOSVN_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISSVN(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSVNObject_rawClone)

typedef IoObject IoSVNObject;

IoTag *IoSVN_newTag(void *state);
IoObject *IoSVN_proto(void *state);
IoObject *IoSVN_rawClone(IoSVNObject *self);
IoObject *IoSVN_mark(IoSVNObject *self);
void IoSVN_free(IoSVNObject *self);


#endif

