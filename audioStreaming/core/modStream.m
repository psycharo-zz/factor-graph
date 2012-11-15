function modStream(varargin)
%MODSTREAM stream audio data with optional processing plug-in
%
%  syntax: modStream(varargin)
%
% parameter-value pairs:
%   audioFile - input file name ['bbcnews.wav']
%   plugin    - name of audio filter plugin; use 'copy' for no plugin (=default)
%   target    - ['m'] | 'mex'

% process input args
audioFile = 'test.avi'; % getAudioFile(audioFile);
plugin = 'copy';
target = 'm';
%procArgs(varargin); % overwrite defaults through input args

% set up audio reader and player
hafr = dsp.AudioFileReader(audioFile, ...
  'SamplesPerFrame', 32, ...
  'OutputDataType', 'double');

hap = dsp.AudioPlayer( ...
  'DeviceName','pulse (ALSA)', ...
  'SampleRate',16000, ... % better: hafr.SampleRate
  'BufferSizeSource', 'Property', ...
  'BufferSize', 200, ...
  'QueueDuration', 1);

% get plugin function handles
[createPlugin, stepPlugin] = pluginToFun(plugin);

% create plugin parameters and init states
[p,s] = createPlugin(); 

% create mex
if strcmp(target,'mex')
  oneframe = step(hafr); reset(hafr); % get a frame, useful for mex building
  stepPlugin_build(plugin,oneframe,p,s); % handle to plugin
  stepPlugin = str2func(['step',upper(plugin(1)),plugin(2:end),'_mex']);
end

while ~isDone(hafr)
  x = step(hafr);
  [x,s] = stepPlugin(x,p,s);
  step(hap, x);
end

pause(hap.QueueDuration); % Wait until audio is played to the end
release(hafr);            % close the input file
release(hap);             % close the audio output device

end

function [createPlugin, stepPlugin] = pluginToFun(plugin)
createPlugin = str2func(['create',upper(plugin(1)),plugin(2:end)]);
stepPlugin = str2func(['step',upper(plugin(1)),plugin(2:end)]);
end



