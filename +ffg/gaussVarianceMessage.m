function result = gaussVarianceMessage(mean, variance)
%GAUSSVARIANCEMESSAGE create gaussian message with variance
    result.type = 'VARIANCE';
    result.mean = mean;
    result.var = variance;
end

