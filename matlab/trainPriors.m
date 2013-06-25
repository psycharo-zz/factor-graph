function [arr, SPEECH_PRIORS, NOISE_PRIORS] = trainPriors

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[speech, freqS] = wavread('bbcnews'); 
SIGNAL_LEN = length(speech);
[noise, freqN] = wavread('white');
noise = repeatSignal(noise, length(speech));
inSNR = 20;
noisySpeech = addNoise(speech, noise, inSNR);

FRAME_LEN = 256; 
% shifting window length
OVERLAP_LEN = 128;
% the size of DFT
FFT_SIZE = 256;
% analysis window TODO: what does periodic mean?
WINDOW  = hanning(FRAME_LEN);  
% the number of frequency bins FFT_SIZE/2+1
NUM_BINS = FFT_SIZE/2+1;

fftSpeech = spectrogram(speech(:), WINDOW, OVERLAP_LEN, FFT_SIZE);
speechLog = log(abs(fftSpeech).^2);

fftNoise = spectrogram(noise(:), WINDOW, OVERLAP_LEN, FFT_SIZE);
noiseLog = log(abs(fftNoise).^2);

fftNoisy = spectrogram(noisySpeech(:), WINDOW, OVERLAP_LEN, FFT_SIZE);
noisyLog = log(abs(fftNoisy).^2);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% for prior vmp training
MAX_NUM_ITERS = 150;

NUM_SPEECH = 8;
NUM_NOISE = 2;

NOISE_PRIORS = cell(1,NUM_BINS);
SPEECH_PRIORS = cell(1,NUM_BINS);

EDGE = ceil(sqrt(NUM_BINS));

arr = NetworkArray(NUM_BINS);

testS = speechLog(1:NUM_BINS,:)';
testN = noiseLog(1:NUM_BINS,:)';

t = cputime; 
arr.train(testS, NUM_SPEECH, testN, NUM_NOISE, MAX_NUM_ITERS);
elapsed = cputime-t

for i = 1:NUM_BINS
    speechBin = speechLog(i,:);
    noiseBin = noiseLog(i,:);
    
    SPEECH_PRIORS{i} = arr.speechDistr(i);
    NOISE_PRIORS{i} = arr.noiseDistr(i);
    
    weights = [SPEECH_PRIORS{i}.weight];
    precs = [SPEECH_PRIORS{i}.precision];
    means = [SPEECH_PRIORS{i}.meanPrecision] ./ precs;
     
    subplot(EDGE, EDGE, i);
    hold on;
    histDistr(speechBin, 100);
    plotGMM(min(speechBin):0.1:max(speechBin), means, precs, weights);   
end



end

