function result = gaussianPrecisionMessage(mean, precision)
%GAUSSIANPRECISIONMESSAGE create gaussian message with precision
    result.type = 'PRECISION';
    result.mean = mean;
    result.var = precision;
end

