function output = denoiseWienerFilter(noisySpeech, speech, noise)
%DENOISEWIENER denoising using wiener filter that uses local (NOTE: the local information is used)

% the whole signal is splitted into frames of FRAME_LEN, overlapping by
% WINDOW_LEN samples, each of those frames is then converted to smaller
% subframes of size SUBFRAME_LEN with overlapping of SUBWINDOW_LEN
% then each subframe is zero-padded up to 256 samples, a Hanning window
% is applied, and then fft is taken.

% The noise and speech power are averaged over the top-level frame, and
% the estimate of the clean signal is made using this estimate

FRAME_LEN = 800;
OVERLAP_LEN = 400;
SUBFRAME_LEN = 200;
SUBOVERLAP_LEN = 100;
FFT_SIZE = 256;

% the Hanning windows 
WINDOW = hanning(FRAME_LEN);
WINDOW_SUB = hanning(SUBFRAME_LEN);

NUM_SAMPLES = length(noisySpeech);

% TODO: fill the rem with zeros?
numFrames = floor(NUM_SAMPLES / OVERLAP_LEN) - 1;
% the number of subframes in a frame
numSubFrames = floor(FRAME_LEN / SUBOVERLAP_LEN) - 1;


output = zeros(NUM_SAMPLES, 1);

for f = 1:numFrames-1
    idx = (f-1) * OVERLAP_LEN + 1 : (f-1) * OVERLAP_LEN + FRAME_LEN;
    
    frameS = WINDOW .* speech(idx);
    frameN = WINDOW .* noise(idx);
   
    % average power for each frequency
    powS = zeros(FFT_SIZE,1);
    powN = zeros(FFT_SIZE,1);
    
    % collecting     
    for sf = 1:numSubFrames
        subidx = (sf-1) * SUBOVERLAP_LEN + 1 : (sf-1) * SUBOVERLAP_LEN + SUBFRAME_LEN;
        % it will be automatically padded with zeros  
        coefsS = fft(WINDOW_SUB .* frameS(subidx), FFT_SIZE);
        coefsN = fft(WINDOW_SUB .* frameN(subidx), FFT_SIZE);
        
        powS = powS + coefsS .* conj(coefsS);
        powN = powN + coefsN .* conj(coefsN);
    end
    powS = powS ./ numSubFrames;
    powN = powN ./ numSubFrames;
    
    % the gain based on average speech and noise powers    
    gain = powS ./ (powS + powN);
    
    % denoising each subframe
    frame = WINDOW .* noisySpeech(idx);
    cleanFrame = zeros(FRAME_LEN, 1);
    for sf = 1:numSubFrames
        subidx = (sf-1) * SUBOVERLAP_LEN + 1 : (sf-1) * SUBOVERLAP_LEN + SUBFRAME_LEN;
        % spectral subtraction         
        coefs = gain .* fft(WINDOW_SUB .* frame(subidx), FFT_SIZE);
        cleanSubFrame = ifft(coefs, FFT_SIZE);
        % subframe add-overlap         
        cleanFrame(subidx) = cleanFrame(subidx) + cleanSubFrame(1:SUBFRAME_LEN);
    end
    % frame add-overlap
    output(idx) = output(idx) + cleanFrame(1:FRAME_LEN);
end


return

