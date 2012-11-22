classdef EstimateMultiplicationNode < ffg.FactorNode
    %ESTIMATEMULTIPLICATIONNODE used for estimating parameters
    %   
    
    properties
    end
    
    methods
        function this = EstimateMultiplicationNode()
            this = this@ffg.FactorNode('EstimateMultiplicationNode');            
        end

        function setMatrix(this, matrix)
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end
