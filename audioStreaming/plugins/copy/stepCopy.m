function [y,s] = stepCopy(x,~,s)
%STEPCOPY copies input to output (= no processing)
%
%  syntax: y = stepCopy(x)
%
%  output:
%    y = x

y = x;