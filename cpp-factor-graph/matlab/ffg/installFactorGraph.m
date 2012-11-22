function installFactorGraph
    
    MATLAB_FULL_PATH = fileparts(which(mfilename));
    APP_FULL_PATH = fileparts(fileparts(MATLAB_FULL_PATH));
    
    addpath(APP_FULL_PATH);

    OUT_NAME = 'mexfactorgraph';
    SRC_PATH = [APP_FULL_PATH filesep 'src'];
    MATLAB_SRC_PATH = [MATLAB_FULL_PATH filesep 'src'];
    BUILD_PATH = [MATLAB_FULL_PATH filesep 'build'];
    
   
    MEXOPTS_PATH = BUILD_PATH;
    if isunix
        MEXOPTS_PATH = [MEXOPTS_PATH filesep 'mexopts.sh'];
    elseif  ispc
        MEXOPTS_PATH = [MEXOPTS_PATH filesep 'mexopts.bat'];
    else 
        error('installFactorGraph:Unsupported platform');
    end
    
    % core sources
    sources = dir([SRC_PATH filesep '*.cpp']);
    SOURCES_NAMES = sprintf([SRC_PATH filesep '%s\t'], sources(1:end).name);
    % matlab support sources
    sources = dir([MATLAB_SRC_PATH filesep '*.cpp']);
    SOURCES_NAMES = [SOURCES_NAMES, sprintf([MATLAB_SRC_PATH filesep '%s\t'], sources(1:end).name)];
            
    
    eval(sprintf('mex -f %s -I%s %s -output %s', MEXOPTS_PATH, SRC_PATH, SOURCES_NAMES, OUT_NAME));
    
    
    
end