function getSpectrograms
% run filtering and plot resulting spectrograms (of estimated speech)
    P = defaultParams;
    % only for one fixed noise level
    inSNR = 5;
    
    [speech, freq] = audioread('bbcnews.wav');
    [noise, freq] = audioread('white.wav');
    noise = repeatSignal(noise, length(speech));
    [corrupted, noise] = addNoise(speech, noise, inSNR);
    
    speechLog = logPowerDomain(speech, P);
    noiseLog = logPowerDomain(noise, P);
    corruptedLog = logPowerDomain(corrupted, P);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    estWiener = denoiseWienerFilter(corrupted, speech, noise);
    estPow = denoiseAlgonquinEM(corrupted, speech, noise, 8);
    
    T = 1:size(estPow,2);
    F = 1:size(estPow,1);
    % TODO: add another methods
    subplot(3,1,1);
    plotSpectrogram(T,F,powerDomain(estWiener,P), P.CAXIS);
    subplot(3,1,2);
    plotSpectrogram(T,F,estPow, P.CAXIS);
    subplot(3,1,3);
    plotSpectrogram(T,F,exp(corruptedLog), P.CAXIS);
   
    

return

