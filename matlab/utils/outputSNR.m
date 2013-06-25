function snr = outputSNR(speech, estimatedSpeech)
%OUTPUTSNR compute the average SNR for the estimated signal
% Input parameters:
%   |speech|          - speech signal in time-domain
%   |estimatedSpeech| - the output of the noise reduction algorithm
% Output parameters:
%   |snr| - SNR in dBs
    snr = 10 * log10(sum((speech).^2) / sum((estimatedSpeech - speech).^2));
return