#include <stdio.h>
#include <string.h>

#include "options.h"
#include "reader.h"
#include "decoder.h"
#include "model.h"
#include "trainers.h"
#include "progress.h"
#include "quark.h"
#include "tools.h"
#include "wapiti.h"

#include "native.h"

VALUE mWapiti;
VALUE mNative;

VALUE cOptions;
VALUE cModel;

VALUE cArgumentError;
VALUE cNativeError;
VALUE cConfigurationError;
VALUE cLogger;


/* --- Forward declarations --- */

int wapiti_main(int argc, char *argv[argc]);

void dolabel(mdl_t *mdl);


/* --- Utilities --- */

static const struct {
  const char *name;
  void (* train)(mdl_t *mdl);
} trn_lst[] = {
  {"l-bfgs", trn_lbfgs},
  {"sgd-l1", trn_sgdl1},
  {"bcd",    trn_bcd  },
  {"rprop",  trn_rprop},
  {"rprop+", trn_rprop},
  {"rprop-", trn_rprop}
};
static const uint32_t trn_cnt = sizeof(trn_lst) / sizeof(trn_lst[0]);


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
  if (options->algo) { free((void*)options->algo); }
  if (options->devel) { free(options->devel); }
  if (options->pattern) { free((void*)options->pattern); }

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

  if (options->maxiter == 0) {
    options->maxiter = INT_MAX;
  }

  // copy the default algorithm name to the heap so that all options strings
  // are on the heap
  char* tmp = calloc(strlen(options->algo), sizeof(char));
  memcpy(tmp, options->algo, strlen(options->algo));
  options->algo = tmp;

  if (argc > 1) {
    rb_raise(cArgumentError,
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

static VALUE options_nbest(VALUE self) {
  return INT2FIX(get_options(self)->nbest);
}

static VALUE options_set_nbest(VALUE self, VALUE rb_fixnum) {
  Check_Type(rb_fixnum, T_FIXNUM);
  get_options(self)->nbest = FIX2INT(rb_fixnum);

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

static VALUE options_histsz(VALUE self) {
  return INT2FIX(get_options(self)->lbfgs.histsz);
}

static VALUE options_set_histsz(VALUE self, VALUE rb_fixnum) {
  Check_Type(rb_fixnum, T_FIXNUM);
  get_options(self)->lbfgs.histsz = FIX2INT(rb_fixnum);

  return rb_fixnum;
}

static VALUE options_maxls(VALUE self) {
  return INT2FIX(get_options(self)->lbfgs.maxls);
}

static VALUE options_set_maxls(VALUE self, VALUE rb_fixnum) {
  Check_Type(rb_fixnum, T_FIXNUM);
  get_options(self)->lbfgs.maxls = FIX2INT(rb_fixnum);

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

static VALUE options_eta0(VALUE self) {
  return rb_float_new(get_options(self)->sgdl1.eta0);
}

static VALUE options_set_eta0(VALUE self, VALUE rb_numeric) {
  get_options(self)->sgdl1.eta0 = NUM2DBL(rb_numeric);
  return rb_numeric;
}

static VALUE options_alpha(VALUE self) {
  return rb_float_new(get_options(self)->sgdl1.alpha);
}

static VALUE options_set_alpha(VALUE self, VALUE rb_numeric) {
  get_options(self)->sgdl1.alpha = NUM2DBL(rb_numeric);
  return rb_numeric;
}

static VALUE options_kappa(VALUE self) {
  return rb_float_new(get_options(self)->bcd.kappa);
}

static VALUE options_set_kappa(VALUE self, VALUE rb_numeric) {
  get_options(self)->bcd.kappa = NUM2DBL(rb_numeric);
  return rb_numeric;
}

static VALUE options_stpmin(VALUE self) {
  return rb_float_new(get_options(self)->rprop.stpmin);
}

static VALUE options_set_stpmin(VALUE self, VALUE rb_numeric) {
  get_options(self)->rprop.stpmin = NUM2DBL(rb_numeric);
  return rb_numeric;
}

static VALUE options_stpmax(VALUE self) {
  return rb_float_new(get_options(self)->rprop.stpmax);
}

static VALUE options_set_stpmax(VALUE self, VALUE rb_numeric) {
  get_options(self)->rprop.stpmax = NUM2DBL(rb_numeric);
  return rb_numeric;
}

static VALUE options_stpinc(VALUE self) {
  return rb_float_new(get_options(self)->rprop.stpinc);
}

static VALUE options_set_stpinc(VALUE self, VALUE rb_numeric) {
  get_options(self)->rprop.stpinc = NUM2DBL(rb_numeric);
  return rb_numeric;
}

static VALUE options_stpdec(VALUE self) {
  return rb_float_new(get_options(self)->rprop.stpdec);
}

static VALUE options_set_stpdec(VALUE self, VALUE rb_numeric) {
  get_options(self)->rprop.stpdec = NUM2DBL(rb_numeric);
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

static VALUE options_clip(VALUE self) {
  return get_options(self)->lbfgs.clip ? Qtrue : Qfalse;
}

static VALUE options_set_clip(VALUE self, VALUE rb_boolean) {
  get_options(self)->lbfgs.clip = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);
  return rb_boolean;
}

static VALUE options_cutoff(VALUE self) {
  return get_options(self)->rprop.cutoff ? Qtrue : Qfalse;
}

static VALUE options_set_cutoff(VALUE self, VALUE rb_boolean) {
  get_options(self)->rprop.cutoff = !(TYPE(rb_boolean) == T_NIL || !rb_boolean);
  return rb_boolean;
}




// String Accessors

static VALUE options_pattern(VALUE self) {
  const char *pattern = get_options(self)->pattern;
  return rb_str_new2(pattern ? pattern : "");
}

static VALUE options_set_pattern(VALUE self, VALUE rb_string) {
  opt_t *options = get_options(self);
  copy_string((char**)&(options->pattern), rb_string);

  return rb_string;
}

static VALUE options_model(VALUE self) {
  const char *model = get_options(self)->model;
  return rb_str_new2(model ? model : "");
}

static VALUE options_set_model(VALUE self, VALUE rb_string) {
  opt_t *options = get_options(self);
  copy_string(&(options->model), rb_string);

  return rb_string;
}

static VALUE options_algorithm(VALUE self) {
  const char *algorithm = get_options(self)->algo;
  return rb_str_new2(algorithm ? algorithm : "");
}

static VALUE options_set_algorithm(VALUE self, VALUE rb_string) {
  opt_t *options = get_options(self);
  copy_string((char**)&(options->algo), rb_string);

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

  rb_define_method(cOptions, "skip_tokens", options_label, 0);
  rb_define_method(cOptions, "skip_tokens=", options_set_label, 1);

  rb_define_alias(cOptions, "skip_tokens?", "skip_tokens");

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

  rb_define_method(cOptions, "clip", options_clip, 0);
  rb_define_method(cOptions, "clip=", options_set_clip, 1);

  rb_define_method(cOptions, "histsz", options_histsz, 0);
  rb_define_method(cOptions, "histsz=", options_set_histsz, 1);

  rb_define_method(cOptions, "maxls", options_maxls, 0);
  rb_define_method(cOptions, "maxls=", options_set_maxls, 1);

  rb_define_method(cOptions, "eta0", options_eta0, 0);
  rb_define_method(cOptions, "eta0=", options_set_eta0, 1);

  rb_define_method(cOptions, "alpha", options_alpha, 0);
  rb_define_method(cOptions, "alpha=", options_set_alpha, 1);

  rb_define_method(cOptions, "kappa", options_kappa, 0);
  rb_define_method(cOptions, "kappa=", options_set_kappa, 1);

  rb_define_method(cOptions, "stpmin", options_stpmin, 0);
  rb_define_method(cOptions, "stpmin=", options_set_stpmin, 1);

  rb_define_method(cOptions, "stpmax", options_stpmax, 0);
  rb_define_method(cOptions, "stpmax=", options_set_stpmax, 1);

  rb_define_method(cOptions, "stpinc", options_stpinc, 0);
  rb_define_method(cOptions, "stpinc=", options_set_stpinc, 1);

  rb_define_method(cOptions, "stpdec", options_stpdec, 0);
  rb_define_method(cOptions, "stpdec=", options_set_stpdec, 1);

  rb_define_method(cOptions, "cutoff", options_cutoff, 0);
  rb_define_method(cOptions, "cutoff=", options_set_cutoff, 1);

  rb_define_method(cOptions, "nbest", options_nbest, 0);
  rb_define_method(cOptions, "nbest=", options_set_nbest, 1);

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
    rb_raise(cArgumentError, "argument must be a Wapiti::Options instance");
  }

  mdl_t *model = get_model(self);

  // Store reference to options in model struct
  model->opt = get_options(rb_options);

  // Update reader
  model->reader->autouni = model->opt->maxent;

  // Save instance variable
  rb_ivar_set(self, rb_intern("@options"), rb_options);

  return rb_options;
}

static VALUE initialize_model(int argc, VALUE *argv, VALUE self) {
  VALUE options;

  if (argc > 1) {
    rb_raise(cArgumentError,
      "wrong number of arguments (%d for 0..1)", argc);
  }

  if (argc) {
    if (TYPE(argv[0]) == T_HASH) {
      options = rb_funcall(cOptions, rb_intern("new"), 1, argv[0]);
    }
    else {
      if (strncmp("Wapiti::Options", rb_obj_classname(argv[0]), 15) != 0) {
        rb_raise(cArgumentError, "argument must be a hash or an options instance");
      }
      options = argv[0];
    }
  }
  else {
    options = rb_funcall(cOptions, rb_intern("new"), 0);
  }

  // yield options if block_given?
  if (rb_block_given_p()) {
    rb_yield(options);
  }

  model_set_options(self, options);

  // Load a previous model if specified by options
  if (get_options(options)->model) {
    rb_funcall(self, rb_intern("load"), 0);
  }

  // initialize counters
  rb_funcall(self, rb_intern("clear_counters"), 0);

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


// Instance methods

static VALUE model_sync(VALUE self) {
  mdl_sync(get_model(self));
  return self;
}

static VALUE model_compact(VALUE self) {
  mdl_compact(get_model(self));
  return self;
}

// call-seq:
//   m.save       # => saves the model to the file defined in m.path
//   m.save(path) # => sets m.path and saves the model to the file <path>
//
// Saves the model to a file. Uses the Model's path if no argument given,
// otherwise uses the passed-in argument as the Model's path.
static VALUE model_save(int argc, VALUE *argv, VALUE self) {
  if (argc > 1) {
    rb_raise(cArgumentError,
      "wrong number of arguments (%d for 0..1)", argc);
  }

  mdl_t *model = get_model(self);

  // save passed-in path in options
  if (argc) {
    Check_Type(argv[0], T_STRING);
    rb_ivar_set(self, rb_intern("@path"), argv[0]);
  }

  // open the output file
  FILE *file = 0;
  VALUE path = rb_ivar_get(self, rb_intern("@path"));

  if (NIL_P(path)) {
    fatal("failed to save model: no path given");
  }

  if (!(file = fopen(StringValueCStr(path), "w"))) {
    fatal("failed to save model: failed to open model file");
  }

  mdl_save(model, file);
  fclose(file);

  return self;
}

static VALUE model_load(int argc, VALUE *argv, VALUE self) {
  if (argc > 1) {
    rb_raise(cArgumentError,
      "wrong number of arguments (%d for 0..1)", argc);
  }

  mdl_t *model = get_model(self);

  // save passed-in argument in options
  if (argc) {
    Check_Type(argv[0], T_STRING);
    rb_ivar_set(self, rb_intern("@path"), argv[0]);
  }

  // open the model file
  FILE *file = 0;
  VALUE path = rb_ivar_get(self, rb_intern("@path"));

  if (NIL_P(path)) {
    fatal("failed to load model: no path given");
  }

  if (!(file = fopen(StringValueCStr(path), "r"))) {
    fatal("failed to load model: failed to open model file");
  }

  mdl_load(model, file);
  fclose(file);

  return self;
}

static dat_t *to_dat(rdr_t *reader, VALUE data, bool labelled) {
  Check_Type(data, T_ARRAY);

  const unsigned int n = RARRAY_LEN(data);
  unsigned int i, j, k;

  dat_t *dat = wapiti_xmalloc(sizeof(dat_t));
  dat->nseq = 0;
  dat->mlen = 0;
  dat->lbl = labelled;
  dat->seq = wapiti_xmalloc(sizeof(seq_t*) * n);

  for (i = 0; i < n; ++i) {
    VALUE sequence = rb_ary_entry(data, i);
    Check_Type(sequence, T_ARRAY);

    k = RARRAY_LEN(sequence);
    raw_t *raw = wapiti_xmalloc(sizeof(raw_t) + sizeof(char*) * k);

    for (j = 0; j < k; ++j) {
      VALUE line = rb_ary_entry(sequence, j);
      Check_Type(line, T_STRING);
      raw->lines[j] = StringValueCStr(line);
    }

    raw->len = k;

    seq_t *seq = rdr_raw2seq(reader, raw, labelled);
    xfree(raw);

    if (seq == 0) { break; }

    // and store the sequence
    dat->seq[dat->nseq++] = seq;
    dat->mlen = max(dat->mlen, seq->len);

  }

  // if no sequence was read, free memory
  if (dat->nseq == 0) {
    xfree(dat->seq);
    xfree(dat);

    return 0;
  }

  return dat;
}


static VALUE model_train(VALUE self, VALUE data) {

  mdl_t* model = get_model(self);

  uint32_t trn;
  for (trn = 0; trn < trn_cnt; trn++) {
    if (!strcmp(model->opt->algo, trn_lst[trn].name)) break;
  }

  if (trn == trn_cnt) {
    fatal("failed to train model: unknown algorithm '%s'", model->opt->algo);
  }

  FILE *file;

  // Load the pattern file. This will unlock the database if previously
  // locked by loading a model.
  if (model->opt->pattern) {
    file = fopen(model->opt->pattern, "r");

    if (!file) {
      fatal("failed to train model: failed to load pattern file '%s'",
          model->opt->pattern);
    }

    rdr_loadpat(model->reader, file);
    fclose(file);

    qrk_lock(model->reader->obs, false);
  }


  // Load the training data. When this is done we lock the quarks as we
  // don't want to put in the model, informations present only in the
  // development set.

  switch (TYPE(data)) {
    case T_STRING:
      if (!(file = fopen(StringValuePtr(data), "r"))) {
        fatal("failed to train model: failed to open training data '%s",
          StringValuePtr(data));
      }

      model->train = rdr_readdat(model->reader, file, true);
      fclose(file);

      break;
    case T_ARRAY:
      model->train = to_dat(model->reader, data, true);

      break;
    default:
      fatal("failed to train model: invalid training data type (expected instance of String or Array)");
  }

  qrk_lock(model->reader->lbl, true);
  qrk_lock(model->reader->obs, true);

  if (!model->train || model->train->nseq == 0) {
    fatal("failed to train model: no training data loaded");
  }

  // If present, load the development set in the model. If not specified,
  // the training dataset will be used instead.
  if (model->opt->devel) {
    if (!(file = fopen(model->opt->devel, "r"))) {
      fatal("failed to train model: cannot open development file '%s'",
          model->opt->devel);
    }

    model->devel = rdr_readdat(model->reader, file, true);
    fclose(file);
  }

  // Initialize the model. If a previous model was loaded, this will be
  // just a resync, else the model structure will be created.
  // rb_funcall(self, rb_intern("sync"), 0);
	mdl_sync(model);

  // Train the model.
  uit_setup(model);
  trn_lst[trn].train(model);
  uit_cleanup(model);

  // If requested compact the model.
  if (model->opt->compact) {
    // rb_funcall(self, rb_intern("compact"), 0);
		mdl_compact(model);
  }

  return self;
}

// Returns a sorted list of all labels in the Model's label database.
static VALUE model_labels(VALUE self) {
  mdl_t *model = get_model(self);
  const uint32_t Y = model->nlbl;

  qrk_t *lp = model->reader->lbl;

  VALUE labels = rb_ary_new2(Y);

  for (unsigned int i = 0; i < Y; ++i) {
    rb_ary_push(labels, rb_str_new2(qrk_id2str(lp, i)));
  }

  rb_funcall(labels, rb_intern("sort!"), 0);

  return labels;
}

static VALUE decode_sequence(VALUE self, mdl_t *model, raw_t *raw) {
  qrk_t *lbls = model->reader->lbl;

  const unsigned int Y = model->nlbl;
  const unsigned int N = model->opt->nbest;

  seq_t *seq = rdr_raw2seq(model->reader, raw, model->opt->check);

  const unsigned int T = seq->len;
  unsigned int n, t, tcnt = 0, terr = 0, scnt = 0, serr = 0, stat[3][Y];

  uint32_t *out = wapiti_xmalloc(sizeof(uint32_t) * T * N);
  double *psc = wapiti_xmalloc(sizeof(double) * T * N);
  double *scs = wapiti_xmalloc(sizeof(double) * N);

  VALUE sequence, tokens;

  if (N == 1) {
    tag_viterbi(model, seq, out, scs, psc);
  }
  else {
    tag_nbviterbi(model, seq, N, (void*)out, scs, (void*)psc);
  }

  sequence = rb_ary_new();

  for (t = 0; t < T; ++t) {
    tokens = rb_ary_new();

    if (!model->opt->label) {
      VALUE token = rb_str_new2(raw->lines[t]);

      #ifdef HAVE_RUBY_ENCODING_H
      int enc = rb_enc_find_index("UTF-8");
      rb_enc_associate_index(token, enc);
      #endif

      rb_ary_push(tokens, token);
    }

    for (n = 0; n < N; ++n) {

      uint64_t lbl = out[t * N + n];
      rb_ary_push(tokens, rb_str_new2(qrk_id2str(lbls, lbl)));

      // output individual score
      if (model->opt->outsc) {
        rb_ary_push(tokens, rb_float_new(psc[t * N + n]));
      }

    }

    // yield token/label pair to block if given
    if (rb_block_given_p()) {
      tokens = rb_yield(tokens);
    }

    rb_ary_push(sequence, tokens);


    // TODO output sequence score: scs[n] (float)

  }

  // Statistics
  if (model->opt->check) {
    int err = 0;

    for (t = 0; t < T; ++t) {
      stat[0][seq->pos[t].lbl]++;
      stat[1][out[t * N]]++;

      if (seq->pos[t].lbl != out[t * N]) {
        terr++;
        err = 1;
      }
      else {
        stat[2][out[t * N]]++;
      }
    }

    tcnt = FIX2INT(rb_ivar_get(self, rb_intern("@token_count")));
    rb_ivar_set(self, rb_intern("@token_count"), INT2FIX(tcnt + (unsigned int)T));

    terr += FIX2INT(rb_ivar_get(self, rb_intern("@token_errors")));
    rb_ivar_set(self, rb_intern("@token_errors"), INT2FIX(terr));

    scnt = FIX2INT(rb_ivar_get(self, rb_intern("@sequence_count")));
    rb_ivar_set(self, rb_intern("@sequence_count"), INT2FIX(++scnt));

    serr = FIX2INT(rb_ivar_get(self, rb_intern("@sequence_errors")));
    rb_ivar_set(self, rb_intern("@sequence_errors"), INT2FIX(serr + err));

  }


  // Cleanup memory used for this sequence
  xfree(scs);
  xfree(psc);
  xfree(out);

  rdr_freeseq(seq);

  return sequence;
}

static VALUE decode_sequence_array(VALUE self, VALUE array) {
  Check_Type(array, T_ARRAY);
  const unsigned int n = RARRAY_LEN(array);

  mdl_t *model = get_model(self);
  raw_t *raw;

  const unsigned int N = model->opt->nbest;
  unsigned int i, j;

  VALUE result = rb_ary_new2(n * N), sequence;

  for (i = 0; i < n; ++i) {
    sequence = rb_ary_entry(array, i);
    Check_Type(sequence, T_ARRAY);

    const unsigned int k = RARRAY_LEN(sequence);
    raw = wapiti_xmalloc(sizeof(raw_t) + sizeof(char*) * k);
    raw->len = k;

    for (j = 0; j < k; ++j) {
      VALUE line = rb_ary_entry(sequence, j);
      Check_Type(line, T_STRING);

      raw->lines[j] = StringValueCStr(line);
    }

    rb_ary_push(result, decode_sequence(self, model, raw));

    xfree(raw);
  }

  return result;
}

static VALUE decode_sequence_file(VALUE self, VALUE path) {
  Check_Type(path, T_STRING);
  FILE *file;

  if (!(file = fopen(StringValueCStr(path), "r"))) {
    fatal("failed to label data: could not open file '%s'", StringValueCStr(path));
  }

  mdl_t *model = get_model(self);
  raw_t *raw;

  VALUE result = rb_ary_new();

  // Next read the input file sequence by sequence and label them, we have
  // to take care of not discarding the raw input as we want to send it
  // back to the output with the additional predicted labels.
  while (!feof(file)) {

    // So, first read an input sequence keeping the raw_t object
    // available, and label it with Viterbi.
    if ((raw = rdr_readraw(model->reader, file)) == 0) {
      break;
    }

    rb_ary_push(result, decode_sequence(self, model, raw));
    rdr_freeraw(raw);
  }

  return result;
}

// cal-seq:
//   m.label(tokens, options = {})  # => array of labelled tokens
//   m.label(filename, options = {}) # => array of labelled tokens
//
static VALUE model_label(VALUE self, VALUE data) {
  VALUE result;

  switch (TYPE(data)) {
    case T_STRING:
      result = decode_sequence_file(self, data);
      break;
    case T_ARRAY:
      result = decode_sequence_array(self, data);
      break;
    default:
      fatal("failed to label data: invalid data (expected type String or Array)");
  }

  return result;
}

static void Init_model() {
  cModel = rb_define_class_under(mWapiti, "Model", rb_cObject);
  rb_define_alloc_func(cModel, allocate_model);

  rb_define_method(cModel, "initialize", initialize_model, -1);

  rb_define_attr(cModel, "options", 1, 0);


  rb_define_method(cModel, "nlbl", model_nlbl, 0);
  rb_define_method(cModel, "labels", model_labels, 0);

  rb_define_method(cModel, "nobs", model_nobs, 0);
  rb_define_alias(cModel, "observations", "nobs");

  rb_define_method(cModel, "nftr", model_nftr, 0);
  rb_define_alias(cModel, "features", "nftr");

  rb_define_method(cModel, "sync", model_sync, 0);
  rb_define_method(cModel, "compact", model_compact, 0);
  rb_define_method(cModel, "save", model_save, -1);
  rb_define_method(cModel, "load", model_load, -1);

  rb_define_method(cModel, "train", model_train, 1);
  rb_define_method(cModel, "label", model_label, 1);
}

/* --- Top-Level Utility Methods --- */


static VALUE label(VALUE self __attribute__((__unused__)), VALUE rb_options) {
  if (strncmp("Wapiti::Options", rb_obj_classname(rb_options), 15) != 0) {
    rb_raise(cArgumentError, "argument must be a native options instance");
  }

  opt_t *options = get_options(rb_options);

  if (options->mode != 1) {
    rb_raise(cArgumentError,
      "invalid options argument: mode should be set to 1 for labelling");
  }

  mdl_t *model = mdl_new(rdr_new(options->maxent));
  model->opt = options;

  dolabel(model);

  mdl_free(model);

  return Qnil;
}

#if defined EXTRA
static VALUE dump(VALUE self __attribute__((__unused__)), VALUE rb_options) {
  if (strncmp("Wapiti::Options", rb_obj_classname(rb_options), 15) != 0) {
    rb_raise(cArgumentError, "argument must be a native options instance");
  }

  opt_t *options = get_options(rb_options);

  if (options->mode != 2) {
    rb_raise(cArgumentError,
        "invalid options argument: mode should be set to 2 for training");
  }

  mdl_t *model = mdl_new(rdr_new(options->maxent));
  model->opt = options;

  dodump(model);

  mdl_free(model);

  return Qnil;
}

// This function is a proxy for Wapiti's main entry point.
static VALUE wapiti(VALUE self __attribute__((__unused__)), VALUE arguments) {
  int result = -1, argc = 0;
  char **ap, *argv[18], *input, *tmp;

  Check_Type(arguments, T_STRING);
  tmp = StringValueCStr(arguments);

  // allocate space for argument vector
  input = (char*)malloc(strlen(tmp) + 8);

  // prepend command name
  strncpy(input, "wapiti ", 8);
  strncat(input, tmp, strlen(input) - 8);

  // remember allocation pointer
  tmp = input;

  // turn input string into argument vector (using
  // only the first seventeen tokens from input)
  for (ap = argv; (*ap = strsep(&input, " \t")) != (char*)0; ++argc) {
    if ((**ap != '\0') && (++ap >= &argv[18])) break;
  }

  // call main entry point
  result = wapiti_main(argc, argv);

  // free allocated memory
  free(tmp);

  return INT2FIX(result);
}
#endif

/* --- Wapiti Extension Entry Point --- */

void Init_native() {
  mWapiti = rb_const_get(rb_mKernel, rb_intern("Wapiti"));
  mNative = rb_define_module_under(mWapiti, "Native");

  cArgumentError = rb_const_get(rb_mKernel, rb_intern("ArgumentError"));
  cNativeError = rb_const_get(mWapiti, rb_intern("NativeError"));
  cConfigurationError = rb_const_get(mWapiti, rb_intern("ConfigurationError"));
  cLogger = rb_funcall(mWapiti, rb_intern("log"), 0);

  rb_define_singleton_method(mNative, "label", label, 1);
  // rb_define_singleton_method(mNative, "wapiti", wapiti, 1);

  rb_define_const(mNative, "VERSION", rb_str_new2(VERSION));

  Init_options();
  Init_model();
}
