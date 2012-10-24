classdef CustomNode < FactorNode
    %CUSTOMNODE Summary of this class goes here
    %   Detailed explanation goes here
    
    methods
        function this = CustomNode()
            this = this@FactorNode('customnode');
        end
        
        function setFunction(this, functionName)
            mexfactorgraph('setFunction', this.type_name, this.cpp_handle, functionName);
        end
    end
    
end

