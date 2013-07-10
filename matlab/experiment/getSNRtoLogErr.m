function RESULTS = getSNRtoLogErr
% get plots with input SNR against log error
    
    P = defaultParams;

    % TODO: for different speech sentences and noise types
    [speech, freq] = audioread('scholars_16.wav');
    [noise, freq] = audioread('white.wav');
    
    noise = repeatSignal(noise, length(speech));

    % applying algorithm to various snrs (in dB)
    

    RESULTS = struct('wiener',[], ...
                     'gerkmann',[], ...
                     'VB',[], ...
                     'EM',[], ...
                     'nofiltering', []);
    
    speechTraining = prepareSpeechData('data/speech-training')';
        
    NUM_SPEECH = [1,8,16,24,32,40,48,56];
    inSNR = [5];
    for m = 1:length(NUM_SPEECH)
        PARAMS_VB = trainAlgonquinVB(speechTraining, NUM_SPEECH(m), 10);
        PARAMS_EM = trainAlgonquinEM(speechTraining', NUM_SPEECH(m));

        for i = 1:(length(inSNR))
            fprintf('Running for SNR:%f\n', inSNR(i));
            corrupted = addNoise(speech, noise, inSNR(i));
            powCorrupted = powerDomain(corrupted, P);
            powSpeech = powerDomain(speech, P);

            estSpeech = denoiseWienerFilter(corrupted, speech, noise);
            RESULTS.wiener(m,i) = logError(powerDomain(estSpeech, P),...
                                         powerDomain(speech, P));

        %     estimatedSpeech = denoiseGerkmann(noisySpeech);
        %     RESULTS.gerkmann(i) = outputSNR(speech, estimatedSpeech);

            estPowS = denoiseAlgonquinEM(corrupted, PARAMS_VB, noise, 2, 10);
            RESULTS.VB(i,m) = logError(powSpeech, estPowS);

            estPowS = denoiseAlgonquinEM(corrupted, PARAMS_EM, noise, 2, 10);
            RESULTS.EM(i,m) = logError(powSpeech, estPowS);

            RESULTS.nofiltering(i, m) = logError(powSpeech, powCorrupted);
        end
    end

    RESULTS.NUM_SPEECH = NUM_SPEECH;
    RESULTS.inSNR = inSNR;

    hold on;
    plot(inSNR, RESULTS.wiener, '-r+');
    plot(inSNR, RESULTS.VB(:,1), '--bs');
    plot(inSNR, RESULTS.EM(:,1), '-g*');
    plot(inSNR, RESULTS.nofiltering, '-m^');
    legend('wiener', ...
           'algonquin VB 64',...
           'algonquin EM 64',...
           'no filtering', ...
           'Location', 'NorthEast');
    xlabel('input SNR');
    ylabel('logErr');

return


function result = logError(estPow, realPow)
    numFrames = size(estPow,2);
    numBins = size(estPow,1);
    % TODO: fix infitities for wiener
    p = 10 * log10(realPow(:,3:numFrames-2) ./ estPow(:,3:numFrames-2));
    logUnder = sum(sum(max(0,p)));
    logOver = sum(sum(abs(min(0,p))));
    result = (logUnder + logOver) / (numFrames * numBins);
return



