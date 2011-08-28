#include <string.h>

#include "wapiti.h"
#include "options.h"
#include "reader.h"
#include "model.h"
#include "trainers.h"

#include "native.h"


VALUE mWapiti;
VALUE mNative;

VALUE cOptions;
VALUE cModel;

VALUE cNativeError;
VALUE cLogger;


/* --- Utilities --- */

static void trn_auto(mdl_t *mdl) {
	const int maxiter = mdl->opt->maxiter;
	mdl->opt->maxiter = 3;
	trn_sgdl1(mdl);
	mdl->opt->maxiter = maxiter;
	trn_lbfgs(mdl);
}

static const struct {
	char *name;
	void (* train)(mdl_t *mdl);
} trn_lst[] = {
	{"l-bfgs", trn_lbfgs},
	{"sgd-l1", trn_sgdl1},
	{"bcd",    trn_bcd  },
	{"rprop",  trn_rprop},
	{"rprop+", trn_rprop},
	{"rprop-", trn_rprop},
	{"auto",   trn_auto }
};
static const int trn_cnt = sizeof(trn_lst) / sizeof(trn_lst[0]);


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

static void mark_options(opt_t* options __attribute__((__unused__))) {
	// nothing
}

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
	return Data_Wrap_Struct(self, mark_options, deallocate_options, options);
}

static VALUE initialize_options(int argc, VALUE *argv, VALUE self) {	
	opt_t* options = get_options(self);
	*options = opt_defaults;
	
	// copy the default algorithm name to the heap so that all options strings
	// are on the heap
	char* tmp = calloc(strlen(options->algo), sizeof(char));
	memcpy(tmp, options->algo, strlen(options->algo));
	options->algo = tmp;

	if (argc > 1) {
		rb_raise(rb_const_get(rb_mKernel, rb_intern("ArgumentError")),
			"wrong number of arguments (%d for 0..1)", argc);
	}

	// set defaults
	if (argc) {
		Check_Type(argv[0], T_HASH);
		(void)rb_funcall(self, rb_intern("update"), 1, argv[0]);
	}
	
	// yield self if block_given?
	if (rb_block_given_p()) {
		rb_yield(self);
	}
	
	return self;
}


// Instance Methods


// Fixnum Accessors

static VALUE options_mode(VALUE self) {
	return INT2FIX(get_options(self)->mode);
}

static VALUE options_set_mode(VALUE self, VALUE rb_fixnum) {
	Check_Type(rb_fixnum, T_FIXNUM);
	get_options(self)->mode = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

static VALUE options_stopwin(VALUE self) {
	return INT2FIX(get_options(self)->stopwin);
}

static VALUE options_set_stopwin(VALUE self, VALUE rb_fixnum) {
	Check_Type(rb_fixnum, T_FIXNUM);
	get_options(self)->stopwin = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

static VALUE options_objwin(VALUE self) {
	return INT2FIX(get_options(self)->objwin);
}

static VALUE options_set_objwin(VALUE self, VALUE rb_fixnum) {
	Check_Type(rb_fixnum, T_FIXNUM);
	get_options(self)->objwin = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}


static VALUE options_maxiter(VALUE self) {
	return INT2FIX(get_options(self)->maxiter);
}

static VALUE options_set_maxiter(VALUE self, VALUE rb_fixnum) {
	opt_t *options = get_options(self);

	Check_Type(rb_fixnum, T_FIXNUM);
	options->maxiter = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

static VALUE options_jobsize(VALUE self) {
	return INT2FIX(get_options(self)->jobsize);
}

static VALUE options_set_jobsize(VALUE self, VALUE rb_fixnum) {
	opt_t *options = get_options(self);

	Check_Type(rb_fixnum, T_FIXNUM);
	options->jobsize = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

static VALUE options_nthread(VALUE self) {
	return INT2FIX(get_options(self)->nthread);
}

static VALUE options_set_nthread(VALUE self, VALUE rb_fixnum) {
	opt_t *options = get_options(self);

	Check_Type(rb_fixnum, T_FIXNUM);
	options->nthread = FIX2INT(rb_fixnum);
	
	return rb_fixnum;
}

// Float Accessors

static VALUE options_rho1(VALUE self) {
	return rb_float_new(get_options(self)->rho1);
}

static VALUE options_set_rho1(VALUE self, VALUE rb_numeric) {
	get_options(self)->rho1 = NUM2DBL(rb_numeric);
	return rb_numeric;
}

static VALUE options_rho2(VALUE self) {
	return rb_float_new(get_options(self)->rho2);
}

static VALUE options_set_rho2(VALUE self, VALUE rb_numeric) {
	get_options(self)->rho2 = NUM2DBL(rb_numeric);
	return rb_numeric;
}

static VALUE options_stopeps(VALUE self) {
	return rb_float_new(get_options(self)->stopeps);
}

static VALUE options_set_stopeps(VALUE self, VALUE rb_numeric) {
	get_options(self)->stopeps = NUM2DBL(rb_numeric);
	return rb_numeric;
}


// Boolean Accessors

static VALUE options_maxent(VALUE self) {
	return get_options(self)->maxent ? Qtrue : Qfalse;	
}

static VALUE options_set_maxent(VALUE self, VALUE rb_boolean) {
	get_options(self)->maxent = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);	
	return rb_boolean;
}

static VALUE options_compact(VALUE self) {
	return get_options(self)->compact ? Qtrue : Qfalse;	
}

static VALUE options_set_compact(VALUE self, VALUE rb_boolean) {
	get_options(self)->compact = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);
	return rb_boolean;
}

static VALUE options_sparse(VALUE self) {
	return get_options(self)->sparse ? Qtrue : Qfalse;	
}

static VALUE options_set_sparse(VALUE self, VALUE rb_boolean) {
	get_options(self)->sparse = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);
	return rb_boolean;
}

static VALUE options_check(VALUE self) {
	return get_options(self)->check ? Qtrue : Qfalse;	
}

static VALUE options_set_check(VALUE self, VALUE rb_boolean) {
	get_options(self)->check = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);	
	return rb_boolean;
}

