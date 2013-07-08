function multivariateGaussianMixture
%MULTIVARIATEGAUSSIANMIXTURE test framework on an artificial 2D dataset

    %     
    file = load('synthetic.mat');
    SYNTHETIC = file.SYNTHETIC;
    
    % datasets to run    
    DATASETS = {SYNTHETIC.DATA, dlmread('faithful.txt')};
    NUM_MIXTURES = [5,3,3,3];
    
    % the number of repetitions to make using each method
    NUM_REPS = 1;
    
    % methods to evaluate         
%     METHODS = {@trainGMM_EM_nondiag};
    METHODS = {@trainGMM_VB};
    NAMES = {'VMP'};
    
    
    RESULT = struct('means',{});
    for ds = 1:length(DATASETS)
        DATA = DATASETS{ds};
        DIMS = size(DATA,2);
        
        for md = 1:length(METHODS)
            METHOD = METHODS{md};
            [means, covs, weights] = METHOD(DATA, NUM_MIXTURES(ds));
%             distr = gmdistribution(means, covs, weights);
            
            RESULT(ds,md).means = means;
            RESULT(ds,md).covs = covs;
            RESULT(ds,md).weights = weights;
%             RESULT(ds,md).logL = sum(log(pdf(distr, DATA)));
            
            subplot(length(DATASETS),1,ds)
            
            plotGMM(DATA, means, covs, weights);
        end
    end
    
    
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

end

