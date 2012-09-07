classdef equ_node < factor_node
%EQU equ node with no pre-defined number of port
%   EQU is a subclass of factor node.  It functions as EQU3 but for more
%   than three ports.
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
% then msg = in_msg{1}.*in_msg{2}.* .....

   properties
   end

   methods
       function s = equ_node(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           msg = ones(1,size(in_msg,2));
           for i = 1:size(in_msg,1)
               msg = msg.*in_msg{i};
           end
           sum_msg = sum(msg);
           if sum_msg == 0
               msg = ones(size(msg));
           end
           msg = msg/sum(msg);
       end

   end
end
