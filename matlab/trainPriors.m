function [SPEECH_PRIORS, NOISE_PRIORS] = trainPriors

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
NUM_NOISE = 1;
NUM_SPEECH_FRAMES = 600;

NOISE_PRIORS = cell(1,NUM_BINS);
SPEECH_PRIORS = cell(1,NUM_BINS);
for i = 1:NUM_BINS
    speechBin = speechLog(i,:);
    noiseBin = noiseLog(i,:);
    
    nwk = Network;
    [priorS, priorN] = nwk.train(speechBin(1:NUM_SPEECH_FRAMES), NUM_SPEECH, ...
                                 noiseBin(1:300), NUM_NOISE, ...
                                 MAX_NUM_ITERS);
    SPEECH_PRIORS{i} = priorS;
    NOISE_PRIORS{i} = priorN;
    
    i
end



end

