classdef EstimateMultiplicationNode < FactorNode
    %ESTIMATEMULTIPLICATIONNODE used for estimating parameters
    %   
    
    properties
    end
    
    methods
        function this = EstimateMultiplicationNode()
            this = this@FactorNode('EstimateMultiplicationNode');            
        end

        function setMatrix(this, matrix)
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end
