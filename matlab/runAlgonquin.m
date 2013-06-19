function runAlgonquin(SPEECH_PRIORS, NOISE_PRIORS)
%RUNALGONQUIN run algonquin algorithm

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
    nwks = {};
    for bin = 1:NUM_BINS
       nwks{bin} = Network;
       nwks{bin}.setPriors(SPEECH_PRIORS{bin}, NOISE_PRIORS{bin});
    end
    NUM_FRAMES = length(noisyLog);

    estPowS = zeros(NUM_BINS, NUM_FRAMES);
    estPowN = zeros(NUM_BINS, NUM_FRAMES);

    for f = 1:NUM_FRAMES
        for bin = 1:NUM_BINS
            frame = noisyLog(bin, f);
            [logS, logN] = nwks{bin}.process(frame);
            estPowS(bin, f) = exp(logS);
            estPowN(bin, f) = exp(logN);
        end
    end

    T = 1:size(estPowS,2);
    F = 1:size(estPowS,1);
    subplot(4,1,1);
    plotSpectrogram(T,F,estPowS);
    
    T = 1:size(estPowS,2);
    F = 1:size(estPowS,1);
    subplot(4,1,2);
    plotSpectrogram(T,F,estPowN);

    subplot(4,1,3);
    [SS,FS,TS,powS] = spectrogram(noisySpeech, FRAME_LEN, OVERLAP_LEN, FFT_SIZE);
    plotSpectrogram(TS,FS,powS);


    subplot(4,1,4);
    [SS,FS,TS,powS] = spectrogram(speech, FRAME_LEN, OVERLAP_LEN, FFT_SIZE);
    plotSpectrogram(TS,FS,powS);



end

