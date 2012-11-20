#include <gtest/gtest.h>

#include "tests/kalmanfilter_test.h"
#include "tests/equalitynode_test.h"
#include "tests/evidencenode_test.h"
#include "tests/addnode_test.h"
#include "tests/schedule_test.h"
#include "tests/multiplicationnode_test.h"
#include "tests/esitmatemultiplicationnode_test.h"
#include "tests/ar_test.h"
#include "tests/matrixutil_test.h"
#include "tests/util_test.h"
//#include "factorgraph.h"


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

