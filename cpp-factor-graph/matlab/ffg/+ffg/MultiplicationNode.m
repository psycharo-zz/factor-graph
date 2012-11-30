classdef MultiplicationNode < ffg.FactorNode
    %MULTIPLICATIONNODE multiplication-by-matrix node. (Table 3, Loeliger 2007)
    
    methods
        function this = MultiplicationNode()
            this = this@ffg.FactorNode('MultiplicationNode');            
        end

        function setMatrix(this, matrix)
            % (ffg) set the matrix of the node
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end

