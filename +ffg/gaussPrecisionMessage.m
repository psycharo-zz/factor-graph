function result = gaussPrecisionMessage(mean, precision)
%GAUSSPRECISIONMESSAGE create gaussian message with precision
    result.type = 'PRECISION';
    result.mean = mean;
    result.precision = precision;
end

