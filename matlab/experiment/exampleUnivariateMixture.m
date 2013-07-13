function exampleUnivariateMixture
%UNIVARIATEGAUSSIANMIXTURE test framework on univariate mixture datasets

% description of the measures used?

FILES = {'enzyme.txt', 'acidity.txt', 'galaxy.txt'};
NAMES = {'Enzime', 'Acidity', 'Galaxy'};

numMixtures = 4;

RESULTS = struct('logL',{}, 'name',{});

for f = 1:length(FILES)
    DATA = dlmread(FILES{f})';
    [means, covs, weights, iters, lbEvidence] = trainMixture_VMP(DATA, numMixtures, 800);
    
    RESULTS{f}.lbEvidence = lbEvidence;
    
%     [means, covs, weights] = trainGMM_EM(DATA, numMixtures);
    distr = gmdistribution(means, covs, weights');    
    
%     X = min(DATA)-1:1e-4:max(DATA)+1;
%     subplot(3, 1, f);
%     hold on;
%     histDistr(DATA, 50);
%     plot(X, pdf(distr, X'), '-r', 'LineWidth', 3);
%     xlabel(NAMES{f});
%     ylabel('density');
    
    RESULTS{f}.means = means;
    RESULTS{f}.covs = covs;
    RESULTS{f}.weights = weights;
    
    RESULTS{f}.name = FILES{f};
    RESULTS{f}.logL = sum(log(pdf(distr, DATA)));
end


lb = RESULTS{3}.lbEvidence;
plot(lb)
xlabel('Variational likelihood')
ylabel('Number of iterations')
ylim([min(lb)-1, max(lb)+1])



return


function [means, covs, weights, iters, lbEvidence] = trainMixture_VMP(data, numMixtures, maxNumIters)
% train a univariate mixture model using VMP
    [means, precs, weights, iters, lbEvidence] = mexfactorgraph('examples', 'univariateMixture', data, numMixtures, maxNumIters);
    covs = reshape(1./precs, [1,1,numMixtures]);
return

function [means, covs, weights] = trainMixture_EM(data, numMixtures)
    error('not implemented');
return








