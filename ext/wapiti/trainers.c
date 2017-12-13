#include <string.h>
#include "tools.h"
#include "trainers.h"

const struct {
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

const uint32_t trn_cnt = sizeof(trn_lst) / sizeof(trn_lst[0]);

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
