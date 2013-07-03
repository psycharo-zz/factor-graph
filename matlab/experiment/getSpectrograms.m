function getSpectrograms
% run filtering and plot resulting spectrograms (of estimated speech)

    P = defaultParams;
    % only for one fixed noise level
    inSNR = 0;
    
    [speech, freq] = audioread('bbcnews.wav');
    [noise, freq] = audioread('white.wav');
    noise = repeatSignal(noise, length(speech));
    corrupted = addNoise(speech, noise, inSNR);
    
    speechLog = logPowerDomain(speech, P);
    noiseLog = logPowerDomain(noise, P);
    corruptedLog = logPowerDomain(corrupted, P);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    estimatedSpeech = denoiseWienerFilter(corrupted, speech, noise);
    estPowS = logPowerDomain(estimatedSpeech, P);
    
    T = 1:size(estPowS,2);
    F = 1:size(estPowS,1);
    
    % TODO: add another methods
    subplot(3,1,1);
    plotSpectrogram(T,F,exp(estPowS), P.CAXIS);
    subplot(3,1,2);
    plotSpectrogram(T,F,exp(corruptedLog), P.CAXIS);
   
    

return

