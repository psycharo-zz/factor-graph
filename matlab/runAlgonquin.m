function runAlgonquin(arr)
%RUNALGONQUIN run algonquin algorithm

P = defaultParams;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    [speech, freqS] = wavread('data/speech-testing/bigtips_16'); 
    [noise, freqN] = wavread('data/noise/white');
    noise = repeatSignal(noise, length(speech));
    inSNR = 0;
    [noisySpeech, noise] = addNoise(speech, noise, inSNR);
%     soundsc(noisySpeech, freqS);
    speechLog = logPowerDomain(speech, P);
    noiseLog = logPowerDomain(noise, P);
    noisyLog = logPowerDomain(noisySpeech, P);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    NUM_FRAMES = length(noisyLog);

    t = cputime;
    
    estPowS = zeros(P.NUM_BINS, NUM_FRAMES);
    estPowN = zeros(P.NUM_BINS, NUM_FRAMES);

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
    plotSpectrogram(T,F,exp(noiseLog), CAXIS_COMMON);
    subplot(4,1,3);
    plotSpectrogram(T,F,exp(noisyLog),CAXIS_COMMON);
    subplot(4,1,4);
    plotSpectrogram(T,F,exp(speechLog),CAXIS_COMMON);
end

