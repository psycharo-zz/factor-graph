function univariateGaussianMixture
%UNIVARIATEGAUSSIANMIXTURE test framework

% description of the measures used?

FILES = {'enzyme.txt', 'acidity.txt', 'galaxy.txt'}
NUM_BINS = 50;
numMixtures = 4;

RESULTS = struct('logL',{}, 'name',{});


for f = 1:length(FILES)
    DATA = dlmread(FILES{f})';
        
    [means, covs, weights] = trainGMM_EM(DATA, numMixtures);
%     [means, covs, weights] = trainGMM_VB(DATA, numMixtures);
    distr = gmdistribution(means, covs, weights);
    histDistr(DATA, NUM_BINS);
    hold on;
    X = min(DATA):1e-4:max(DATA);
    plot(X, pdf(distr, X'), '-r', 'LineWidth', 3);
    
    RESULTS{f}.means = means;
    RESULTS{f}.covs = covs;
    RESULTS{f}.weights = weights;
    
    RESULTS{f}.name = FILES{f};
    RESULTS{f}.logL = sum(log(pdf(distr, DATA)));
end


% is log-likelihood a proper measure?

return



