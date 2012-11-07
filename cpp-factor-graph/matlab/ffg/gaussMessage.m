function [ result ] = gaussMessage(mean, variance)
%GAUSSMESSAGE create a gaussian message given mean and variance
    result.type = 1;
    result.mean = mean;
    result.var = variance;
end