static VALUE options_label(VALUE self) {
	return get_options(self)->label ? Qtrue : Qfalse;	
}

static VALUE options_set_label(VALUE self, VALUE rb_boolean) {
	get_options(self)->label = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);	
	return rb_boolean;
}

static VALUE options_outsc(VALUE self) {
	return get_options(self)->outsc ? Qtrue : Qfalse;	
}

static VALUE options_set_outsc(VALUE self, VALUE rb_boolean) {
	get_options(self)->outsc = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);	
	return rb_boolean;
}

static VALUE options_lblpost(VALUE self) {
	return get_options(self)->lblpost ? Qtrue : Qfalse;	
}

static VALUE options_set_lblpost(VALUE self, VALUE rb_boolean) {
	get_options(self)->lblpost = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);	
	return rb_boolean;
}


// String Accessors

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
	cOptions = rb_define_class_under(mWapiti, "Options", rb_cObject);
	rb_define_alloc_func(cOptions, allocate_options);
	
	rb_define_method(cOptions, "initialize", initialize_options, -1);

	// Option Accessors

	rb_define_method(cOptions, "mode", options_mode, 0);
	rb_define_method(cOptions, "mode=", options_set_mode, 1);

	rb_define_method(cOptions, "stopwin", options_stopwin, 0);
	rb_define_method(cOptions, "stopwin=", options_set_stopwin, 1);

	rb_define_alias(cOptions, "stop_window", "stopwin");
	rb_define_alias(cOptions, "stop_window=", "stopwin=");

	rb_define_method(cOptions, "objwin", options_objwin, 0);
	rb_define_method(cOptions, "objwin=", options_set_objwin, 1);

	rb_define_alias(cOptions, "convergence_window", "objwin");
	rb_define_alias(cOptions, "convergence_window=", "objwin=");
	
	rb_define_method(cOptions, "maxiter", options_maxiter, 0);
	rb_define_method(cOptions, "maxiter=", options_set_maxiter, 1);

	rb_define_alias(cOptions, "max_iterations", "maxiter");
	rb_define_alias(cOptions, "max_iterations=", "maxiter=");
	
	rb_define_method(cOptions, "jobsize", options_jobsize, 0);
	rb_define_method(cOptions, "jobsize=", options_set_jobsize, 1);

	rb_define_method(cOptions, "nthread", options_nthread, 0);
	rb_define_method(cOptions, "nthread=", options_set_nthread, 1);

	rb_define_alias(cOptions, "threads", "nthread");
	rb_define_alias(cOptions, "threads=", "nthread=");

	rb_define_method(cOptions, "rho1", options_rho1, 0);
	rb_define_method(cOptions, "rho1=", options_set_rho1, 1);

	rb_define_method(cOptions, "rho2", options_rho2, 0);
	rb_define_method(cOptions, "rho2=", options_set_rho2, 1);

	rb_define_method(cOptions, "stopeps", options_stopeps, 0);
	rb_define_method(cOptions, "stopeps=", options_set_stopeps, 1);

	rb_define_alias(cOptions, "stop_epsilon", "stopeps");
	rb_define_alias(cOptions, "stop_epsilon=", "stopeps=");

	rb_define_method(cOptions, "maxent", options_maxent, 0);
	rb_define_method(cOptions, "maxent=", options_set_maxent, 1);

	rb_define_alias(cOptions, "maxent?", "maxent");

	rb_define_method(cOptions, "compact", options_compact, 0);
	rb_define_method(cOptions, "compact=", options_set_compact, 1);

	rb_define_alias(cOptions, "compact?", "compact");

	rb_define_method(cOptions, "sparse", options_sparse, 0);
	rb_define_method(cOptions, "sparse=", options_set_sparse, 1);

	rb_define_alias(cOptions, "sparse?", "sparse");

	rb_define_method(cOptions, "label", options_label, 0);
	rb_define_method(cOptions, "label=", options_set_label, 1);

	rb_define_alias(cOptions, "label?", "label");

	rb_define_method(cOptions, "check", options_check, 0);
	rb_define_method(cOptions, "check=", options_set_check, 1);

	rb_define_alias(cOptions, "check?", "check");

	rb_define_method(cOptions, "lblpost", options_lblpost, 0);
	rb_define_method(cOptions, "lblpost=", options_set_lblpost, 1);

	rb_define_alias(cOptions, "lblpost?", "lblpost");

	rb_define_alias(cOptions, "posterior", "lblpost");
	rb_define_alias(cOptions, "posterior?", "lblpost");
	rb_define_alias(cOptions, "posterior=", "lblpost=");

	rb_define_method(cOptions, "outsc", options_outsc, 0);
	rb_define_method(cOptions, "outsc=", options_set_outsc, 1);

	rb_define_alias(cOptions, "outsc?", "outsc");

	rb_define_alias(cOptions, "score", "outsc");
	rb_define_alias(cOptions, "score?", "outsc");
	rb_define_alias(cOptions, "score=", "outsc=");

	rb_define_method(cOptions, "input", options_input, 0);
	rb_define_method(cOptions, "input=", options_set_input, 1);

	rb_define_method(cOptions, "output", options_output, 0);	
	rb_define_method(cOptions, "output=", options_set_output, 1);

	rb_define_alias(cOptions, "out", "output");
	rb_define_alias(cOptions, "out=", "output=");
	
	rb_define_method(cOptions, "pattern", options_pattern, 0);
	rb_define_method(cOptions, "pattern=", options_set_pattern, 1);

	rb_define_alias(cOptions, "template", "pattern");
	rb_define_alias(cOptions, "template=", "pattern=");

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


