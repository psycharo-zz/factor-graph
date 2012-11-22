classdef CustomNode < ffg.FactorNode
    %CUSTOMNODE Base class for customized nodes
    %   
    %
    
    methods
        function this = CustomNode()
            this = this@ffg.FactorNode('CustomNode');            
        end
        
        function setFunction(this, functionName)
            mexfactorgraph('setFunction', this.type_name, this.cpp_handle, functionName);
        end
    end
    
end

