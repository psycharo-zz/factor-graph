function [dg_s, dg_n] = logSumJacobian(s, n)
    tmp = exp(n - s) ./ (1 + exp(n - s));
    dg_s = 1 - tmp;
    dg_n = tmp;
return
