/*
 *      Wapiti - A linear-chain CRF tool
 *
 * Copyright (c) 2009-2011  CNRS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"
#include "native.h"

/*
 * Wapiti Ruby Logging
 *
 * Wapiti-Ruby delegates all wapiti logging messages to a Ruby logger in the
 * main Wapiti module.
 *
 */

FILE *ufopen(VALUE path, const char *mode) {
  FILE *file = (FILE*)0;
  Check_Type(path, T_STRING);

  if (!(file = fopen(StringValueCStr(path), mode))) {
    pfatal("failed to open file '%s'", StringValueCStr(path));
  }

  return file;
}

/*******************************************************************************
 * Error handling and memory managment
 *
 *   Wapiti use a very simple system for error handling: violently fail. Errors
 *   can occurs in two cases, when user feed Wapiti with bad datas or when there
 *   is a problem on the system side. In both cases, there is nothing we can do,
 *   so the best thing is to exit with a meaning full error message.
 *
 *   Memory allocation is one of the possible point of failure and its painfull
 *   to always remeber to check return value of malloc so we provide wrapper
 *   around it and realloc who check and fail in case of error.
 ******************************************************************************/

/* fatal:
 *   This is the main error function, it will print the given message with same
 *   formating than the printf family and exit program with an error. We let the
 *   OS care about freeing ressources.
 */
__attribute__((noreturn)) void fatal(const char *fmt, ...) {
  VALUE msg;
	va_list args;
	va_start(args, fmt);
  msg = rb_vsprintf(fmt, args);
	va_end(args);
	rb_raise(cNativeError, "%s", StringValueCStr(msg));
}

/* pfatal:
 *   This one is very similar to the fatal function but print an additional
 *   system error message depending on the errno. This can be used when a
 *   function who set the errno fail to print more detailed informations. You
 *   must be carefull to not call other functino that might reset it before
 *   calling pfatal.
 */
__attribute__((noreturn)) void pfatal(const char *fmt, ...) {
	const char *err = strerror(errno);
  VALUE msg;
	va_list args;
	va_start(args, fmt);
  msg = rb_vsprintf(fmt, args);
	va_end(args);
	rb_str_catf(msg, ": %s", err);
	rb_raise(cNativeError, "%s", StringValueCStr(msg));
}

/* warning:
 *   This one is less violent as it just print a warning on stderr, but doesn't
 *   exit the program. It is intended to inform the user that something strange
 *   have happen and the result might be not what it have expected.
 */
void warning(const char *fmt, ...) {
  VALUE msg;
	va_list args;
	va_start(args, fmt);
  msg = rb_vsprintf(fmt, args);
	va_end(args);
	(void)rb_funcall(cLogger, rb_intern("warn"), 1, msg);
}

/* info:
 *   Function used for all progress reports. This is where an eventual verbose
 *   level can be implemented later or redirection to a logfile. For now, it is
 *   just a wrapper for printf to stderr. Note that unlike the previous one,
 *   this function doesn't automatically append a new line character.
 */
void info(const char *fmt, ...) {
  VALUE msg;
	va_list args;
	va_start(args, fmt);
  msg = rb_vsprintf(fmt, args);
	va_end(args);
	(void)rb_funcall(cLogger, rb_intern("info"), 1, msg);
}

/*  wapiti_xmalloc:
 *   A simple wrapper around malloc who violently fail if memory cannot be
 *   allocated, so it will never return NULL.
 */
void *wapiti_xmalloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr == NULL)
		fatal("out of memory");
	return ptr;
}

/*  wapiti_xrealloc:
 *   As  wapiti_xmalloc, this is a simple wrapper around realloc who fail on memory
 *   error and so never return NULL.
 */
void *wapiti_xrealloc(void *ptr, size_t size) {
	void *new = realloc(ptr, size);
	if (new == NULL)
		fatal("out of memory");
	return new;
}

/* xstrdup:
 *   As the previous one, this is a safe version of xstrdup who fail on
 *   allocation error.
 */
char *xstrdup(const char *str) {
	const int len = strlen(str) + 1;
	char *res =  wapiti_xmalloc(sizeof(char) * len);
	memcpy(res, str, len);
	return res;
}

/******************************************************************************
 * Netstring for persistent storage
 *
 *   This follow the format proposed by D.J. Bernstein for safe and portable
 *   storage of string in persistent file and networks. This used for storing
 *   strings in saved models.
 *   We just add an additional end-of-line character to make the output files
 *   more readable.
 *
 ******************************************************************************/

/* ns_readstr:
 *   Read a string from the given file in netstring format. The string is
 *   returned as a newly allocated bloc of memory 0-terminated.
 */
char *ns_readstr(FILE *file) {
	int len;
	if (fscanf(file, "%d:", &len) != 1)
		pfatal("cannot read from file");
	char *buf =  wapiti_xmalloc(len + 1);
	if (fread(buf, len, 1, file) != 1)
		pfatal("cannot read from file");
	if (fgetc(file) != ',')
		fatal("invalid format");
	buf[len] = '\0';
	fgetc(file);
	return buf;
}

/* ns_writestr:
 *   Write a string in the netstring format to the given file.
 */
void ns_writestr(FILE *file, const char *str) {
	const int len = strlen(str);
	if (fprintf(file, "%d:%s,\n", len, str) < 0)
		pfatal("cannot write to file");
}

