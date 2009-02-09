//metadoc copyright Your Name Here 2008

// don't forget the macro guard
#ifndef IOSVN_DEFINED
#define IOSVN_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

// define a macro that can check whether an IoObject is of our type by checking whether it holds a pointer to our clone function
#define ISSVN(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSVNObject_rawClone)

// declare a C type for ourselves
typedef IoObject IoSVNObject;


// define the requisite functions
IoTag *IoSVN_newTag(void *state);
IoObject *IoSVN_proto(void *state);
IoObject *IoSVN_rawClone(IoSVNObject *self);
IoObject *IoSVN_mark(IoSVNObject *self);
void IoSVN_free(IoSVNObject *self);


#endif