/* --- Model Class --- */

// Auxiliary Methods

static mdl_t *get_model(VALUE self) {
	mdl_t *model;	
	Data_Get_Struct(self, mdl_t, model);
	return model;
}

// Constructor / Desctructor

static void mark_model(mdl_t *model __attribute__((__unused__))) {
	// nothing
}

static void deallocate_model(mdl_t *model) {
	if (model) {
		mdl_free(model);
		model = (mdl_t*)0;
	}
}

static VALUE allocate_model(VALUE self) {
	mdl_t *model = mdl_new(rdr_new(false));
	return Data_Wrap_Struct(self, mark_model, deallocate_model, model);
}

static VALUE model_set_options(VALUE self, VALUE rb_options) {	
	if (strncmp("Wapiti::Options", rb_obj_classname(rb_options), 15) != 0) {
		rb_raise(cNativeError, "argument must be a Wapiti::Options instance");
	}
	
	mdl_t *model = get_model(self);
	
	// Store reference to options in model struct
	model->opt = get_options(rb_options);
	
	// Update reader
	model->reader->maxent = model->opt->maxent;
	
	// Save instance variable
	rb_ivar_set(self, rb_intern("@options"), rb_options);

	return rb_options;
}

static VALUE initialize_model(int argc, VALUE *argv, VALUE self) {
	VALUE options;
	
	if (argc > 1) {
		rb_raise(rb_const_get(rb_mKernel, rb_intern("ArgumentError")),
			"wrong number of arguments (%d for 0..1)", argc);
	}

	if (argc) {
		if (TYPE(argv[0]) == T_HASH) {
			options = rb_funcall(cOptions, rb_intern("new"), 1, argv[0]);			
		}
		else {
			if (strncmp("Wapiti::Options", rb_obj_classname(argv[0]), 15) != 0) {
				rb_raise(cNativeError, "argument must be a hash or an options instance");
			}
			options = argv[0];
		}
	}
	else {
		options = rb_funcall(cOptions, rb_intern("new"), 0);
	}
	
	// yield self if block_given?
	if (rb_block_given_p()) {
	 	rb_yield(options);
	}

	model_set_options(self, options);

	// Load a previous model if specified by options
	if (get_options(options)->model) {
		rb_funcall(self, rb_intern("load"), 0);
	}

	return self;
}


