function [arr, SPEECH_PRIORS, NOISE_PRIORS] = trainPriors

P = defaultParams;

% [speech, freqS] = wavread('bigtips_16');
[noise, freqN] = wavread('white');
speechLog = prepareTrainingSpeech;
noiseLog = logPowerDomain(noise, P);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% for prior vmp training
NUM_POINTS = min(length(speechLog), 1000);
MAX_NUM_ITERS = 300;
NUM_SPEECH = 6;
NUM_NOISE = 1;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
NOISE_PRIORS = cell(1,P.NUM_BINS);
SPEECH_PRIORS = cell(1,P.NUM_BINS);

EDGE = ceil(sqrt(P.NUM_BINS));
arr = NetworkArray(P.NUM_BINS);
testS = speechLog(1:P.NUM_BINS,1:NUM_POINTS)';
testN = noiseLog(1:P.NUM_BINS,:)';
t = now; 
arr.train(testS, NUM_SPEECH, testN, NUM_NOISE, MAX_NUM_ITERS);
elapsed = (now-t) * 1e5;
fprintf('time: %f\n', elapsed);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 

for i = 1:P.NUM_BINS
    speechBin = speechLog(i,:);
    noiseBin = noiseLog(i,:);
    
    SPEECH_PRIORS{i} = arr.speechDistr(i);
    NOISE_PRIORS{i} = arr.noiseDistr(i);
    
%     weights = [SPEECH_PRIORS{i}.weight];
%     precs = [SPEECH_PRIORS{i}.precision];
%     means = [SPEECH_PRIORS{i}.meanPrecision] ./ precs;

    weights = [NOISE_PRIORS{i}.weight];
    precs = [NOISE_PRIORS{i}.precision];
    means = [NOISE_PRIORS{i}.meanPrecision] ./ precs;

%      subplot(EDGE, EDGE, i);
%     hold on;
%     histDistr(speechBin(1:NUM_POINTS), 100);
%     plotGMM(min(speechBin):0.2:max(speechBin), means, precs, weights);   
    histDistr(noiseBin, 100);
    plotGMM(min(noiseBin):0.1:max(noiseBin), means, precs, weights);   
end





end

