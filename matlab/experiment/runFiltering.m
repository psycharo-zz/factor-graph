function runFiltering(arr)
%RUNFILTERING test the function
%   Detailed explanation goes here

% TODO: for different speech sentences and noise types
[speech, freqS] = audioread('bbcnews');
[noise, freqN] = audioread('white');
noise = repeatSignal(noise, length(speech));

% applying algorithm to various snrs (in dB)
inSNR = -15:5:5;

RESULTS = struct('wiener',[], ...
                 'gerkmann',[], ...
                 'algonquinML',[], ...
                 'algonquinVMP',[], ...
                 'algonquinHVMP',[], ...
                 'nofiltering', []);
             
for i = 1:length(inSNR)
    noisySpeech = addNoise(speech, noise, inSNR(i));
    estimatedSpeech = denoiseWienerFilter(noisySpeech, speech, noise);
    RESULTS.wiener(i) = outputSNR(speech, estimatedSpeech);
    
%     estimatedSpeech = denoiseGerkmann(noisySpeech);
%     RESULTS.gerkmann(i) = outputSNR(speech, estimatedSpeech);
    
    % TODO: enable the real algonquin
%     estimatedSpeech = denoiseAlgonquin(noisySpeech, arr);
    RESULTS.algonquin(i) = outputSNR(speech, estimatedSpeech);
    
    RESULTS.nofiltering(i) = outputSNR(speech, noisySpeech);
end


RESULTS.wiener

hold on;
ylim([-15, 15]);
plot(inSNR, RESULTS.wiener, '-r+')
% plot(inSNR, RESULTS.gerkmann, '--bs')
plot(inSNR, RESULTS.algonquin, '--g*')
plot(inSNR, RESULTS.nofiltering, '-m^')
legend('wiener', ...
       'algonquin',...
       'no filtering', ...
       'Location', 'NorthWest');

xlabel('input SNR');
ylabel('output SNR');

return











