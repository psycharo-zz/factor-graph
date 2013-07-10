function  estPowS = denoiseAlgonquinEM(corrupted, paramS, noise, NUM_NOISE, NUM_ITERATIONS)
%DENOISEALGONQUIN pure matlab implementation of the ALGONQUIN algorithm [Frey et al.'2011]
% Input parameters:
%   |noisySpeech|   - the 
%   |noiseTrain|    - noise training data
%   |numNoise|      - the number of noise components to use
%   |numSpeech|     -
% Output parameters:
%   |estimatedSpeech| - the estimated (denoised) speech in time domain
    % the number of iterations to run inference for each frame
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    

    P = defaultParams;

    noiseLog = logPowerDomain(noise, P);
    corruptedLog = logPowerDomain(corrupted, P);

    % noise prior training
    [mu_n, var_n, pi_n] = initNoise(noiseLog(:,1:length(noiseLog)), NUM_NOISE);
    % prior covariance likelihood matrix: TODO: make a parameter
    psi = ones(P.NUM_BINS, 1);
    [logPowS, logPowN] = mexfactorgraph('algonquin', 'algonquin', corruptedLog, ...
                                        paramS.mean, paramS.cov, paramS.weight, ...,
                                        mu_n, var_n, pi_n, ...
                                        NUM_ITERATIONS);
    estPowS = exp(logPowS);
    estPowN = exp(logPowN);        
return


function [mu_n, var_n, pi_n] = initNoise(logPower, NUM_NOISE)
    NUM_BINS = size(logPower, 1);

    mu_n = randn(NUM_BINS, NUM_NOISE); 
    var_n = ones(NUM_BINS, NUM_NOISE);
    pi_n = ones(1, NUM_NOISE) ./ NUM_NOISE;
    
    distr = gmdistribution.fit(logPower', NUM_NOISE, 'CovType', 'diagonal');
    
    mu_n(:) = distr.mu';
    for m = 1:NUM_NOISE
        var_n(:,m) = distr.Sigma(:,:,m);
    end
    pi_n(:) = distr.PComponents;
return
