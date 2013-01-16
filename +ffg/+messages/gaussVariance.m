function result = gaussVariance(mean, variance, from)
%GAUSSVARIANCEMESSAGE create gaussian message with variance matrix
% INPUT:
% mean - mean array
% variance - variance matrix
% OUTPUT: message struct
    result.type = 'VARIANCE';
    result.mean = mean;
    result.var = variance;
    
    if nargin == 3
        result.from = from;
    end
end

