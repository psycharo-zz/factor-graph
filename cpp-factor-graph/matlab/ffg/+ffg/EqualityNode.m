classdef EqualityNode < ffg.FactorNode
    %EQUALITYNODE Equality factor node. Gaussian
    methods
       function this = EqualityNode()
           this = this@ffg.FactorNode('EqualityNode');
       end
       
       % set the type
       function setType(this, type)
           mexfactorgraph('setType', this.type_name, this.cpp_handle, type);
       end
       
       % get the type string repr
       function result = getType(this)
           result = mexfactorgraph('type', this.type_name, this.cpp_handle);
       end
    end
end

