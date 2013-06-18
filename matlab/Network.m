classdef Network < CppObject
    %NETWORK algonquin network
  
    methods
        function this = Network(type)
            if nargin == 0
                type = 'Network';
            end
            this = this@CppObject(type);
        end
        
        function [speechPrior, noisePrior] = train(this, speech, noise)
            [speechPrior, noisePrior] = mexfactorgraph('train', this.type_name, this.cpp_handle, speech, noise);
        end
        
        function setPriors(this, speech, noise)
            mexfactorgraph('setPriors', this.type_name, this.cpp_handle, speech, noise);
        end
        
        function process(this, frame)
            mexfactorgraph('process', this.type_name, this.cpp_handle, frame);
        end
            
    end
  
   
end

