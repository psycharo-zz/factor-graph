function stepPlugin_build(plugin,x,p,s)
%STEPPLUGIN_BUILD builds stepPlugin mex file
%  syntax: stepPlugin_build(plugin,x,p,s)

oldDir = pwd;
cd(fileparts(mfilename('fullpath')));

clear mex
target = 'mex';
cfg = coder.config(target);
cfg.GenerateReport = true;

cfg.DynamicMemoryAllocation = 'Threshold';
cfg.DynamicMemoryAllocationThreshold = 500;
cfg.EchoExpressions = false;
cfg.IntegrityChecks = false; % checks memory integrity violations [def. true] 
cfg.ResponsivenessChecks = false; % periodic checks for Ctrl+C breaks [true]
cfg.ExtrinsicCalls = false; % Extrinsic calls to MATLAB functions [true]

stepPlugin = ['step',upper(plugin(1)),plugin(2:end)];
eval(['codegen -config cfg ', stepPlugin, ' -args {x,p,s}']);

cd(oldDir)

