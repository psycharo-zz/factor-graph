function logPow = prepareTrainingSpeech
% prepare training speech data: combine several wav's into one 
% and remove silences
    
    P = defaultParams;
    PATH = 'data/speech-testing';
    signal = [];
    FILES = dir([PATH '/*.wav']);
    for i = 1:length(FILES)
        [s, fs] = wavread([PATH '/' FILES(i).name]);
        if fs ~= P.FREQUENCY
            error('unsupported frequency');
        end
        signal = [signal; s];
    end
    % soundsc(signal, P.FREQUENCY);
    dbPow = dbPowerDomain(signal, P);
    
    logPow = logPowerDomain(signal, P);
    % removing silences     
    dbMeans = mean(dbPow);
    MAX_DEVIATION = 25;
    logPow = logPow(:, dbMeans > (max(dbMeans) - MAX_DEVIATION));
end

