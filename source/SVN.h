#ifndef SVN_H
#define SVN_H 1

int   SVN_init(void);
void  SVN_finish(void);

void* SVN_getAllocator(void);
void* SVN_getPool(void);
int   SVN_getInitialized(void);

void* SVN_createSubpool(void);
void* SVN_createSubpool_(void*);
void  SVN_destroySubpool(void*);
void  SVN_clearSubpool(void*);
#endif
