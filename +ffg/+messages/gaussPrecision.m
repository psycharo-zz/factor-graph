function result = gaussPrecision(mean, precision, from)
%GAUSSPRECISION create gaussian message with precision matrix
% INPUT:
% mean - mean array
% precision - precision matrix
% OUTPUT: message struct
    result.type = 'PRECISION';
    result.mean = mean;
    result.precision = precision;
    
    if nargin == 3
        result.from = from;
    end
end

