function [ result ] = gaussMessage(mean, variance, type)
%GAUSSMESSAGE create a gaussian message given mean and variance

    if isequal(type, 'VARIANCE')
        result.type = 1;
        result.var = variance;
    elseif isequal(type, 'PRECISION')
        result.type = 2;
        result.precision = variance;
    end
        
    result.mean = mean;
    
end

