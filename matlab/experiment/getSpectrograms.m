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
%     estWiener = denoiseWienerFilter(corrupted, speech, noise);
    
    NUM_NOISE = 2;
    NUM_SPEECH = [4,8,16];
    inSNR = -5:5:5;
    N = length(NUM_SPEECH);
    MAX_LEN = 400;
    
    speechTraining = prepareSpeechData('data/speech-training')';
    
    for i = 1:N
        [corrupted, noise] = addNoise(speech, noise, inSNR(i));
        speechTraining = logPowerDomain(speech, P);
               
        paramS = trainAlgonquinEM(speechTraining, 8);
        estPow = denoiseAlgonquinEM(corrupted, paramS, noise, NUM_NOISE, 10);
        estPow = estPow(:,1:MAX_LEN);
        
        T = 1:size(estPow,2);
        F = 1:size(estPow,1);
        subplot(N+2,1,i);
        plotSpectrogram(T,F,estPow, P.CAXIS);
        xlabel(['Input SNR:', int2str(inSNR(i)) ]);
        ylabel('Frequency bins');
    end
    
    subplot(N+2,1,N+1);
    plotSpectrogram(T,F,exp(speechLog(:,1:MAX_LEN)), P.CAXIS);
    xlabel('Clear speech');
    ylabel('Frequency bins');
    subplot(N+2,1,N+2);
    plotSpectrogram(T,F,exp(corruptedLog(:,1:MAX_LEN)), P.CAXIS);
    xlabel('Corruputed signal at input 0 dB SNR');    
    ylabel('Frequency bins');
    
    
    

return

