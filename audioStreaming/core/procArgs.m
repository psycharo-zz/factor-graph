function procArgs(vars, prefix)
% procArgs(vars=cell array, [prefix=string])
% Set the values of named arguments passed to your function (as in plot(), etc).
% vars is a cell array of names and values, prefix is an optional text
% string to prepend onto each name (useful for setting values in a
% structure).
% procArgs allows you to write functions like this:
%   inputfun('prompt', '>', 'defval', 5.1, 'timeout', 5);
% Usage: (1) in your function define vars with default values; (2) call procargs(varargin)
% Minimal error checking: requires that all passed variable names match
% already defined variables in your function.
% see end of file for examples.
% V1.7 (c) 2010 Alan Robinson (cogsci.ucsd.edu/~aerobins)

if ~exist('prefix', 'var')
    prefix = '';
end

if (mod(length(vars),2) == 0) % must be an even number of args
   for (i=1:2:length(vars))
      arg = [prefix vars{i}];    % assign readable names
      val =  vars{i+1};
     if evalin('caller', ['exist(''' arg ''',''var'');'])  % make sure var exists in caller
            assignin('caller', arg, val);
     else
          index = strfind(arg, '.');
          assigned = 0;
          if index % it's a structure
             if evalin('caller', ['exist(''' arg(1:index-1) ''',''var'');']) 
                 if  evalin('caller', ['isfield(' arg(1:index-1) ',''' arg(index+1:end) ''');'])
                     evalin('caller', [arg '=[' num2str(val) '];']); % only numbers supported
                     assigned = 1;
                 end
             end
          end
          
          if ~assigned                 
              error([arg ' is not defined! Assign default value first before calling procArgs.']);
          end
      end
   end

else
   error(['Args must come in pairs (actual size: ' num2str(size(vars)) ')']);
end

% %% example function using procargs
% function test(varargin)
% name = 'John';
% age = 32;
% procArgs(varargin)
% fprintf('name=%s, age=%i\n', name, age);
% 
% %% example calls
% test('name', 'Amber'); 
% test('age', 99);
% test('Age', 0); % error, will report that Age is  not defined.