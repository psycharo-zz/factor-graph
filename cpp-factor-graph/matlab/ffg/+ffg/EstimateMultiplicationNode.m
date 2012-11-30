classdef EstimateMultiplicationNode < ffg.FactorNode
    %ESTIMATEMULTIPLICATIONNODE used for estimating parameters. 
    % Incoming and outgoing connections the same as in MULTIPLICATIONNODE, 
    % for 'estimate' tag see parameter estimation in (Korl 2004)
    
    methods
        function this = EstimateMultiplicationNode()
            this = this@ffg.FactorNode('EstimateMultiplicationNode');            
        end

        function setMatrix(this, matrix)
            % (ffg) set the matrix 
            mexfactorgraph('setMatrix', this.type_name, this.cpp_handle, matrix);
        end
    end
    
end