// Native accessors

static VALUE model_nlbl(VALUE self) {
	return INT2FIX(get_model(self)->nlbl);
}

static VALUE model_nobs(VALUE self) {
	return INT2FIX(get_model(self)->nobs);
}

static VALUE model_nftr(VALUE self) {
	return INT2FIX(get_model(self)->nftr);
}

static VALUE model_total(VALUE self) {
	return rb_float_new(get_model(self)->total);
}


// Instance methods

static VALUE model_sync(VALUE self) {
	mdl_sync(get_model(self));
	return self;
}

static VALUE model_compact(VALUE self) {
	mdl_compact(get_model(self));
	return self;
}

static VALUE model_save(int argc, VALUE *argv, VALUE self) {
	if (argc > 1) {
		rb_raise(rb_const_get(rb_mKernel, rb_intern("ArgumentError")),
			"wrong number of arguments (%d for 0..1)", argc);
	}
	
	FILE *file;	
	mdl_t *model = get_model(self);
	
	// save passed-in argument in options
	if (argc) {
		rb_funcall(rb_ivar_get(self, "@options"), rb_intern("model="), 1, argv[0]);
	}

	// open the output file
	if (!model->opt->model) {
		rb_raise(cNativeError, "failed to save model: no model file defined in options");
	
		if (!(file = fopen(model->opt->model, "w"))) {
			rb_raise(cNativeError, "failed to save model: failed to open model file");
		}
	}
	
	mdl_save(model, file);
	fclose(file);
		
	return self;
}

static VALUE model_load(int argc, VALUE *argv, VALUE self) {
	if (argc > 1) {
		rb_raise(rb_const_get(rb_mKernel, rb_intern("ArgumentError")),
			"wrong number of arguments (%d for 0..1)", argc);
	}
	
	FILE *file;	
	mdl_t *model = get_model(self);
	
	// save passed-in argument in options
	if (argc) {
		rb_funcall(rb_ivar_get(self, "@options"), rb_intern("model="), 1, argv[0]);
	}
	
	// open the model file
	if (!model->opt->model) {
		rb_raise(cNativeError, "failed to load model: no model file defined in options");
	
		if (!(file = fopen(model->opt->model, "r"))) {
			rb_raise(cNativeError, "failed to load model: failed to open model file");
		}
	}
	
	mdl_load(model, file);
	fclose(file);
	
	return self;
}

