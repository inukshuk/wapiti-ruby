
#include <options.h>
#include <wapiti.h>

#include <ruby.h>

VALUE mNative;
VALUE cOptions;


static void train() {
	
}

static void label() {
	
}

static void dump() {
	
}

/* --- Options Class --- */

// Auxiliary Methods

static opt_t* get_options(VALUE self) {
	opt_t* options;	
	Data_Get_Struct(self, opt_t, options);
	return options;
}

// Copies a Ruby string to the heap and returns a pointer to it.
static char* copy_string(VALUE rb_string) {
	char* data;
	
	Check_Type(rb_string, T_STRING);
	
	data = calloc(RSTRING_LEN(rb_string), sizeof(char));
	memcpy(data, StringValuePtr(rb_string), RSTRING_LEN(rb_string));
	
	return data;
}


// Constructor / Desctructor

static void deallocate_options(opt_t* options) {
	
	if (options->input) { free(options->input); }
	if (options->output) { free(options->output); }
	
	free(options);
	options = (opt_t*)0;
}

static VALUE allocate_options(VALUE self) {
	opt_t* options = malloc(sizeof(opt_t));
	return Data_Wrap_Struct(self, 0, deallocate_options, options);
}

static VALUE initialize_options(VALUE self) {
	opt_t* options = get_options(self);
	*options = opt_defaults;
	
	return self;
}


// Instance Methods

static VALUE options_mode(VALUE self) {
	return INT2FIX(get_options(self)->mode);
}

static VALUE options_set_mode(VALUE self, VALUE rb_fixnum) {
	opt_t* options = get_options(self);

	Check_Type(rb_fixnum, T_FIXNUM);
	options->mode = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

static VALUE options_input(VALUE self) {
	char* input = get_options(self)->input;
	return rb_str_new2(input ? input : "");
}

static VALUE options_set_input(VALUE self, VALUE rb_string) {
	opt_t* options = get_options(self);
	options->input = copy_string(rb_string);
	
	return rb_string;
}

static VALUE options_output(VALUE self) {
	char* output = get_options(self)->output;
	return rb_str_new2(output ? output : "");
}

static VALUE options_set_output(VALUE self, VALUE rb_string) {
	opt_t* options = get_options(self);
	options->output = copy_string(rb_string);
	
	return rb_string;
}


void Init_options() {
	cOptions = rb_define_class_under(mNative, "Options", rb_cObject);
	rb_define_alloc_func(cOptions, allocate_options);
	
	rb_define_method(cOptions, "initialize", initialize_options, 0);
	
	rb_define_method(cOptions, "mode", options_mode, 0);
	rb_define_method(cOptions, "mode=", options_set_mode, 1);

	rb_define_method(cOptions, "input", options_input, 0);
	rb_define_method(cOptions, "output", options_output, 0);
	
	rb_define_method(cOptions, "input=", options_set_input, 1);
	rb_define_method(cOptions, "output=", options_set_output, 1);

}


/* --- Entry Point --- */

void Init_native() {
	VALUE mWapiti = rb_const_get(rb_mKernel, rb_intern("Wapiti"));
	mNative = rb_define_module_under(mWapiti, "Native");

	rb_define_singleton_method(mNative, "train", train, 0);
	rb_define_singleton_method(mNative, "label", label, 0);
	rb_define_singleton_method(mNative, "dump", dump, 0);
	
	rb_define_const(mNative, "VERSION", rb_str_new2(VERSION));
	
	Init_options();
}