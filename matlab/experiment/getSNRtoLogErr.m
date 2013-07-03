function getSNRtoLogErr
% get plots with input SNR against log error
    
    P = defaultParams;

    % TODO: for different speech sentences and noise types
    [speech, freqS] = wavread('bbcnews');
    [noise, freqN] = wavread('white');
    noise = repeatSignal(noise, length(speech));

    % applying algorithm to various snrs (in dB)
    inSNR = -5:5:5;

    RESULTS = struct('wiener',[], ...
                     'gerkmann',[], ...
                     'algonquin8',[], ...
                     'algonquin16',[], ...
                     'algonquinHVMP',[], ...
                     'nofiltering', []);

    for i = 1:length(inSNR)
        corrupted = addNoise(speech, noise, inSNR(i));
        powCorrupted = powerDomain(corrupted, P);
        powSpeech = powerDomain(speech, P);
                        
        estSpeech = denoiseWienerFilter(corrupted, speech, noise);
        RESULTS.wiener(i) = logError(powerDomain(estSpeech, P),...
                                     powerDomain(speech, P));

    %     estimatedSpeech = denoiseGerkmann(noisySpeech);
    %     RESULTS.gerkmann(i) = outputSNR(speech, estimatedSpeech);

        % TODO: enable the real algonquin
    %     estimatedSpeech = denoiseAlgonquin(noisySpeech, arr);
        estPow = denoiseAlgonquinEM(corrupted, speech, noise, 8);
        RESULTS.algonquin8(i) = logError(powSpeech, estPow);
        
        estPow = denoiseAlgonquinEM(corrupted, speech, noise, 16);
        RESULTS.algonquin16(i) = logError(powSpeech, estPow);
        
        RESULTS.nofiltering(i) = logError(powSpeech, powCorrupted);
    end



    hold on;
 %     ylim([-15, 15]);
    plot(inSNR, RESULTS.wiener, '-r+')
    plot(inSNR, RESULTS.algonquin8, '--bs')
    plot(inSNR, RESULTS.algonquin16, '--g*')
    plot(inSNR, RESULTS.nofiltering, '-m^')
    legend('wiener', ...
           'algonquin 8',...
           'algonquin 16', ...
           'Location', 'NorthEast');
% 
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



