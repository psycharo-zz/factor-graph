classdef EquMultNode < ffg.FactorNode
    %EQUMULTNODE compound node, (Table 4, Loeliger 2007)
    
    methods
        function this = EquMultNode()
            this = this@ffg.FactorNode('EquMultNode');            
        end

        function setMatrix(this, matrix)
            % (ffg) set the matrix of the multiplication subnode
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end

