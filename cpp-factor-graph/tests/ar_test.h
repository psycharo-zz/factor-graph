#ifndef AR_TEST_H
#define AR_TEST_H

#include <factorgraph.h>


TEST(AutoRegressive, One) {

   normal_distribution<double> randn(0, 1);
   default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

   auto x_prev = new EvidenceNode;
   auto x_next = new EvidenceNode;
   auto y = new EvidenceNode;
   auto noiseB = new EvidenceNode;
   auto B = new MultiplicationNode;
   auto A = new MultiplicationNode;
   auto addAB = new AddNode;
   auto equ = new EqualityNode;
   auto C = new MultiplicationNode;
   auto noiseC = new EvidenceNode;
   auto addCN = new AddNode;



   Network nwk;

   nwk.addEdge(x_prev, A);
   nwk.addEdge(A, addAB);

   nwk.addEdge(noiseB, B);
   nwk.addEdge(B, addAB);

   // = connections
   nwk.addEdge(addAB, equ);
   nwk.addEdge(equ, x_next);
   nwk.addEdge(equ, C);


   nwk.addEdge(C, addCN);
   nwk.addEdge(noiseC, addCN);
   nwk.addEdge(addCN, y);


   Network::Schedule schedule = {
    x_prev, A, // 1
    A, addAB,  // 2
    addAB, equ, // 3
    addCN, C,   // 4
    equ, x_next, // 5
    equ, addAB, // 6
    addAB, A,   // 7


   };


}


#endif // AR_TEST_H
