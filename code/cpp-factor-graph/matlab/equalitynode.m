classdef equalitynode < factornode
    methods
       function this = equalitynode(id)
           this = this@factornode(id, 'equalitynode');
       end
       
       function setConnections(this, a, b, c)
           mexfactorgraph('setConnections', this.type_name, this.cpp_handle, a.cpp_handle, b.cpp_handle, c.cpp_handle);
       end
    end
end

