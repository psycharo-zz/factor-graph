function result = gaussianVarianceMessage(mean, variance)
%GAUSSIANVARIANCEMESSAGE create gaussian message with variance
    result.type = 'VARIANCE';
    result.mean = mean;
    result.var = variance;
end

