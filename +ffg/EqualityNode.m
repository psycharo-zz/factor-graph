classdef EqualityNode < ffg.FactorNode
    %EQUALITYNODE Equality factor node. Gaussian messages only. (Table 2, Loeliger 2007)
    methods
       function this = EqualityNode(nwk)
           this = this@ffg.FactorNode('EqualityNode', nwk);
       end
       
       
       function setType(this, type)
           % (ffg) set the type of the node. currently supported types are
           % 'VARIANCE' and 'PRECISION'. 
           mexfactorgraph('setType', this.type_name, this.cpp_handle, type);
       end
       
       function result = getType(this)
           % (ffg) get the type of the node
           result = mexfactorgraph('type', this.type_name, this.cpp_handle);
       end
    end
end

