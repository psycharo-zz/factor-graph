function [estimatedSpeech, estimatedNoisePow] = denoiseGerkmann(noisySignal)
%DENOISEGERKMANN the denoising algorithm based on noise tracking
% Input parameters: 
%   |noisySignal| - the signal corrupted with noise
% Output parameters:
%   |estimatedSpeech|   - the estimated clean speech
%   |estimatedNoisePow| - the estimated power of the noise signal
% (c) Gerkmann


% frame size, the same with FFT_SIZE
FRAME_LEN = 512;  
% shifting window size
WINDOW_LEN  = FRAME_LEN / 2; 
% analysis window
WINDOW  = hanning(FRAME_LEN, 'periodic'); 
% the level to clamp signal at
SNR_CLAMP = 200;

SIGNAL_LEN = length(noisySignal);
numFrames = floor(SIGNAL_LEN / WINDOW_LEN) - 1; 


% spectral subtraction parameter
minGain = 0.6;

% parameters
PH1mean  = 0.5;
alphaPH1mean = 0.9;
alphaPSD = 0.8;

% constants for a posteriori SPP
q          = 0.4; 
% a priori probability of speech presence (here it is one)
priorFact  = q ./ (1 - q);

xiOptDb    = 15; % optimal fixed a priori SNR for SPP estimation (ξ_opt) in dB
xiOpt      = 10 .^ (xiOptDb./10); % in absolute domain
logGLRFact = log(1 ./ (1 + xiOpt)); 
GLRexp     = xiOpt ./ (1 + xiOpt);

% real frequency bin values
%     f = (0:frameLen/2) * fs / frameLen;

estimatedSpeech = zeros(SIGNAL_LEN,1);
estimatedNoisePow = zeros(FRAME_LEN/2+1, numFrames);

% estimating the noise power based on first 5 frames
noisePow = initialisePSDIdeal(noisySignal, FRAME_LEN, FRAME_LEN, WINDOW_LEN, WINDOW); 

for f = 1:numFrames

    %% FFT
    % signal indices for the given frame
    idx       = (f-1) * WINDOW_LEN + 1 : (f-1) * WINDOW_LEN + FRAME_LEN;
    % getting the frame of a signal by applying the windowing function
    noisyFrame   = WINDOW .* noisySignal(idx);

    % converting to frequency domain
    noisyDftFrame = fft(noisyFrame, FRAME_LEN);
    % leaving only the first half of frequency bins (only below Nyquist
    % frequency??), since the rest is simply the conjugate
    noisyDftFrame = noisyDftFrame(1:FRAME_LEN/2 + 1);

    %% noise power estimation
    % signal power: |Y|^2
    signalPower = noisyDftFrame .* conj(noisyDftFrame);
    % a posteriori SNR based on old noise power estimate: |Y|^2 / s_n^2
    snrPost1 =  signalPower ./ noisePow;

    % SNR max is clamped at 200
    GLR     = priorFact .* exp(min(logGLRFact + GLRexp .* snrPost1, SNR_CLAMP));
    % a posteriori speech presence probability
    PH1     = GLR ./ (1+GLR); 

    % smoothing using the data from the previous step
    PH1mean  = alphaPH1mean * PH1mean + (1 - alphaPH1mean) * PH1;

    % avoiding stagnation
    stuckInd = PH1mean > 0.99;
    PH1(stuckInd) = min(PH1(stuckInd), 0.99);

    % updating noise periodogram estimate
    estimate =  PH1 .* noisePow + (1-PH1) .* signalPower;

    % updating the noise power estimate
    noisePow = alphaPSD * noisePow + (1-alphaPSD) * estimate;

    % nd saving it
    estimatedNoisePow(:,f) = noisePow;

    % spectral subtraction
    SNR = signalPower ./ noisePow;
    gain = sqrt( (SNR + minGain) ./ (SNR + 1));
    noisyDftFrame = gain .* noisyDftFrame;

    % convolution
    inversed = real(ifft([noisyDftFrame; flipud(conj(noisyDftFrame(2:end-1)))], FRAME_LEN));
    estimatedSpeech(idx) = estimatedSpeech(idx) + inversed;
end

return



function noisePsd = initialisePSDIdeal(noisy, frame_size, fft_size, window_len, window)
% computes an initial noise PSD estimate by means of a Bartlett estimate.
% Input parameters:   
%   |noisy|          - noisy signal
%   |frame_size|     - frame size
%   |fft_size|       - fft size
%   |window_len|     - the size of the shifting window
%   |window|         - analysis window (e.g. Hanning)
% Output parameters:  
% |noise_psd_init| - initial noise PSD estimate
% Author: Richard C. Hendriks, 15/4/2010

    % the number of frames to use as an initialization     
    NUM_FRAMES = 5;
    
    for i = 1:NUM_FRAMES
        idx = (i-1)*window_len + 1 : (i-1)*window_len + frame_size;
        noisy_frame = window .* noisy(idx);
        noisy_dft_frame_matrix(:,i) = fft(noisy_frame, fft_size);
    end
    % averaging over the means
    noisePsd = mean(abs(noisy_dft_frame_matrix(1:frame_size/2+1, 1:end)).^2,2);
return


