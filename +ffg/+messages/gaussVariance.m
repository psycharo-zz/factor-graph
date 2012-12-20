function result = gaussVariance(mean, variance)
%GAUSSVARIANCEMESSAGE create gaussian message with variance matrix
% INPUT:
% mean - mean array
% variance - variance matrix
% OUTPUT: message struct
    result.type = 'VARIANCE';
    result.mean = mean;
    result.var = variance;
end

