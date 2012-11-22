classdef EquMultNode < ffg.FactorNode
    %EQUMULTNODE compound node
    
    properties
    end
    
    methods
        function this = EquMultNode()
            this = this@ffg.FactorNode('EquMultNode');            
        end

        function setMatrix(this, matrix)
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end

