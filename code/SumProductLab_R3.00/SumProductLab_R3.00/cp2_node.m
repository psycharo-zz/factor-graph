classdef cp2_node < cp_node
%CP2 2-port cp node
%   Detailed explanation goes here
% CP2 derived from cp class provides a message to other
% nodes corresponding according to the cpt.
% See CP class for more details.
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
% Before any operation, the index corresponding to the in_msg will be
% located.
% e.g. in_msg = {[0.2 0.8]} and 
% the cpt is defined as follows:
%     %   a,b    P(b|a)
%     cpt(1,1) = 0.5;
%     cpt(1,2) = 0.5;
%     cpt(2,1) = 0.9;
%     cpt(2,2) = 0.1;
% if the in_msg is from the port corresponding to the 2nd index
% then msg(1) = cpt(1,1)*in_msg{1}(1) + cpt(1,2)*in_msg{1}(2)
%             = 0.5*0.2 + 0.5*0.8;
%      msg(2) = cpt(2,1)*in_msg{1}(1) + cpt(2,2)*in_msg{1}(2)
%             = 0.9*0.2 + 0.1*0.8;
% if in_msg is from the port corresponding to the 1st index
% then msg(1) = cpt(1,1)*in_msg{1}(1) + cpt(2,1)*in_msg{1}(2)
%             = 0.5*0.2 + 0.9*0.8;
%      msg(2) = cpt(1,2)*in_msg{1}(1) + cpt(2,2)*in_msg{1}(2)
%             = 0.5*0.2 + 0.1*0.8;


   properties
   end

   methods
       function s = cp2_node(id)
           s = s@cp_node(id);
       end

       function msg = factor_fun(s, in_msg, from_id, to_id)
           if isempty(s.cpt)
               sprintf('Node %0.0f ERROR from factor_fun: empty cpt !!!\n',s.id)
           elseif size(in_msg,1) ~= 1
               sprintf('Node %0.0f ERROR from factor_fun: size of linklist not equal to 2 !!!\n',s.id)
           else
               % discover the order of message
               % index for summarization
               sum_var = find(s.link_id == to_id);
               % index for var1
               var1 = find(s.link_id == from_id);
               idx = ones(1,2);
               msg_size = size(s.cpt,sum_var);
               msg = zeros(1,msg_size);
               for i = 1:msg_size
                   idx(sum_var) = i;
                   for j = 1:size(in_msg{1},2)
                       idx(var1) = j;
                       msg(i) = msg(i) + s.cpt(idx(1),idx(2))*in_msg{1}(j);
                   end % for j
               end % for i
               sum_msg = sum(msg);
               if sum_msg == 0
                   msg = ones(size(msg));
               end
               msg = msg/sum(msg);
           end
       end % function msg = factor_fun
   end % methods
end 
