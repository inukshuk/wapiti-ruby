
#include <wapiti.h>
#include <ruby.h>

static void train() {
	
}

static void label() {
	
}

static void dump() {
	
}

void Init_native() {
	VALUE mWapiti = rb_const_get(rb_mKernel, rb_intern("Wapiti"));
	VALUE mNative = rb_define_module_under(mWapiti, "Native");

	rb_define_singleton_method(mNative, "train", train, 0);
	rb_define_singleton_method(mNative, "label", label, 0);
	rb_define_singleton_method(mNative, "dump", dump, 0);
	
	rb_define_const(mNative, "VERSION", rb_str_new2(VERSION));
}