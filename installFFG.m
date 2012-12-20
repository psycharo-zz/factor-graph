function installFFG

    disp('Building factor graph framework for your platform...');
    
    % clearing in case it hasn't been cleared before
    clear mexfactorgraph;
    
    APP_FULL_PATH = fileparts(which(mfilename));

    addpath(genpath(APP_FULL_PATH));

    OUT_NAME = 'mexfactorgraph';
    SRC_PATH = [APP_FULL_PATH filesep 'src'];
    BUILD_PATH = [APP_FULL_PATH filesep 'build'];

    
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
    eval(['mex ' SOURCES_NAMES ' -I"' SRC_PATH '" ' MEXOPTS ' -output ' [APP_FULL_PATH filesep  OUT_NAME]]);
    
    disp('Done!');

    % testing
    runtests ffg.tests;
    
end