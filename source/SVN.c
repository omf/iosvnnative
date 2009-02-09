#include <stdio.h>
#include "SVN.h"

#include "apr_general.h"
#include "apr_pools.h"
#include "svn_types.h"
#include "svn_pools.h"


typedef struct 
{
    int             initialized;
	apr_allocator_t	*allocator;
	apr_pool_t	    *pool;
} SVNData;

static SVNData svn_data;

static int SVN_isInitialized()
{
    if(svn_data.initialized) {
        return TRUE;
    }
    else {
        fprintf(stderr, "SVN not initialized\n");
        return FALSE;
    }
}

#define SVN_CHECK_INITIALZED(ret) \
    if(!SVN_isInitialized()) return (ret)

int SVN_init(void)
{
    if(!svn_data.initialized) {
    	apr_status_t aprsts;
	    apr_allocator_t *allocator;

	    printf("SVN_init\n");

        if(APR_SUCCESS != apr_initialize()) {
		    printf("<SVN_init> error apr_initialize\n");
		    return FALSE;
	    }

	    if(APR_SUCCESS != (aprsts = apr_allocator_create(&allocator))) {
		    printf("<SVN_init> error apr_allocator_create\n");
		    return FALSE;
	    }

	    svn_data.allocator = allocator;
	    svn_data.pool = svn_pool_create_ex(NULL, allocator);
        svn_data.initialized = TRUE;
    }	
	return TRUE;	
}

void SVN_finish(void)
{
    if(svn_data.initialized) {
	    printf("SVN_finish\n");

        svn_pool_destroy(svn_data.pool);
        apr_terminate();
        svn_data.initialized = FALSE;
    }
}

void* SVN_getAllocator(void)
{
    SVN_CHECK_INITIALZED(NULL);
    return (void*)svn_data.allocator;
}

void* SVN_getPool(void)
{
    SVN_CHECK_INITIALZED(NULL);
    return (void*)svn_data.pool;
}

int SVN_getInitialized(void)
{
    return svn_data.initialized;
}


void* SVN_createSubpool(void)
{
    return (void*)svn_pool_create_ex(svn_data.pool, svn_data.allocator);
}

void* SVN_createSubpool_(void* p)
{
    return (void*)svn_pool_create_ex((apr_pool_t *)p, svn_data.allocator);
}

void  SVN_destroySubpool(void* pool)
{
    svn_pool_destroy((apr_pool_t*)pool);
}

void  SVN_clearSubpool(void* pool)
{
    svn_pool_clear((apr_pool_t*)pool);
}

