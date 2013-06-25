function [noisedSignal, noise] = addWhiteNoise(signal, noiseVariance)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% adding white noise to the given signal in time domain
% Input:
%       |signal| - input signal
% Output:
%       |noisedSignal| - signal with noise
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    noise = noiseVariance * (2*rand(size(signal)) - 1);
    noisedSignal = signal + noise;
    return
