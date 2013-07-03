function  estPowS = denoiseAlgonquinEM(corrupted, speech, noise, NUM_SPEECH)
%DENOISEALGONQUIN pure matlab implementation of the ALGONQUIN algorithm [Frey et al.'2011]
% Input parameters:
%   |noisySpeech|   - the 
%   |noiseTrain|    - noise training data
%   |numNoise|      - the number of noise components to use
%   |numSpeech|     -
% Output parameters:
%   |estimatedSpeech| - the estimated (denoised) speech in time domain
    % the number of iterations to run inference for each frame
    NUM_ITERATIONS = 10;
    % the number of noise components
    NUM_NOISE = 2;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
    P = defaultParams;

    speechLog = logPowerDomain(speech, P);
    noiseLog = logPowerDomain(noise, P);
    corruptedLog = logPowerDomain(corrupted, P);

    NUM_FRAMES = size(speechLog,2);

    estPowN = zeros(P.NUM_BINS, NUM_FRAMES);
    estPowS = zeros(P.NUM_BINS, NUM_FRAMES);

    % speech prior training
    [mu_s, var_s, pi_s] = initSpeech(speechLog, NUM_SPEECH);
    % noise prior training
    [mu_n, var_n, pi_n] = initNoise(noiseLog(:,1:300), NUM_NOISE);
    % prior covariance likelihood matrix
    psi = ones(P.NUM_BINS, 1);

    fprintf('Running denoising\n');
    for f = 1:NUM_FRAMES
        logFrame = corruptedLog(:,f);

        [estLogPowS, estLogPowN] = processFrame(logFrame, ...
                                                pi_n, pi_s, mu_n, mu_s, ...
                                                var_n, var_s, psi, NUM_ITERATIONS);
        estPowS(:,f) = exp(estLogPowS);
        estPowN(:,f) = exp(estLogPowN);
    end
return



function [logPowS, logPowN] = processFrame(y, piN, piS, muN, muS, varN, varS, psi, numIterations)
% process a single frame of the noisy signal

    NUM_NOISE = size(piN, 2);
    % the number of speech components
    NUM_SPEECH = size(piS, 2);
    % the number of frequency bins
    NUM_BINS = size(y, 1);

    % variational parameters
    % mixture components for each pair (c_s, c_n)
    etaS = repmat(muS, [1,1,NUM_NOISE]);
    etaN = zeros(NUM_BINS, NUM_SPEECH, NUM_NOISE);
    
    phiS = ones(NUM_BINS, NUM_SPEECH, NUM_NOISE);
    phiN = ones(NUM_BINS, NUM_SPEECH, NUM_NOISE);
    
    for s = 1:NUM_SPEECH
        for n = 1:NUM_NOISE
            etaN(:,s,n) = muN(:,n);
        end
    end    
    
    % mixture weights for each pair (c_s, c_n)
    rho = ones(NUM_SPEECH, NUM_NOISE) ./ (NUM_NOISE + NUM_SPEECH);
    
    inv_psi = 1 ./ psi;
    
    % updating fi and nu
    for i = 1:numIterations
        % updating parameters for each pair        
        for c_s = 1:NUM_SPEECH
            for c_n = 1:NUM_NOISE
                meanS = etaS(:,c_s,c_n);
                meanN = etaN(:,c_s,c_n);
                % first getting a derivative matrix
                [dg_s, dg_n] = logSumJacobian(meanS, meanN);
                
                % we are making use of diagonal and bloc matrix properties
                prec_s = 1 ./ varS(:,c_s);
                prec_n = 1 ./ varN(:,c_n);
                
                % Fi, containing 3 blocks
                % fi = ( var^-1 + [dg_s, dg_n]^T * psi^-1 * [dg_s, dg_n])
                phiS(:,c_s,c_n) = 1 ./ (prec_s + (dg_s .^ 2) .* inv_psi);
                phiN(:,c_s,c_n) = 1 ./ (prec_n + (dg_n .^ 2) .* inv_psi);
                
                % nu^* = psi * (covar^-1 * (mu - nu) + g'^T * psi^-1 * (y - g))
                % current calues of nu                 
                % assuming diagonal posterior covariance matrix
                etaS(:,c_s,c_n) = meanS + phiS(:,c_s,c_n) .* (prec_s .* (muS(:,c_s) - meanS) + dg_s .* inv_psi .* (y - logSum(meanS, meanN)));
                etaN(:,c_s,c_n) = meanN + phiN(:,c_s,c_n) .* (prec_n .* (muN(:,c_n) - meanN) + dg_n .* inv_psi .* (y - logSum(meanS, meanN)));
            end
        end
    end

    % updating ro
    % TODO: check if this is correct at all     
    for c_s = 1:NUM_SPEECH
        for c_n = 1:NUM_NOISE
            prec_s = 1 ./ varS(:, c_s);
            prec_n = 1 ./ varN(:, c_n);
            
            meanS = etaS(:,c_s,c_n);
            meanN = etaN(:,c_s,c_n);
            
            % all the parts for quadratic forms             
            means = sum((y - logSum(meanS, meanN)).^2 .* inv_psi) + ...
                    sum((meanS - muS(:,c_s)).^2 .* prec_s) + ...
                    sum((meanN - muN(:,c_n)).^2 .* prec_n);
                
            [dg_s, dg_n] = logSumJacobian(meanS, meanN);
            % traces             
            % temporary precision matrices for p(y|g,psi)             
            tmp_s = (dg_s .^ 2) .* inv_psi;
            tmp_n = (dg_n .^ 2) .* inv_psi;

            % for diagonal posterior matrix             
            % TODO: CHECKME              
            traces = sum(prec_s .* phiS(:,c_s,c_n)) + ...
                     sum(prec_n .* phiN(:,c_s,c_n)) + ...
                     sum(tmp_s .* phiS(:,c_s,c_n) + tmp_n .* phiN(:,c_s,c_n));    
                 
                 
            determs = log(prod(varS(:,c_s))) + ...
                      log(prod(varN(:,c_n))) - ...
                      log(prod(phiS(:,c_s,c_n) .* phiN(:,c_s,c_n)));
                             
            rho(c_s,c_n) = piN(c_n) * piS(c_s) * exp(-0.5 * (means + traces + determs));
        end
    end
    % normalizing ro    
    rho(:,:) = rho(:,:) ./ sum(sum(rho));
    
    % computing the estimate of the clean speech
    logPowS = zeros(NUM_BINS,1);
    logPowN = zeros(NUM_BINS,1);
    for c_s = 1:NUM_SPEECH
        for c_n = 1:NUM_NOISE
            logPowS = logPowS + rho(c_s,c_n) .* etaS(:,c_s,c_n); 
            logPowN = logPowN + rho(c_s,c_n) .* etaN(:,c_s,c_n);
        end
    end
    
return


function y = logSum(s, n)
    y = s + log(1 + exp(n - s));
return

function [dg_s, dg_n] = logSumJacobian(s, n)
    tmp = exp(n - s) ./ (1 + exp(n - s));
    dg_s = 1 - tmp;
    dg_n = tmp;
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


function [mu_s, var_s, pi_s] = initSpeech(logPower, NUM_SPEECH)

    NUM_BINS = size(logPower, 1);
    % means
    mu_s = randn(NUM_BINS, NUM_SPEECH);
    % variances (diagonal)
    var_s = ones(NUM_BINS, NUM_SPEECH);
    % components weights
    pi_s = ones(1, NUM_SPEECH) ./ NUM_SPEECH;
    
    distr = gmdistribution.fit(logPower', NUM_SPEECH, 'CovType', 'diagonal');
    
    mu_s(:) = distr.mu';
    for m = 1:NUM_SPEECH
        var_s(:,m) = distr.Sigma(:,:,m);
    end
    pi_s(:) = distr.PComponents;    
    
return

