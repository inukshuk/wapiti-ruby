
#include <ruby.h>

void Init_native() {
	VALUE mWapiti = rb_const_get(rb_mKernel, rb_intern("Wapiti"));
}