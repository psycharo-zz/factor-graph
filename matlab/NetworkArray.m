classdef NetworkArray < CppObject
    %NETWORKARRAY Summary of this class goes here
    %   Detailed explanation goes here
    
    %NETWORK algonquin network
  
    methods
        function this = NetworkArray(numFreqs)
            type = 'NetworkArray';
            this = this@CppObject(type, mexfactorgraph('create', type, numFreqs));
        end
        
        function train(this, speech, noise)
            % TODO: preprocessing of frames?             
            mexfactorgraph('train', this.type_name, this.cpp_handle, speech, noise)
        end
        
        function process(this, frame)
            mexfactorgraph('process', this.type_name, this.cpp_handle, frame)
        end
            
    end
end

