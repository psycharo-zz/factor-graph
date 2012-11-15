classdef EqualityNode < FactorNode
    %EQUALITYNODE Equality factor node. Gaussian
    methods
       function this = EqualityNode()
           this = this@FactorNode('EqualityNode');
       end
       
       function setType(this, type)
           mexfactorgraph('setType', this.type_name, this.cpp_handle, type);
       end
    end
end

