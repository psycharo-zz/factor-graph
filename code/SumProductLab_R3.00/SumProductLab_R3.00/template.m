classdef template < factor_node

   properties
       % add your properties here
   end

   methods
       function s = template(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           % add your code here
           % ..................
           
           % re-scale msg
           sum_msg = sum(msg);
           if sum_msg == 0
               msg = ones(size(msg));
           end
           msg = msg/sum(msg);
       end
   end
end
