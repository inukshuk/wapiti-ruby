#include <string.h>

#include "options.h"
#include "reader.h"
#include "model.h"
#include "wapiti.h"

#include "native.h"


VALUE mWapiti;
VALUE mNative;

VALUE cOptions;
VALUE cNativeError;
VALUE cLogger;


extern void dotrain(mdl_t *mdl);


/* --- Options Class --- */

// Auxiliary Methods

static opt_t *get_options(VALUE self) {
	opt_t *options;	
	Data_Get_Struct(self, opt_t, options);
	return options;
}

// Copies a Ruby string to the heap and stores it in a pointer.
// Frees the pointer before assigning the new value.
static void copy_string(char **dst, VALUE rb_string) {
	Check_Type(rb_string, T_STRING);

	if (*dst) { free(*dst); *dst = (char*)0; }
	*dst = calloc(RSTRING_LEN(rb_string) + 1, sizeof(char));

	memcpy(*dst, StringValuePtr(rb_string), RSTRING_LEN(rb_string) + 1);
}


// Constructor / Desctructor

static void deallocate_options(opt_t* options) {
	
	// free string options
	if (options->input) { free(options->input); }
	if (options->output) { free(options->output); }
	if (options->algo) { free(options->algo); }
	if (options->devel) { free(options->devel); }
	if (options->pattern) { free(options->pattern); }
	
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
	
	// copy the default algorithm name to the heap so that all options strings
	// are on the heap
	char* tmp = calloc(strlen(options->algo), sizeof(char));
	memcpy(tmp, options->algo, strlen(options->algo));
	options->algo = tmp;
	
	return self;
}


// Instance Methods

static VALUE options_mode(VALUE self) {
	return INT2FIX(get_options(self)->mode);
}

static VALUE options_set_mode(VALUE self, VALUE rb_fixnum) {
	opt_t *options = get_options(self);

	Check_Type(rb_fixnum, T_FIXNUM);
	options->mode = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

static VALUE options_maxent(VALUE self) {
	return get_options(self)->maxent ? Qtrue : Qfalse;	
}

static VALUE options_set_maxent(VALUE self, VALUE rb_boolean) {
	get_options(self)->maxent = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);
	
	return rb_boolean;
}

static VALUE options_input(VALUE self) {
	char *input = get_options(self)->input;
	return rb_str_new2(input ? input : "");
}

static VALUE options_set_input(VALUE self, VALUE rb_string) {
	opt_t *options = get_options(self);
	copy_string(&(options->input), rb_string);

	return rb_string;
}

static VALUE options_output(VALUE self) {
	char *output = get_options(self)->output;
	return rb_str_new2(output ? output : "");
}

static VALUE options_set_output(VALUE self, VALUE rb_string) {
	opt_t *options = get_options(self);
	copy_string(&(options->output), rb_string);
	
	return rb_string;
}

static VALUE options_pattern(VALUE self) {
	char *pattern = get_options(self)->pattern;
	return rb_str_new2(pattern ? pattern : "");
}

static VALUE options_set_pattern(VALUE self, VALUE rb_string) {
	opt_t *options = get_options(self);
	copy_string(&(options->pattern), rb_string);
	
	return rb_string;
}

static VALUE options_model(VALUE self) {
	char *model = get_options(self)->model;
	return rb_str_new2(model ? model : "");
}

static VALUE options_set_model(VALUE self, VALUE rb_string) {
	opt_t *options = get_options(self);
	copy_string(&(options->model), rb_string);
	
	return rb_string;
}

static VALUE options_algorithm(VALUE self) {
	char *algorithm = get_options(self)->algo;
	return rb_str_new2(algorithm ? algorithm : "");
}

static VALUE options_set_algorithm(VALUE self, VALUE rb_string) {
	opt_t *options = get_options(self);
	copy_string(&(options->algo), rb_string);
	
	return rb_string;
}

static VALUE options_development_data(VALUE self) {
	char *development_data = get_options(self)->devel;
	return rb_str_new2(development_data ? development_data : "");
}

static VALUE options_set_development_data(VALUE self, VALUE rb_string) {
	opt_t *options = get_options(self);
	copy_string(&(options->devel), rb_string);
	
	return rb_string;
}


void Init_options() {
	cOptions = rb_define_class_under(mNative, "Options", rb_cObject);
	rb_define_alloc_func(cOptions, allocate_options);
	
	rb_define_method(cOptions, "initialize", initialize_options, 0);

	rb_define_method(cOptions, "mode", options_mode, 0);
	rb_define_method(cOptions, "mode=", options_set_mode, 1);

	rb_define_method(cOptions, "maxent", options_maxent, 0);
	rb_define_method(cOptions, "maxent=", options_set_maxent, 1);

	rb_define_alias(cOptions, "maxent?", "maxent");

	rb_define_method(cOptions, "input", options_input, 0);
	rb_define_method(cOptions, "input=", options_set_input, 1);

	rb_define_method(cOptions, "output", options_output, 0);	
	rb_define_method(cOptions, "output=", options_set_output, 1);

	rb_define_method(cOptions, "pattern", options_pattern, 0);
	rb_define_method(cOptions, "pattern=", options_set_pattern, 1);

	rb_define_method(cOptions, "model", options_model, 0);
	rb_define_method(cOptions, "model=", options_set_model, 1);

	rb_define_method(cOptions, "algorithm", options_algorithm, 0);
	rb_define_method(cOptions, "algorithm=", options_set_algorithm, 1);

	rb_define_alias(cOptions, "algo", "algorithm");
	rb_define_alias(cOptions, "algo=", "algorithm=");

	rb_define_method(cOptions, "development_data", options_development_data, 0);
	rb_define_method(cOptions, "development_data=", options_set_development_data, 1);

	rb_define_alias(cOptions, "devel", "development_data");
	rb_define_alias(cOptions, "devel=", "development_data=");

}


/* --- Top-Level Utility Methods --- */

static VALUE train(VALUE self __attribute__((__unused__)), VALUE rb_options) {

	if (strncmp("Wapiti::Native::Options", rb_obj_classname(rb_options), 23) != 0) {
		rb_raise(cNativeError, "argument must be a native options instance");
	} 
	
	opt_t *options = get_options(rb_options);

	if (options->mode != 0) {
		rb_raise(cNativeError, "invalid options argument: mode should be set to 0 for training");
	}

	// Next we prepare the model
	mdl_t *model = mdl_new(rdr_new(options->maxent));
	model->opt = options;

	dotrain(model);
	
	// And cleanup
	mdl_free(model);
	
	return Qnil;
}

static VALUE label(VALUE self __attribute__((__unused__)), VALUE rb_options) {
	opt_t *options = get_options(rb_options);
	return Qnil;
}

static VALUE dump(VALUE self __attribute__((__unused__)), VALUE rb_options) {
	opt_t *options = get_options(rb_options);
	return Qnil;	
}



/* --- Wapiti Extension Entry Point --- */

void Init_native() {
	mWapiti = rb_const_get(rb_mKernel, rb_intern("Wapiti"));
	mNative = rb_define_module_under(mWapiti, "Native");

	cNativeError = rb_const_get(mWapiti, rb_intern("NativeError"));
	cLogger = rb_funcall(mWapiti, rb_intern("log"), 0);
	
	rb_define_singleton_method(mNative, "train", train, 1);
	rb_define_singleton_method(mNative, "label", label, 1);
	rb_define_singleton_method(mNative, "dump", dump, 1);
	
	rb_define_const(mNative, "VERSION", rb_str_new2(VERSION));
	
	Init_options();
}