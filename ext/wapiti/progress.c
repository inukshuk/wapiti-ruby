/*
 *      Wapiti - A linear-chain CRF tool
 *
 * Copyright (c) 2009-2013  CNRS
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


#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "progress.h"
#include "decoder.h"
#include "options.h"
#include "tools.h"

/*******************************************************************************
 * User interaction during training
 *
 *   Handle progress reporting during training and clean early stoping. Trainers
 *   have to call uit_progress at the end of each iterations, this will display
 *   various informations for the user.
 *   Timing is also done here, an iteration is assumed to take all the time
 *   between to call to the progress function and evualtion on the devel data
 *   are included.
 *
 *   This module setup a signal handler for SIGINT. If this signal is catched,
 *   the uit_stop global variable to inform the trainer that it have to stop as
 *   early as possible, discarding the recent computations if they cannot be
 *   integrated very quickly. They must leave the model in a clean state. Any
 *   further signal will terminate the program. So it's simple :
 *     - 1 signal mean "I can wait a little so try to stop as soon as possible
 *         but leave me a working model"
 *     - 2 signal mean "Stop immediatly what you are doing, I can't wait and
 *         don't care about getting a working model"
 ******************************************************************************/

/* uit_stop:
 *   This value is set to true when the user request the trainer to stop. In
 *   this case, the trainer have to stop as soon as possible in a clean state,
 *   discarding the lasts computations if it cannot integrate them quickly.
 */
bool uit_stop = false;

/* uit_signal:
 *   Signal handler to catch interupt signal. When a signal is received, the
 *   trainer is aksed to stop as soon as possible leaving the model in a clean
 *   state. We don't reinstall the handler so if user send a second interrupt
 *   signal, the program will stop imediatly. (to cope with BSD system, we even
 *   reinstall explicitly the default handler)
 */
static void uit_signal(int sig) {
	signal(sig, SIG_DFL);
	uit_stop = true;
}

/* uit_setup:
 *   Install the signal handler for clean early stop from the user if possible
 */
void uit_setup(mdl_t *mdl) {
	uit_stop = false;
	if (signal(SIGINT, uit_signal) == SIG_ERR) {
		warning("failed to set signal handler, no clean early stop");
  }

	if (mdl->opt->stopwin != 0) {
		mdl->werr = wapiti_xmalloc(sizeof(double) * mdl->opt->stopwin);
  }

	mdl->wcnt = mdl->wpos = 0;
}

/* uit_cleanup:
 *   Remove the signal handler restoring the defaul behavior in case of
 *   interrupt.
 */
void uit_cleanup(mdl_t *mdl) {
	unused(mdl);
	if (mdl->opt->stopwin != 0) {
		free(mdl->werr);
		mdl->werr = NULL;
	}

	signal(SIGINT, SIG_DFL);
}

/* uit_progress:
 *   Display a progress report to the user consisting of information
 *   provided by the trainer: iteration count and objective function value, and
 *   some information computed here on the current model performance.
 *   This function returns true if the trainer have to keep training the model
 *   and false if he must stop, so this is were we will implement the trainer
 *   independant stoping criterion.
 */
bool uit_progress(mdl_t *mdl, uint32_t it, double obj) {
	// First we just compute the error rate on devel or train data
	double te, se;
	tag_eval(mdl, &te, &se);

	// If requested, check the error rate stoping criterion. We check if the
	// error rate is stable enought over a few iterations.
	bool res = true;
	if (mdl->opt->stopwin != 0) {
		mdl->werr[mdl->wpos] = te;
		mdl->wpos = (mdl->wpos + 1) % mdl->opt->stopwin;
		mdl->wcnt++;
		if (mdl->wcnt >= mdl->opt->stopwin) {
			double emin = 200.0, emax = -100.0;
			for (uint32_t i = 0; i < mdl->opt->stopwin; i++) {
				emin = min(emin, mdl->werr[i]);
				emax = max(emax, mdl->werr[i]);
			}
			if (emax - emin < mdl->opt->stopeps)
				res = false;
		}
	}

	return (uit_stop) ? false : res;
}
