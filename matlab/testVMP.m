% basic

% mex mexfactorgraph.cpp ../algonquin.cpp ../variable.cpp ../examples.cpp ../mathutil.cpp -I../


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[speech, freqS] = wavread('bbcnews'); 
SIGNAL_LEN = length(speech);
[noise, freqN] = wavread('white');
noise = repeatSignal(noise, length(speech));
inSNR = 45;
noisySpeech = addNoise(speech, noise, inSNR);

FRAME_LEN = 256; 
% shifting window length
OVERLAP_LEN = 128;
% the size of DFT
FFT_SIZE = 128;
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

load('speechpriors.mat');
load('noisepriors.mat');

% for i = 1:NUM_BINS
%     nwk = Network;
%     nwk.setPriors(SPEECH_PRIORS{i}, NOISE_PRIORS{i});
%     [speech, noise] = nwk.priors;
% end

nwks = {};
for bin = 1:NUM_BINS
   nwks{bin} = Network;
   nwks{bin}.setPriors(SPEECH_PRIORS{bin}, NOISE_PRIORS{bin});
end

bin = 3;

% NUM_FRAMES = floor(length(noisyLog) / 2);
NUM_FRAMES = length(noisyLog);

estPowS = zeros(bin, NUM_FRAMES);
estPowN = zeros(bin, NUM_FRAMES);

for f = 1:NUM_FRAMES
    for bin = 1:NUM_BINS
        frame = speechLog(bin, f);
        [logS, logN] = nwks{bin}.process(frame);
        estPowS(bin, f) = exp(logS);
        estPowN(bin, f) = exp(logN);
    end
end

T = 1:size(estPowS,2);
F = 1:size(estPowS,1);
subplot(3,1,1);
plotSpectrogram(T,F,estPowS)

subplot(3,1,2);
[SS,FS,TS,powS] = spectrogram(noisySpeech, FRAME_LEN, OVERLAP_LEN, FFT_SIZE);
plotSpectrogram(TS,FS,powS);


subplot(3,1,3);
[SS,FS,TS,powS] = spectrogram(speech, FRAME_LEN, OVERLAP_LEN, FFT_SIZE);
plotSpectrogram(TS,FS,powS);



% for f = 1:length(noisyLog)
%     frame = speechLog(bin, f);
% %     nwk.process(frame)
%     fprintf('%f,', frame);
% end
% fprintf('\n');

 

	
% histDistr(speechBin, 100);
% hold on;
% plotGMM(min(speechBin):0.1:max(speechBin), means, precs, weights);



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


