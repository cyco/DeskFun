
#if __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#endif