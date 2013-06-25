function [mu, var, weights] = trainGMM(data, numMixtures) %#codegen
%TRAIN1DGMM train a mixture model
% Input parameters:
%   |data|       - the training vector
%   |numMixture| - the number of components to train
% Output parameters:
%   |mu|,|var|,|weights| - means, variances and mixture weights
% TODO: substitute this with a VB method
    distr = gmdistribution.fit(data, numMixtures, 'CovType', 'diagonal');
    var = zeros(1,numMixtures);
    for m = 1:numMixtures
        var(m) = distr.Sigma(:,:,m);
    end
    mu = distr.mu;
    weights = distr.PComponents;    
return
