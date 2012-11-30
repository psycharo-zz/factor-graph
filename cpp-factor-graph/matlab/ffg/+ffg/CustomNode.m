classdef CustomNode < ffg.FactorNode
    %CUSTOMNODE Base class for customized nodes
    % use method SETFUNCTION to set the custom function
    % see examples for function signature and description
    
    methods
        function this = CustomNode()
            this = this@ffg.FactorNode('CustomNode');            
        end
        
        function setFunction(this, functionName)
            % (ffg) set the name of the custom function
            mexfactorgraph('setFunction', this.type_name, this.cpp_handle, functionName);
        end
    end
    
end

