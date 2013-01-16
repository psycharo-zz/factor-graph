classdef MultiplicationNode < ffg.FactorNode
    %MULTIPLICATIONNODE multiplication-by-matrix node. (Table 3, Loeliger 2007)
    
    methods
        function this = MultiplicationNode(nwk)
            this = this@ffg.FactorNode('MultiplicationNode', nwk);            
        end

        function setMatrix(this, matrix)
            % (ffg) set the matrix of the node
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
        
        
        function result = matrix(this)
            % (ffg) get the matrix
            result = mexfactorgraph('matrix', this.type_name, this.cpp_handle);
        end
    end
    
end

