classdef customnode < factornode
    %CUSTOMNODE Summary of this class goes here
    %   Detailed explanation goes here
    
    methods
        function this = customnode(id)
            this = this@factornode(id, 'customnode');
        end
    end
    
end

