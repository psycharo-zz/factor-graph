function [noisedSignal, noise] = addModulatedNoise(signal, noiseVariance, freq)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% adding modulated white noise to the given signal in time domain
% Input:
%       |signal| - input signal
% Output:
%       |noisedSignal| - signal with noise
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    amplitude = 1 + 0.5 * sin(2*pi*freq*(1:length(signal)))';
    noise = noiseVariance * amplitude .* (2*rand(size(signal)) - 1);
    noisedSignal = signal + noise;
    return