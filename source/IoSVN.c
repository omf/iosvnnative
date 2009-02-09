#include "IoState.h"
#include "IoSVN.h"
#include "SVN.h"

#define DATA(self) ((IoSVNData *)IoObject_dataPointer(self))

//---------------------------------------------------------------

// _tag makes an IoTag for the bookkeeping of names and methods for this proto
IoTag *IoSVN_newTag(void *state)
{
	printf("IoSVN_newTag\n");

	// first allocate a new IoTag
	IoTag *tag = IoTag_newWithName_("SVN");

	// record this tag as belonging to this VM
	IoTag_state_(tag, state);

	// give the tag pointers to the _free, _mark and _rawClone functions we'll need to use
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSVN_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSVN_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSVN_rawClone);
	return tag;
}

// _proto creates the first-ever instance of the prototype 
IoObject *IoSVN_proto(void *state)
{
	printf("IoSVN_proto\n");

	// First we allocate a new IoObject
	IoSVNObject *self = IoObject_new(state);

	// Then tag it
	IoObject_tag_(self, IoSVN_newTag(state));
	SVN_init();

	// then register this proto generator
	IoState_registerProtoWithFunc_(state, self, IoSVN_proto);

	// and finally, define the table of methods this proto supports
	// we just have one method here, returnSelf, then terminate the array
	// with NULLs
	{
		IoMethodTable methodTable[] = {
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

    //IoObject_setSlog_to_(self, IOSYMBOIL("HEAD"), IONUMBER());

	return self;
}

// _rawClone clones the existing proto passed as the only argument
IoObject *IoSVN_rawClone(IoSVNObject *proto)
{
	printf("IoSVN_rawClone\n");

	IoObject *self = IoObject_rawClonePrimitive(proto);
	// This is where any object-specific data would be copied
	return self;
}

// _new creates a new object from this prototype
IoObject *IoSVN_new(void *state)
{
	printf("IoSVN_new\n");

	IoObject *proto = IoState_protoWithInitFunction_(state, IoSVN_proto);
	return IOCLONE(proto);
}

// _mark is called when this proto is marked for garbage collection
// If this proto kept references to any other IoObjects, it should call their mark() methods as well.
IoObject *IoSVN_mark(IoSVNObject* self)
{
	printf("IoSVN_mark\n");

	return self;
}

// _free defines any cleanup or deallocation code to run when the object gets garbage collected
void IoSVN_free(IoSVNObject *self)
{
	// free dynamically allocated data and do any cleanup
	printf("IoSVN_free\n");
	SVN_finish();
}

