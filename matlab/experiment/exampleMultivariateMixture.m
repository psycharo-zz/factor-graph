function exampleMultivariateMixture

    file = load('synthetic2d.mat');
    SYNTHETIC = file.DATA;
    % datasets to run    
    DATASETS = {SYNTHETIC.DATA, dlmread('faithful.txt')};
    NUM_MIXTURES = [5,3,3,3];
    
    ds = 4;
    [means, covs, weights] = trainMVMixture_VMP(DATASETS{ds}, NUM_MIXTURES(ds), 300);
    
    plotGMM(DATASETS{ds}, means', covs, weights);

return


function [means, covs, weights] = trainMVMixture_VMP(data, numMixtures, maxNumIters)
    [assigns, centers] = kmeans(data, numMixtures, 'EmptyAction', 'singleton');
    [means, covs, weights, iters, evidence] = mexfactorgraph('examples', 'multivariateMixture', data, numMixtures, maxNumIters, assigns, centers);
return

