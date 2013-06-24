classdef Network < CppObject
    %NETWORK algonquin network
  
    methods
        function this = Network(type)
            if nargin == 0
                type = 'Network';
            end
            this = this@CppObject(type);
        end
        
        function [speechPrior, noisePrior] = train(this, speech, numSpeechComps, noise, numNoiseComps, maxNumIters)
            [speechPrior, noisePrior] = mexfactorgraph('train', this.type_name, this.cpp_handle, ...
                                                       speech, numSpeechComps, ...
                                                       noise, numNoiseComps, maxNumIters);
        end
        
        function setDistributions(this, speech, noise)
            mexfactorgraph('setDistrs', this.type_name, this.cpp_handle, speech, noise);
        end
        
        function [speech, noise] = distrs(this)
            [speech, noise] = mexfactorgraph('distrs', this.type_name, this.cpp_handle);
        end
        
        function [logPowS, logPowN] = process(this, frame)
            [logPowS, logPowN] = mexfactorgraph('process', this.type_name, this.cpp_handle, frame);
        end
            
    end
  
   
end

