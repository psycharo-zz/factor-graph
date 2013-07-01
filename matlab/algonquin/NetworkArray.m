classdef NetworkArray < CppObject
    %NETWORKARRAY Summary of this class goes here
    %   Detailed explanation goes here
    
    %NETWORK algonquin network
  
    methods
        function this = NetworkArray(numFreqs)
            type = 'NetworkArray';
            this = this@CppObject(type, mexfactorgraph('create', type, numFreqs));
        end
        
        function train(this, speech, numSpeech, noise, numNoise, numIters)
            % TODO: preprocessing of frames?             
            mexfactorgraph('train', this.type_name, this.cpp_handle, speech, numSpeech, noise, numNoise, numIters)
        end
        
        function [logS, logN] = process(this, frame)
            [logS, logN] = mexfactorgraph('process', this.type_name, this.cpp_handle, frame);
        end
        
        function [speech] = speechDistr(this, num)
            speech = mexfactorgraph('speechDistr', this.type_name, this.cpp_handle, num-1);
        end
        
        function [noise] = noiseDistr(this, num)
            noise = mexfactorgraph('noiseDistr', this.type_name, this.cpp_handle, num-1);
        end
            
    end
end

