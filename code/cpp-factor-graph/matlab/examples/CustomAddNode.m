classdef CustomAddNode < CustomNode
%CUSTOMADDNODE an example of how custom nodes should be implemented

    methods
        function this = CustomAddNode
            this = this@CustomNode;
            this.setFunction('customnode_function_gauss');
        end
    end
    
end

