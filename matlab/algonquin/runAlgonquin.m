function runAlgonquin(nwk)
%RUNALGONQUIN run algonquin algorithm

P = defaultParams;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    [speech, freqS] = audioread('bbcnews.wav'); 
    [noise, freqN] = audioread('white.wav');
    
    noise = repeatSignal(noise, length(speech));
    inSNR = 0;
    [noisySpeech, noise] = addNoise(speech, noise, inSNR);
    speechLog = logPowerDomain(speech, P)';
    noiseLog = logPowerDomain(noise, P);
    noisyLog = logPowerDomain(noisySpeech, P);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    speech
    NUM_FRAMES = length(noisyLog);

    t = cputime;
    
    % training network
    
   
    estPowS = zeros(P.NUM_BINS, NUM_FRAMES);
    estPowN = zeros(P.NUM_BINS, NUM_FRAMES);

    for f = 1:2
        frame = noisyLog(f, :);
        [logS, logN] = nwk.process(frame);
%         estPowS(:,f) = exp(logS);
%         estPowN(:,f) = exp(logN);
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

