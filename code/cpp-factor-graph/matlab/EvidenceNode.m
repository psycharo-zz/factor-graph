classdef EvidenceNode < FactorNode
    %EVIDENCENODE Summary of this class goes here
    %   Detailed explanation goes here
   
    methods
       function this = EvidenceNode()
           this = this@FactorNode('evidencenode');
       end
      
       function setInitial(this, message)
           mexfactorgraph('setInitial', this.type_name, this.cpp_handle, message);
       end

       function setDest(this, node)
           mexfactorgraph('setDest', this.type_name, this.cpp_handle, node.cpp_handle);
       end

       function res = evidence(this)
           res = mexfactorgraph('evidence', this.type_name, this.cpp_handle);
       end
    end
    
end

