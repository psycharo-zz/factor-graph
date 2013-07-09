function RESULT = multivariateGaussianMixture
%MULTIVARIATEGAUSSIANMIXTURE test framework on an artificial 2D dataset

    file = load('synthetic2d.mat');
    SYNTHETIC = file.DATA;
    % datasets to run    
    DATASETS = {SYNTHETIC.DATA, dlmread('faithful.txt')};
    NUM_MIXTURES = [5,3,3,3];
        
    % methods to evaluate         
%     METHODS = {@trainGMM_EM_nondiag};
    METHODS = {@trainGMM_VB};
    NAMES = {'VMP'};
    
    RESULT = struct('means',{}, 'covs',{},'weights',{});
    for ds = 3:3
        DATA = DATASETS{ds};
        DIMS = size(DATA,2);
        
        for md = 1:length(METHODS)
            METHOD = METHODS{md};
            numMixtures = 1;
            [means, covs, weights, lb, iters] = METHOD(DATA, numMixtures);
            lb, iters
%             distr = gmdistribution(means, covs, weights);
            
            RESULT(ds,md).means = means;
            RESULT(ds,md).covs = covs;
            RESULT(ds,md).weights = weights;
%             RESULT(ds,md).logL = sum(log(pdf(distr, DATA)));
            
%             subplot(length(DATASETS),1,ds)
            
            plotGMM(DATA, means, covs, weights);
        end
    end
    
%     
%     for md = 1:length(METHODS)
%         fprintf('%s\t&', NAMES{md});
%     end
%     fprintf('\n');
%     for ds = 1:length(DATASETS)
%         for md = 1:length(METHODS)
%             fprintf('%f\t&', RESULT(ds,md).logL);
%         end
%         fprintf('\n');
%     end        

return










