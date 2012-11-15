classdef EquMultNode < FactorNode
    %EQUMULTNODE compound node
    
    properties
    end
    
    methods
        function this = EquMultNode()
            this = this@FactorNode('EquMultNode');            
        end

        function setMatrix(this, matrix)
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end

