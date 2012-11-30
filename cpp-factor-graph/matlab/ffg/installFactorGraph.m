function installFactorGraph

    disp('Building factor graph framework for your platform...');
    
    % clearing in case it hasn't been cleared before
    clear mexfactorgraph;
    
    MATLAB_FULL_PATH = fileparts(which(mfilename));
    APP_FULL_PATH = fileparts(fileparts(MATLAB_FULL_PATH));

    addpath(genpath(APP_FULL_PATH));

    OUT_NAME = 'mexfactorgraph';
    SRC_PATH = [APP_FULL_PATH filesep 'src'];
    MATLAB_SRC_PATH = [MATLAB_FULL_PATH filesep 'src'];
    BUILD_PATH = [MATLAB_FULL_PATH filesep 'build'];

    
    if isunix
        MEXOPTS = [' -f ' BUILD_PATH filesep 'mexopts.sh'  ' -DMATLAB'];
    elseif  ispc
        MEXOPTS = '-lmwblas -lmwlapack -DMATLAB';
    else 
        error('installFactorGraph:Unsupported platform');
    end
    
    % core sources
    sources = cell2mat(arrayfun(@(x) struct('name', ['"' SRC_PATH filesep x.name '" ']), dir([SRC_PATH filesep '*.cpp']), 'UniformOutput', false));
    SOURCES_NAMES = [sources.name];
    sources = cell2mat(arrayfun(@(x) struct('name', ['"' MATLAB_SRC_PATH filesep x.name '" ']), dir([MATLAB_SRC_PATH filesep '*.cpp']), 'UniformOutput', false));
    SOURCES_NAMES = [SOURCES_NAMES, sources.name];
    eval(['mex ' SOURCES_NAMES ' -I"' SRC_PATH '" ' MEXOPTS ' -output ' OUT_NAME]);
    
    disp('Done!');
    
    runtests ffg.tests;
    
    % TODO: testing?
end