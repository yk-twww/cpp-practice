#include <cmath>
#include "util.h"


double log_sum_exp(double d1, double d2)
{
  double max_d, exp_sum;

  max_d = fmax(d1, d2);
  exp_sum = exp(d1 - max_d) + exp(d2 - max_d);

  return max_d + log(exp_sum);
}

