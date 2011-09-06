#ifndef native_h
#define native_h

#include <ruby.h>

#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
#endif

extern VALUE mWapiti;
extern VALUE mNative;

extern VALUE cOptions;
extern VALUE cModel;

extern VALUE cNativeError;
extern VALUE cLogger;

#endif