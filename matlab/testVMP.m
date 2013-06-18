% basic

% mex mexfactorgraph.cpp ../algonquin.cpp ../variable.cpp ../examples.cpp ../mathutil.cpp -I../


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[speech, freqS] = wavread('bbcnews'); 
SIGNAL_LEN = length(speech);
[noise, freqN] = wavread('white');
noise = repeatSignal(noise, length(speech));
inSNR = 25;
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% nwk = NetworkArray(NUM_BINS);
% nwk.train(speechLog(1:NUM_BINS,1:200)', randn(NUM_BINS,10)');

% EDGE = ceil(sqrt(NUM_BINS));
% NOISE_PRIORS = {};
% SPEECH_PRIORS = {};
% for i = 1:NUM_BINS
%     speechBin = speechLog(i,:);
%     noiseBin = noiseLog(i,:);
%     
%     nwk = Network;
%     [priorS, priorN] = nwk.train(speechBin(1:50), noiseBin(1:300));
% 
%     SPEECH_PRIORS{i} = priorS;
%     NOISE_PRIORS{i} = priorN;
% end

% load('speechpriors.mat');
% load('noisepriors.mat');


% NUM_BINS = 3;
% 
EDGE = ceil(sqrt(NUM_BINS));
% 

bin = 3;
speechBin = speechLog(bin,:);

nwk = Network;
[priorS, priorN] = nwk.train(speechBin(1:500), [1]);
weights = [priorS(:).weight];
precs = [priorS(:).precision];
means = [priorS(:).meanPrecision] ./ precs;
	
histDistr(speechBin, 100);
hold on;
plotGMM(min(speechBin):0.1:max(speechBin), means, precs, weights);









% % drawing spectrogram
% subplot(2,1,1);
% subplot(2,1,1);
% spectrogram(speech, FRAME_LEN, OVERLAP_LEN, FFT_SIZE, 'yaxis');


% % drawing separate frequency bins
% START = 3;
% STEP = 1;
% END = 3;
% BINS = START:STEP:END;
% EDGE = ceil(sqrt(length(BINS)));
% hold on;
% for i = 1:length(BINS)
%     subplot(EDGE,EDGE, i);
%     histDistr(speech(BINS(i),:), 100);
% end    


% % testing the c++ implementation of the gmm learning
% [means, precs, weights] = mexfactorgraph('GMM', 'GMM', speechBin);


