% basic

% mex mexfactorgraph.cpp ../algonquin.cpp ../variable.cpp ../examples.cpp ../mathutil.cpp -I../


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


% histDistr(speechBin, 100);
% hold on;
% plotGMM(min(speechBin):0.1:max(speechBin), means, precs, weights);

% % drawing separate frequency bins
NUM_BINS = 20;
EDGE = ceil(sqrt(NUM_BINS));
for i = 1:NUM_BINS
    subplot(EDGE,EDGE, i);
    
    speechBin = speechLog(i,1:600);
    histDistr(speechBin, 100);
    
    weights = [SPEECH_PRIORS{i}.weight];
    precs = [SPEECH_PRIORS{i}.precision];
    means = [SPEECH_PRIORS{i}.meanPrecision] ./ precs;
    hold on;
    
    plotGMM(min(speechBin):0.1:max(speechBin), means, precs, weights);   
    
end    


% % testing the c++ implementation of the gmm learning
% [means, precs, weights] = mexfactorgraph('GMM', 'GMM', speechBin);


