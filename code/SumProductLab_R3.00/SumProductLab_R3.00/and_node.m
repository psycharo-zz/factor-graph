classdef and_node < factor_node
%AND_NODE, a bivalue node & subclass of factor_node, constraints the child
%and parent connections to behave as a and gate such that the child message
%become false when any of the parent message is false. 
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
% if message is directed to child,
%   msg(2) = product of all in_msg{i}(2)
%   msg(1) = 1 - msg(2)
% if message is directed to parent
%  msg(2) = product of all in_msg{i}(2) except the message from child
%  msg(2) = child_msg(1) + msg(1)*(child_msg(2) - child_msg(1))
%  msg(1) = child_msg(1)

   properties
   end

   methods
       function s = and_node(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           msg = [0 0];
           child_id = s.link_id(size(s.link_id,2));
           if to_id == child_id
               % construct message to child
               msg(2) = 1;
               for i = 1:size(s.link_id,2)-1
                   msg(2) = msg(2)*in_msg{i}(2);
               end
               msg(1) = 1 - msg(2);
           else
               % construct message to one of the parents
               % identify message from child
               child_idx = find(from_id==child_id);
               child_msg = in_msg{child_idx};
               msg(2) = 1;
               for i = 1:size(s.link_id,2)-1
                   if from_id(i) == child_id
                       continue;
                   end
                   msg(2) = msg(2)*in_msg{i}(2);
               end
               msg(2) = child_msg(1) + msg(2)*(child_msg(2) - child_msg(1));
               msg(1) = child_msg(1);
           end
           sum_msg = sum(msg);
           if sum_msg == 0
               msg = ones(size(msg));
           end
           msg = msg/sum(msg);
       end

   end
end 
