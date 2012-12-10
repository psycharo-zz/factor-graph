classdef AddNode < ffg.FactorNode
    %ADDNODE Adding factor node. Gaussian messages only. (Table 2, Loeliger 2007)
    
    methods
       function this = AddNode()
           this = this@ffg.FactorNode('AddNode');
       end
    end
    
end