static VALUE model_train(int argc, VALUE *argv, VALUE self) {
	if (argc > 1) {
		rb_raise(rb_const_get(rb_mKernel, rb_intern("ArgumentError")),
			"wrong number of arguments (%d for 0..1)", argc);
	}
	
	mdl_t* model = get_model(self);
	
	int trn;
	for (trn = 0; trn < trn_cnt; trn++) {
		if (!strcmp(model->opt->algo, trn_lst[trn].name)) break;
	}
	
	if (trn == trn_cnt) {
		rb_raise(cNativeError, "failed to train model: unknown algorithm '%s'", model->opt->algo);
	}
	
	FILE *file;
	
	// Load the pattern file. This will unlock the database if previously
	// locked by loading a model.
	if (model->opt->pattern) {
		file = fopen(model->opt->pattern, "r");
		
		if (!file) {
			rb_raise(cNativeError, "failed to train model: failed to load pattern file '%s'", model->opt->pattern);
		}

		rdr_loadpat(model->reader, file);
		fclose(file);
		qrk_lock(model->reader->obs, false);
		
	}
	else {
		rb_raise(cNativeError, "failed to train model: no pattern given");
	}
	
	// Load the training data. When this is done we lock the quarks as we
	// don't want to put in the model, informations present only in the
	// devlopment set.
	if (argc) {
		Check_Type(argv[0], T_STRING);
		if (!(file = fopen(StringValuePtr(argv[0]), "r"))) {
			rb_raise(cNativeError, "failed to train model: failed to open training data '%s", StringValuePtr(argv[0]));
		}
	}
	else {
		if (model->opt->input) {
			if (!(file = fopen(model->opt->input, "r"))) {
				rb_raise(cNativeError, "failed to train model: failed to open training data '%s", model->opt->input);
			}
		}
		else {
			rb_raise(cNativeError, "failed to train model: no training data given");
		}
	}

	model->train = rdr_readdat(model->reader, file, true);

	if (model->opt->input) {
		fclose(file);
	}

	qrk_lock(model->reader->lbl, true);
	qrk_lock(model->reader->obs, true);
	
	if (!model->train || model->train->nseq == 0) {
		rb_raise(cNativeError, "failed to train model: no training data loaded");
	}

	// If present, load the development set in the model. If not specified,
	// the training dataset will be used instead.
	if (model->opt->devel) {
		if (!(file = fopen(model->opt->devel, "r"))) {
			rb_raise(cNativeError, "failed to train model: cannot open development file '%s'", model->opt->devel);
		}
		
		model->devel = rdr_readdat(model->reader, file, true);
		fclose(file);
	}
	
	// Initialize the model. If a previous model was loaded, this will be
	// just a resync, else the model structure will be created.
	rb_funcall(self, rb_intern("sync"), 0);

	// Train the model.
	uit_setup(model);
	trn_lst[trn].train(model);
	uit_cleanup(model);
	
	// If requested compact the model.
	if (model->opt->compact) {
		const size_t O = model->nobs;
		const size_t F = model->nftr;
		rb_funcall(self, rb_intern("compact"), 0);
	}	
	
	return self;
}


static void Init_model() {
	cModel = rb_define_class_under(mWapiti, "Model", rb_cObject);
	rb_define_alloc_func(cModel, allocate_model);
	
	rb_define_method(cModel, "initialize", initialize_model, -1);

	rb_define_attr(cModel, "options", 1, 0);
	
	rb_define_method(cModel, "nlbl", model_nlbl, 0);
	rb_define_alias(cModel, "labels", "nlbl");
	
	rb_define_method(cModel, "nobs", model_nobs, 0);
	rb_define_alias(cModel, "observations", "nobs");

	rb_define_method(cModel, "nftr", model_nftr, 0);
	rb_define_alias(cModel, "features", "nftr");

	rb_define_method(cModel, "total", model_total, 0);

	rb_define_method(cModel, "sync", model_sync, 0);
	rb_define_method(cModel, "compact", model_compact, 0);
	rb_define_method(cModel, "save", model_save, -1);
	rb_define_method(cModel, "load", model_load, -1);

	rb_define_method(cModel, "train", model_train, -1);
}

/* --- Top-Level Utility Methods --- */

static VALUE train(VALUE self __attribute__((__unused__)), VALUE rb_options) {
	if (strncmp("Wapiti::Options", rb_obj_classname(rb_options), 15) != 0) {
		rb_raise(cNativeError, "failed to train model: argument must be a native options instance");
	} 
	
	opt_t *options = get_options(rb_options);

	if (options->mode != 0) {
		rb_raise(cNativeError, "invalid options argument: mode should be set to 0 for training");
	}

	VALUE rb_model = rb_funcall(cModel, rb_intern("new"), 1, rb_options);
	
	dotrain(get_model(rb_model));

	return rb_model;
}

static VALUE label(VALUE self __attribute__((__unused__)), VALUE rb_options) {
	if (strncmp("Wapiti::Options", rb_obj_classname(rb_options), 15) != 0) {
		rb_raise(cNativeError, "argument must be a native options instance");
	} 

	opt_t *options = get_options(rb_options);

	if (options->mode != 1) {
		rb_raise(cNativeError, "invalid options argument: mode should be set to 1 for training");
	}

	mdl_t *model = mdl_new(rdr_new(options->maxent));
	model->opt = options;

	dolabel(model);
	
	mdl_free(model);

	return Qnil;
}

static VALUE dump(VALUE self __attribute__((__unused__)), VALUE rb_options) {
	if (strncmp("Wapiti::Options", rb_obj_classname(rb_options), 15) != 0) {
		rb_raise(cNativeError, "argument must be a native options instance");
	} 

	opt_t *options = get_options(rb_options);

	if (options->mode != 2) {
		rb_raise(cNativeError, "invalid options argument: mode should be set to 2 for training");
	}

	mdl_t *model = mdl_new(rdr_new(options->maxent));
	model->opt = options;

	dodump(model);
	
	mdl_free(model);

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
	Init_model();
}