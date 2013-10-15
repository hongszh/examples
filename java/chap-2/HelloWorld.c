#include <jni.h>
/* Header for class HelloWorld */
#include <stdio.h>
#include "HelloWorld.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     HelloWorld
 * Method:    print
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_HelloWorld_print
  (JNIEnv *e, jobject o)
{
    printf("Hello World.\n");
    return;
}

#ifdef __cplusplus
}
#endif

