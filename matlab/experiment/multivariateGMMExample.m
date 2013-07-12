function RESULT = multivariateGaussianMixture
%MULTIVARIATEGAUSSIANMIXTURE test framework on an artificial 2D dataset

    file = load('synthetic2d.mat');
    SYNTHETIC = file.DATA;
    % datasets to run    
    DATASETS = {SYNTHETIC.DATA, dlmread('faithful.txt')};
    NUM_MIXTURES = [5,3,3,3];
    
    
    VMP = 1;
        
    % methods to evaluate, unfortunately now VMP doesn't work without MKL        
    if VMP == 1
        METHOD = @trainGMM_VB;
        NAME = 'VMP';
    else
        METHOD = @trainGMM_EM_nondiag;
        NAME = 'EM';
    end
    
    RESULT = struct('means',{}, 'covs',{},'weights',{});
    for ds = 1:length(DATASETS)
        DATA = DATASETS{ds};
        DIMS = size(DATA,2);
        
        numMixtures = NUM_MIXTURES(ds);
            
        if VMP == 1
            [means, covs, weights, lb, iters] = METHOD(DATA, numMixtures);
        else
            [means, covs, weights] = METHOD(DATA, numMixtures);
            distr = gmdistribution(means, covs, weights);
            RESULT(ds).logL = sum(log(pdf(distr, DATA)));
        end
        
        RESULT(ds).means = means;
        RESULT(ds).covs = covs;
        RESULT(ds).weights = weights;

            
        subplot(length(DATASETS),1,ds)
        plotGMM(DATA, means, covs, weights);
    end
            
    save(strcat('figures/MVMixture', NAME), 'RESULT');
    
% 
%     data = load(strcat('MVMixture', NAME));
%     logL = computeLogLikelihood(data.RESULT, DATASETS);
%     
%     for ds = 1:length(DATASETS)
%         fprintf('%f\t&', logL(ds));
%         fprintf('\n');
%     end        


return



function logL = computeLogLikelihood(result, DATASETS)
    for i = 1:length(result)
        res = result(i);
        DATA = DATASETS{i};
        distr = gmdistribution(res.means, res.covs, res.weights);
        logL(i) = sum(log(pdf(distr, DATA)));
    end
return











