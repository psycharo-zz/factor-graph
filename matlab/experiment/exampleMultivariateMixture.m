function exampleMultivariateMixture

    file = load('synthetic2d.mat');
    SYNTHETIC = file.DATA;
    % datasets to run    
    DATASETS = {SYNTHETIC.DATA, dlmread('faithful.txt')};
    NUM_MIXTURES = [5,3,3,2];
    
    ds = 4;
    [means, covs, weights, iters, evidence, resps] = trainGMM_VMP(DATASETS{ds}, NUM_MIXTURES(ds), 300);
    
    plotGMM(DATASETS{ds}, means, covs, weights);

return



