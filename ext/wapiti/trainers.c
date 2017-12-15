#include <string.h>
#include "tools.h"
#include "trainers.h"

static const struct {
  const char *name;
  trn_t train;
} trn_lst[] = {
  {"l-bfgs", trn_lbfgs},
  {"sgd-l1", trn_sgdl1},
  {"bcd",    trn_bcd  },
  {"rprop",  trn_rprop},
  {"rprop+", trn_rprop},
  {"rprop-", trn_rprop}
};

static const uint32_t trn_cnt = sizeof(trn_lst) / sizeof(trn_lst[0]);

trn_t trn_get(const char *algo) {
  uint32_t trn;

  for (trn = 0; trn < trn_cnt; trn++) {
    if (!strcmp(algo, trn_lst[trn].name)) break;
  }

  if (trn == trn_cnt) {
    fatal("unknown algorithm '%s'", algo);
    trn = 0;
  }

  return trn_lst[trn].train;
}

static const char *typ_lst[] = {
	"maxent",
	"memm",
	"crf"
};

static const uint32_t typ_cnt = sizeof(typ_lst) / sizeof(typ_lst[0]);

uint32_t typ_get(const char* type) {
  uint32_t typ;

	for (typ = 0; typ < typ_cnt; typ++) {
		if (!strcmp(type, typ_lst[typ])) break;
  }

	if (typ == typ_cnt) {
		fatal("unknown model type '%s'", type);
  }

  return typ;
}

