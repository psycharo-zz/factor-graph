
nwk = trainNetwork;

P = defaultParams;
[speech, freqS] = audioread('bbcnews.wav'); 
speechLog = logPowerDomain(speech, P)';
frame = speechLog(1,:);

nwk.process(frame)

