function [ result ] = gaussMessage(mean, variance, type)
%GAUSSMESSAGE create a gaussian message given mean and variance

    result.type = type;
    if isequal(type, 'VARIANCE')
        result.var = variance;
    elseif isequal(type, 'PRECISION')
        result.precision = variance;
    end
        
    result.mean = mean;
    
end

