function runAlgonquin(arr)
%RUNALGONQUIN run algonquin algorithm

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[speech, freqS] = wavread('bbcnews'); 
SIGNAL_LEN = length(speech);
[noise, freqN] = wavread('white');
noise = repeatSignal(noise, length(speech));
inSNR = 10;
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
    NUM_FRAMES = length(noisyLog);

    t = cputime;
    
    estPowS = zeros(NUM_BINS, NUM_FRAMES);
    estPowN = zeros(NUM_BINS, NUM_FRAMES);

    for f = 1:NUM_FRAMES
        frame = noisyLog(:, f);
        [logS, logN] = arr.process(frame);

        estPowS(:,f) = exp(logS);
        estPowN(:,f) = exp(logN);
    end
    
    elapsed = cputime-t
    
    CAXIS_COMMON = [-80, 30];
    
    T = 1:size(estPowS,2);
    F = 1:size(estPowS,1);
    subplot(4,1,1);
    plotSpectrogram(T,F,estPowS, CAXIS_COMMON);
    subplot(4,1,2);
    plotSpectrogram(T,F,estPowN, CAXIS_COMMON);
    subplot(4,1,3);
    plotSpectrogram(T,F,exp(noisyLog),CAXIS_COMMON);
    subplot(4,1,4);
    plotSpectrogram(T,F,exp(speechLog),CAXIS_COMMON);
    subplot(4,1,4);
    plotSpectrogram(T,F,exp(noiseLog),CAXIS_COMMON);
    
end

