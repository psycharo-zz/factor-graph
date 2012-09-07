classdef not2_node < factor_node
%NOT2_NODE is a bivalue node & subclass of factor_node, constraints the two
%input to be complement to each other
%   Detailed explanation goes here

   properties
   end

   methods
       function s = not2_node(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           msg = [in_msg{1}(2) in_msg{1}(1)];
           sum_msg = sum(msg);
           if sum_msg == 0
               msg = ones(size(msg));
           end
           msg = msg/sum(msg);
       end
   end
end 
