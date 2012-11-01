classdef CustomNode < FactorNode
    %CUSTOMNODE Base class for customized nodes
    %   Detailed explanation goes here
    
    methods
        function this = CustomNode()
            this = this@FactorNode('CustomNode');            
        end
        
        function setFunction(this, functionName)
            mexfactorgraph('setFunction', this.type_name, this.cpp_handle, functionName);
        end
    end
    
end

