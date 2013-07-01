function nwk = trainNetwork

    P = defaultParams;
    [speech, freq] = audioread('bbcnews.wav');
    speechLog = logPowerDomain(speech, P)';
    speechLog = speechLog(1:900,:);
    
    
    [noise, freq] = audioread('white.wav');
    noiseLog = logPowerDomain(noise, P)';
    noiseLog = noiseLog(1:300,:);
    
    NUM_SPEECH = 32;
    NUM_NOISE = 2;
    MAX_NUM_ITERS = 10;
    
    nwk = Network;
    nwk.train(speechLog, NUM_SPEECH, noiseLog, NUM_NOISE, MAX_NUM_ITERS);
      
%     mexfactorgraph('MVGMM', 'MVGMM', speechLog, NUM_SPEECH, MAX_NUM_ITERS);
%     mexfactorgraph('MVGMM', 'MVGMM', speechLog(:,:), NUM_NOISE, MAX_NUM_ITERS);
    

end

