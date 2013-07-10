function getSpectrograms
% run filtering and plot resulting spectrograms (of estimated speech)
    P = defaultParams;
    % only for one fixed noise level
    inSNR = 0;
    [speech, freq] = audioread('bbcnews.wav');
    [noise, freq] = audioread('white.wav');
    noise = repeatSignal(noise, length(speech));
    [corrupted, noise] = addNoise(speech, noise, inSNR);
    
    speechLog = logPowerDomain(speech, P);
    noiseLog = logPowerDomain(noise, P);
    corruptedLog = logPowerDomain(corrupted, P);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    estWiener = denoiseWienerFilter(corrupted, speech, noise);
    
    NUM_NOISE = 2;
    NUM_SPEECH = [4,8,16];
    N = length(NUM_SPEECH);
    
    speechTraining = prepareSpeechData('data/speech-training')';
    
    for i = 1:N
        paramS = trainAlgonquinVB(speechTraining, NUM_SPEECH(i), 10);
        estPow = denoiseAlgonquinEM(corrupted, paramS, noise, 8, 10);
        
        T = 1:size(estPow,2);
        F = 1:size(estPow,1);
        subplot(N+1,1,i);
        plotSpectrogram(T,F,estPow, P.CAXIS);
        xlabel([int2str(NUM_SPEECH(i)) ' components']);
    end
    
    subplot(N+1,1,N+1);
    plotSpectrogram(T,F,powerDomain(speech, P), P.CAXIS);
    xlabel('Original');
    subplot(N+2,1,N+2);
    plotSpectrogram(T,F,powerDomain(corrupted, P), P.CAXIS);
    xlabel('Corrputed');    
    
    
    

return

